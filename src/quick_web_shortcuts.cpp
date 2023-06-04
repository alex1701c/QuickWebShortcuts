#include "quick_web_shortcuts.h"
#include <QtGui/QtGui>
#include <QAction>
#include <KShell>
#include <KNotifications/KNotification>
#include "searchengines/SearchEngines.h"
#include "Config.h"
#include "utilities.h"
#include <searchproviders/Bing.h>
#include <searchproviders/Google.h>
#include <searchproviders/DuckDuckGo.h>

QuickWebShortcuts::QuickWebShortcuts(QObject *parent, const KPluginMetaData &pluginMetaData, const QVariantList &args)
        : KRunner::AbstractRunner(parent, pluginMetaData, args) {
}

QuickWebShortcuts::~QuickWebShortcuts() {
    filterHistory();
    delete requiredData.proxy;
}

void QuickWebShortcuts::init() {
    initializeConfigFile();

    // Add file watcher for config
    watcher.addPath(QDir::homePath() + QStringLiteral("/.config/krunnerplugins/") + Config::ConfigFile);
    connect(&watcher, &QFileSystemWatcher::fileChanged, this, &QuickWebShortcuts::reloadPluginConfiguration);
    connect(this, &QuickWebShortcuts::teardown, this, &QuickWebShortcuts::filterHistory);

    reloadPluginConfiguration();
}

void QuickWebShortcuts::reloadPluginConfiguration(const QString &configFile) {
    // To detect invalid state if config is updated
    currentSearchEngine.url.clear();
    KConfigGroup configGroup = KSharedConfig::openConfig(
            QDir::homePath() + QStringLiteral("/.config/krunnerplugins/") + Config::ConfigFile)
            ->group(Config::RootGroup);
    // Force sync from file
    if (!configFile.isEmpty()) {
        configGroup.config()->reparseConfiguration();
    }

    // If the file gets edited with a text editor, it often gets replaced by the edited version
    // https://stackoverflow.com/a/30076119/9342842
    if (!configFile.isEmpty()) {
        if (QFile::exists(configFile)) {
            watcher.addPath(configFile);
        }
    }

    privateBrowser = loadPrivateBrowser();

    if (configGroup.readEntry(Config::WebShortcut).isEmpty()) {
        // Load search engines
        const QString searchEngineName = configGroup.readEntry(Config::SearchEngineName);
        for (auto &engine:SearchEngines::getAllSearchEngines()) {
            if (engine.name == searchEngineName) {
                currentSearchEngine = engine;
                break;
            }
        }

        // If the config is empty or malformed
        if (currentSearchEngine.url.isEmpty()) {
            currentSearchEngine = getDefaultSearchEngine();
        }
        isWebShortcut = false;
    } else {
        const KConfigGroup webShortcutGroup = KSharedConfig::openConfig(configGroup.readEntry(Config::WebShortcut))->group("Desktop Entry");
        if (webShortcutGroup.exists()) {
            SearchEngine webShortcutEngine;
            webShortcutEngine.qIcon = QIcon::fromTheme(QStringLiteral("globe"));
            webShortcutEngine.name = webShortcutGroup.readEntry("Name");
            webShortcutEngine.url = webShortcutGroup.readEntry("Query");
            currentSearchEngine = webShortcutEngine;
            isWebShortcut = true;
        } else {
            currentSearchEngine = getDefaultSearchEngine();
            isWebShortcut = false;
        }
    }

    // Load general settings
    if (!configGroup.readEntry(Config::ShowSearchForNote, true)) {
        searchOptionTemplate = QStringLiteral("%1");
    } else if (configGroup.readEntry(Config::ShowName, false)) {
        searchOptionTemplate = QStringLiteral("Search ") + currentSearchEngine.name + QStringLiteral(" for %1");
    } else {
        searchOptionTemplate = QStringLiteral("Search for %1");
    }

    // Display text for private browser option
    if (configGroup.readEntry(Config::PrivateWindowNote, true)) {
        privateBrowserMode = privateBrowser.contains(QLatin1String("private")) ?
                             QStringLiteral(" in private window") : QStringLiteral(" in incognito mode");
    } else {
        privateBrowserMode = QString();
    }
    triggerCharacter = configGroup.readEntry(Config::TriggerCharacter, Config::TriggerCharacterDefault);
    privateWindowTrigger = triggerCharacter + triggerCharacter;

    // Search suggestions settings
    searchSuggestionChoice = configGroup.readEntry(Config::SearchSuggestions, Config::SearchSuggestionDisabled);
    searchSuggestions = searchSuggestionChoice != Config::SearchSuggestionDisabled;
    privateWindowSearchSuggestions = searchSuggestions && configGroup.readEntry(Config::PrivateWindowSearchSuggestions, false);
    if (configGroup.readEntry(Config::PrivateWindowAction, true)) {
        normalActions = {new QAction(QIcon::fromTheme(QStringLiteral("view-private")),
                                   QStringLiteral("launch query in private/incognito window"),
                                   this)
        };
    } else {
        normalActions.clear();
    }

    // RequiredData is for all search providers required and does not need to be updated
    // outside of the reloadConfiguration method
    if (searchSuggestions) {
        minimumLetterCount = configGroup.readEntry(Config::MinimumLetterCount, Config::MinimumLetterCountDefault);
        bingMarket = configGroup.readEntry(Config::BingMarket, Config::BingMarketDefault);
        googleLocale = configGroup.readEntry(Config::GoogleLocale, Config::GoogleLocaleDefault);
        requiredData.searchEngine = currentSearchEngine.url;
        requiredData.icon = currentSearchEngine.qIcon;
        requiredData.runner = this;
        requiredData.maxResults = configGroup.readEntry(Config::MaxSuggestionResults, Config::MaxSuggestionResultsDefault);
        requiredData.searchOptionTemplate = searchOptionTemplate;
        requiredData.isWebShortcut = isWebShortcut;
    }

    // Proxy settings
#ifndef NO_PROXY_INTEGRATION
    delete requiredData.proxy;
    requiredData.proxy = getProxyFromConfig(configGroup.readEntry(Config::Proxy, Config::ProxyDisabled));
    requiredData.showNetworkErrors = configGroup.readEntry(Config::ProxyShowErrors, true);
#else
    requiredData.proxy = nullptr;
#endif

    // History settings
    QString historyChoice = configGroup.readEntry(Config::CleanHistory, Config::CleanHistoryDefault);
    cleanAll = historyChoice == Config::CleanHistoryAll;
    cleanQuick = historyChoice == Config::CleanHistoryQuick;
    cleanNone = historyChoice == Config::CleanHistoryNone;

    // Initialize variables only if needed
    if (!cleanNone) {
        generalKrunnerConfig = KSharedConfig::openConfig(QStringLiteral("krunnerrc"))->group("General");
        removeHistoryRegex = QRegularExpression(QStringLiteral(R"([a-z]{1,5}: ?[^,]+,?)"));
        removeHistoryRegex.optimize();
    }
}

