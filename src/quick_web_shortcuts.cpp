#include "quick_web_shortcuts.h"
#include <QtGui/QtGui>
#include <iostream>
#include "searchengines/SearchEngines.h"
#include "Config.h"
#include "utilities.h"
#include <searchproviders/Bing.h>
#include <searchproviders/Google.h>
#include <searchproviders/DuckDuckGo.h>

QuickWebShortcuts::QuickWebShortcuts(QObject *parent, const QVariantList &args)
        : Plasma::AbstractRunner(parent, args) {
    setObjectName("Quick Web Shortcuts");
    setSpeed(NormalSpeed);
    setHasRunOptions(true);
    setPriority(HighestPriority);
}

void QuickWebShortcuts::init() {
    initializeConfigFile();

    // Add file watcher for config
    watcher.addPath(QDir::homePath() + "/.config/krunnerplugins/" + Config::ConfigFile);
    connect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(reloadPluginConfiguration(QString)));
    connect(this, SIGNAL(teardown()), this, SLOT(matchSessionFinished()));

    reloadPluginConfiguration();
}

void QuickWebShortcuts::reloadPluginConfiguration(const QString &configFile) {
    KConfigGroup configGroup = KSharedConfig::openConfig(QDir::homePath() + "/.config/krunnerplugins/" + Config::ConfigFile)
            ->group(Config::RootGroup);
    // Force sync from file
    if (!configFile.isEmpty()) configGroup.config()->reparseConfiguration();

    // If the file gets edited with a text editor, it often gets replaced by the edited version
    // https://stackoverflow.com/a/30076119/9342842
    if (!configFile.isEmpty()) {
        if (QFile::exists(configFile)) {
            watcher.addPath(configFile);
        }
    }
    // Read entry for private browsing launch command
    const QString browser = KSharedConfig::openConfig(QDir::homePath() + "/.kde/share/config/kdeglobals")->group("General")
            .readEntry("BrowserApplication");
    if (!browser.isEmpty()) {
        const KSharedConfig::Ptr browserConfig = KSharedConfig::openConfig("/usr/share/applications/" + browser);
        for (const auto &group: browserConfig->groupList()) {
            if (group.contains("incognito", Qt::CaseInsensitive) || group.contains("private", Qt::CaseInsensitive)) {
                privateBrowser = browserConfig->group(group).readEntry("Exec");
            }
        }
    } else {
        privateBrowser = "firefox --private-window";
    }
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
        SearchEngine defaultEngine;
        defaultEngine.qIcon = QIcon::fromTheme("google");
        defaultEngine.name = "Google";
        defaultEngine.url = "https://www.google.com/search?q=";
        currentSearchEngine = defaultEngine;
    }

    // Load general settings
    openUrls = configGroup.readEntry(Config::OpenUrls, true);
    if (!configGroup.readEntry(Config::ShowSearchForNote, true)) {
        searchOptionTemplate = "%1";
    } else if (configGroup.readEntry(Config::ShowName, false)) {
        searchOptionTemplate = "Search " + currentSearchEngine.name + " for %1";
    } else {
        searchOptionTemplate = "Search for %1";
    }

    if (configGroup.readEntry(Config::PrivateWindowNote, true)) {
        privateBrowserMode = privateBrowser.contains("private") ? " in private window" : " in incognito mode";
    } else {
        privateBrowserMode = "";
    }
    triggerCharacter = configGroup.readEntry(Config::TriggerCharacter, Config::TriggerCharacterDefault);
    privateWindowTrigger = triggerCharacter + triggerCharacter;

    // Search suggestions settings
    searchSuggestionChoice = configGroup.readEntry(Config::SearchSuggestions, Config::SearchSuggestionDisabled);
    searchSuggestions = searchSuggestionChoice != Config::SearchSuggestionDisabled;
    privateWindowSearchSuggestions = searchSuggestions && configGroup.readEntry(Config::PrivateWindowSearchSuggestions, false);
    minimumLetterCount = configGroup.readEntry(Config::MinimumLetterCount, Config::MinimumLetterCountDefault);
    maxSuggestionResults = configGroup.readEntry(Config::MaxSuggestionResults, Config::MaxSuggestionResultsDefault);
    bingMarket = configGroup.readEntry(Config::BingMarket, Config::BingMarketDefault);
    googleLocale = configGroup.readEntry(Config::GoogleLocale, Config::GoogleLocaleDefault);

    // RequiredData is for all search providers required and does not need to be updated
    // outside of the reloadConfiguration method
    requiredData.searchEngine = currentSearchEngine.url;
    requiredData.icon = currentSearchEngine.qIcon;
    requiredData.runner = this;
    requiredData.maxResults = maxSuggestionResults;
    requiredData.searchOptionTemplate = searchOptionTemplate;

    // Proxy settings
    const QString proxyChoice = configGroup.readEntry(Config::Proxy, Config::ProxyDisabled);
    if (proxyChoice != Config::ProxyDisabled) {
        auto *proxy = new QNetworkProxy();
        proxy->setType(proxyChoice == "http" ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy);
        proxy->setHostName(configGroup.readEntry(Config::ProxyHostname));
        proxy->setPort(configGroup.readEntry(Config::ProxyPort).toInt());
        proxy->setUser(QByteArray::fromHex(configGroup.readEntry(Config::ProxyUsername).toLocal8Bit()));
        proxy->setPassword(QByteArray::fromHex(configGroup.readEntry(Config::ProxyPassword).toLocal8Bit()));
        requiredData.proxy = proxy;
        requiredData.showNetworkErrors = configGroup.readEntry(Config::ProxyShowErrors, true);
    } else {
        requiredData.proxy = nullptr;
    }

    // History settings
    QString historyChoice = configGroup.readEntry(Config::CleanHistory, Config::CleanHistoryDefault);
    cleanAll = historyChoice == Config::CleanHistoryAll;
    cleanQuick = historyChoice == Config::CleanHistoryQuick;
    cleanNone = historyChoice == Config::CleanHistoryNone;
}