void QuickWebShortcuts::filterHistory() {
    if (cleanNone || (cleanQuick && !wasActive)) return;
    QString history = generalKrunnerConfig.readEntry("history");
    const int initialHistorySize = history.size();
    QString filteredHistory;
    // Clears the normal web shortcuts, they use ":" as a delimiter between key and value
    if (cleanAll) {
        filteredHistory = history.remove(removeHistoryRegex);
    }
    if (cleanAll || cleanQuick) {
        // If cleanAll is true, filtered history has already been set => read value, clear it and write the final result
        const QString toFilter = cleanAll ? filteredHistory : history;
        if (cleanAll) filteredHistory = "";
        const QChar sep = ',';
        for (const auto &item : toFilter.split(sep, Qt::SkipEmptyParts)) {
            if (!item.startsWith(triggerCharacter)) {
                filteredHistory += item + sep;
            }
        }
    }
    // No changes have been made, exit function
    if (filteredHistory.size() == initialHistorySize) return;

    // Write changes and make sure that the config gets synced
    generalKrunnerConfig.writeEntry("history", filteredHistory);
    generalKrunnerConfig.sync();
}

void QuickWebShortcuts::match(KRunner::RunnerContext &context) {
    if (!context.isValid()) return;
    wasActive = false;

    // Remove escape character
    QString term = QString(context.query()).replace(QString::fromWCharArray(L"\u001B"), " ");

    QMap<QString, QVariant> data;

    if (term.startsWith(privateWindowTrigger)) {
        term = term.mid(2);
        data.insert(QStringLiteral("browser"), privateBrowser);
        QString url;
        if (isWebShortcut) {
            url = QString(currentSearchEngine.url).replace(QStringLiteral("\\{@}"), QUrl::toPercentEncoding(term));
        } else {
            url = currentSearchEngine.url + QUrl::toPercentEncoding(term);
        }
        data.insert(QStringLiteral("url"), url);
        context.addMatch(createMatch(searchOptionTemplate.arg(term) + privateBrowserMode, data));
        if (searchSuggestions && privateWindowSearchSuggestions) {
            if (term.size() >= minimumLetterCount) {
                searchSuggest(context, term, privateBrowser);
            }
        }
    } else if (term.startsWith(triggerCharacter)) {
        term = term.mid(1);
        QString url;
        if (isWebShortcut) {
            url = QString(currentSearchEngine.url).replace(QStringLiteral("\\{@}"), QUrl::toPercentEncoding(term));
        } else {
            url = currentSearchEngine.url + QUrl::toPercentEncoding(term);
        }
        data.insert(QStringLiteral("url"), url);
        context.addMatch(createMatch(searchOptionTemplate.arg(term), data));
        if (searchSuggestions) {
            if (term.size() >= minimumLetterCount) {
                searchSuggest(context, term);
            }
        }
    }
}