void QuickWebShortcuts::matchSessionFinished() {
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
        for (const auto &item : toFilter.split(',', QString::SkipEmptyParts)) {
            if (!item.startsWith(triggerCharacter)) {
                filteredHistory += item + ",";
            }
        }
    }
    // No changes have been made, exit function
    if (filteredHistory.size() == initialHistorySize) return;

    // Write changes and make sure that the config gets synced
    generalKrunnerConfig.writeEntry("history", filteredHistory);
    generalKrunnerConfig.sync();
}

void QuickWebShortcuts::match(Plasma::RunnerContext &context) {
    if (!context.isValid()) return;
    wasActive = false;
    // Remove escape character
    QString term = QString(context.query()).replace(QString::fromWCharArray(L"\u001B"), " ");

    QMap<QString, QVariant> data;

    if (term.startsWith(privateWindowTrigger)) {
        term = term.mid(2);
        data.insert("browser", privateBrowser);
        QString url = currentSearchEngine.url + QUrl::toPercentEncoding(term);
        data.insert("url", url);
        context.addMatch(createMatch(searchOptionTemplate.arg(term) + privateBrowserMode, data));
        if (searchSuggestions && privateWindowSearchSuggestions) {
            if (term.size() < minimumLetterCount) return;
            if (searchSuggestionChoice == Config::SearchSuggestionBing) {
                bingSearchSuggest(context, term, privateBrowser);
            } else if (searchSuggestionChoice == Config::SearchSuggestionGoogle) {
                googleSearchSuggest(context, term, privateBrowser);
            } else {
                duckDuckGoSearchSuggest(context, term, privateBrowser);
            }
        }
    } else if (term.startsWith(triggerCharacter)) {
        term = term.mid(1);
        data.insert("url", currentSearchEngine.url + QUrl::toPercentEncoding(term));
        context.addMatch(createMatch(searchOptionTemplate.arg(term), data));
        if (searchSuggestions) {
            if (term.size() < minimumLetterCount) return;
            if (searchSuggestionChoice == Config::SearchSuggestionBing) {
                bingSearchSuggest(context, term);
            } else if (searchSuggestionChoice == Config::SearchSuggestionGoogle) {
                googleSearchSuggest(context, term);
            } else {
                duckDuckGoSearchSuggest(context, term);
            }
        }
    } else if (openUrls && term.contains(urlRegex)) {
        QString text = "Go To  " + term;
        data.insert("url", !term.startsWith("http") ? "http://" + term : term);
        context.addMatch(createMatch(text, data, true));
    }
}

void QuickWebShortcuts::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) {
    Q_UNUSED(context)

    const QMap<QString, QVariant> payload = match.data().toMap();
    system(qPrintable("$(" + payload.value("browser", "xdg-open").toString() + " '" + payload.value("url").toString() + "') &"));
    wasActive = true;
}

Plasma::QueryMatch QuickWebShortcuts::createMatch(const QString &text, const QMap<QString, QVariant> &data, const bool useGlobe) {
    Plasma::QueryMatch match(this);
    match.setIcon(useGlobe ? globeIcon : currentSearchEngine.qIcon);
    match.setText(text);
    match.setData(data);
    match.setRelevance(1);
    return match;
}

void QuickWebShortcuts::bingSearchSuggest(Plasma::RunnerContext &context, const QString &term, const QString &browser) {
    QEventLoop loop;
    Bing bing(context, term, requiredData, bingMarket, browser);
    connect(&bing, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
}

void QuickWebShortcuts::googleSearchSuggest(Plasma::RunnerContext &context, const QString &term, const QString &browser) {
    QEventLoop loop;
    Google google(context, term, requiredData, googleLocale, browser);
    connect(&google, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
}

void QuickWebShortcuts::duckDuckGoSearchSuggest(Plasma::RunnerContext &context, const QString &term, const QString &browser) {
    QEventLoop loop;
    DuckDuckGo duckDuckGo(context, term, requiredData, browser);
    connect(&duckDuckGo, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
}

K_EXPORT_PLASMA_RUNNER(quick_web_shortcuts, QuickWebShortcuts)

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "quick_web_shortcuts.moc"