void QuickWebShortcuts::run(const KRunner::RunnerContext &/*context*/, const KRunner::QueryMatch &match) {
    const QMap<QString, QVariant> payload = match.data().toMap();
    const QString url = payload.value(QStringLiteral("url")).toString();
    QString launchCommand;
    QStringList parameters;

    if (payload.contains(QStringLiteral("browser")) || match.selectedAction()) {
        KShell::Errors splitArgsError;
        QStringList arguments = KShell::splitArgs(privateBrowser, KShell::AbortOnMeta, &splitArgsError);
        // If the arguments could not be split, abort
        if (splitArgsError != KShell::Errors::NoError || arguments.isEmpty()) {
            KNotification::event(KNotification::Error,
                                 QStringLiteral("Krunner-QuickWebShortcuts"),
                                 QStringLiteral("Error when parsing command browser arguments"),
                                 QStringLiteral("globe")
            );
            return;
        }
        launchCommand = arguments.takeAt(0);
        if (!arguments.isEmpty()) {
            parameters = arguments;
        }
    } else {
        launchCommand = QStringLiteral("xdg-open");
    }
    parameters.append(url);
    QProcess::startDetached(launchCommand, parameters);
    wasActive = true;
}

KRunner::QueryMatch QuickWebShortcuts::createMatch(const QString &text, const QMap<QString, QVariant> &data, const bool useGlobe) {
    KRunner::QueryMatch match(this);
    match.setIcon(useGlobe ? globeIcon : currentSearchEngine.qIcon);
    match.setText(text);
    match.setData(data);
    match.setRelevance(1);
    match.setType(KRunner::QueryMatch::ExactMatch);
    return match;
}


void QuickWebShortcuts::searchSuggest(KRunner::RunnerContext &context, const QString &term, const QString &browser) {

    if (searchSuggestionChoice == Config::SearchSuggestionBing) {
        QEventLoop loop;
        Bing bing(context, term, requiredData, bingMarket, browser);
        connect(&bing, &Bing::finished, &loop, &QEventLoop::quit);
        loop.exec();
    } else if (searchSuggestionChoice == Config::SearchSuggestionGoogle) {
        QEventLoop loop;
        Google google(context, term, requiredData, googleLocale, browser);
        connect(&google, &Google::finished, &loop, &QEventLoop::quit);
        loop.exec();
    } else if (searchSuggestionChoice == Config::SearchSuggestionDuckDuckGo) {
        QEventLoop loop;
        DuckDuckGo duckDuckGo(context, term, requiredData, browser);
        connect(&duckDuckGo, &DuckDuckGo::finished, &loop, &QEventLoop::quit);
        loop.exec();
    }

}

QList<QAction *> QuickWebShortcuts::actionsForMatch(const KRunner::QueryMatch &match) {
    if (match.data().toMap().contains(QStringLiteral("browser"))) {
        return privateActions;
    }
    return normalActions;
}

K_PLUGIN_CLASS_WITH_JSON(QuickWebShortcuts, "quick_web_shortcuts.json")

#include "quick_web_shortcuts.moc"
