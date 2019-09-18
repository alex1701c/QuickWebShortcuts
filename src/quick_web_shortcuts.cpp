#include "quick_web_shortcuts.h"
#include <QtGui/QtGui>
#include <KSharedConfig>
#include <iostream>
#include "SearchEngines.h"
#include <searchproviders/Bing.h>

QuickWebShortcuts::QuickWebShortcuts(QObject *parent, const QVariantList &args)
        : Plasma::AbstractRunner(parent, args) {
    setObjectName("Quick Web Shortcuts");
    setSpeed(NormalSpeed);
    setHasRunOptions(true);
    setPriority(HighestPriority);
}

void QuickWebShortcuts::init() {
    SearchEngines::getDefaultSearchEngines(searchEngines);
    configGroup = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("QuickWebShortcuts");
    QString browser = KSharedConfig::openConfig(QDir::homePath() + "/.kde/share/config/kdeglobals")->group("General")
            .readEntry("BrowserApplication");
    if (!browser.isEmpty()) {
        KSharedConfig::Ptr browserConfig = KSharedConfig::openConfig("/usr/share/applications/" + browser);

        for (const auto &group: browserConfig->groupList()) {
            if (group.contains("incognito", Qt::CaseInsensitive) || group.contains("private", Qt::CaseInsensitive)) {
                privateBrowser = browserConfig->group(group).readEntry("Exec");
            }
        }
    } else {
        privateBrowser = "firefox --private-window";
    }
    reloadConfiguration();

    connect(this, SIGNAL(teardown()), this, SLOT(matchSessionFinished()));
}

void QuickWebShortcuts::reloadConfiguration() {
    icons = SearchEngines::getIcons();
    SearchEngines::getCustomSearchEngines(searchEngines);
    searchEngineBaseUrl = configGroup.readEntry("url");
    if (searchEngineBaseUrl.isEmpty()) searchEngineBaseUrl = "https://www.google.com/search?q=";
    for (auto &key:searchEngines.keys()) {
        if (searchEngines.value(key) == searchEngineBaseUrl) {
            searchEngine = key;
            currentIcon = icons.value(key);
            break;
        }
    }
    if (currentIcon.isNull()) currentIcon = QIcon::fromTheme("globe");
    openUrls = configGroup.readEntry("open_urls", "true") == "true";
    searchEngineDisplayName = configGroup.readEntry("show_name", "false") == "true" ? " " + searchEngine : "";
    privateBrowserMode = privateBrowser.contains("private") ? "private window" : "incognito mode";
    minimumLetterCount = configGroup.readEntry("minimum_letter_count", "3").toInt();

    maxSuggestionResults = configGroup.readEntry("max_search_results", "10").toInt();
    bingMarket = configGroup.readEntry("bing_locale", "en-us");

    searchSuggestionChoice = configGroup.readEntry("search_suggestions", "disabled");
    searchSuggestions = searchSuggestionChoice != "disabled";
    privateWindowSearchSuggestions = configGroup.readEntry("private_window_search_suggestions") == "true";

    // RequiredData is for all search providers required and does not need to be updated
    // outside of the reloadConfiguration method
    requiredData.searchEngine = searchEngineBaseUrl;
    requiredData.icon = currentIcon;
    requiredData.runner = this;
    requiredData.maxResults = maxSuggestionResults;
}

void QuickWebShortcuts::matchSessionFinished() {
    if (configGroup.readEntry("clean_history") != "false") {
        QString history = configGroup.parent().parent().group("General").readEntry("history");
        QString filteredHistory;
        if (configGroup.readEntry("clean_history") == "all") {
            filteredHistory = history.replace(QRegExp(R"([a-z]{0,5}: ?[^,]+,?)"), "");
        } else {
            for (const auto &item : history.split(',')) {
                if (!item.startsWith(':')) {
                    filteredHistory += item + ",";
                }
            }
        }
        if (filteredHistory.size() == KSharedConfig::openConfig("krunnerrc")->group("General").readEntry("history").size()) {
            return;
        }
        QFile f(QDir::homePath() + "/.config/krunnerrc");
        if (f.open(QIODevice::ReadWrite)) {
            QString s;
            QTextStream t(&f);
            while (!t.atEnd()) {
                QString line = t.readLine();
                if (!line.startsWith("history")) {
                    s.append(line + "\n");
                } else {
                    s.append("history=" + filteredHistory + "\n");
                }
            }
            f.resize(0);
            f.write(s.toLocal8Bit());
            f.close();
        }
    }
}

void QuickWebShortcuts::match(Plasma::RunnerContext &context) {
    if (!context.isValid()) return;
    // Remove escape character
    QString term = QString(context.query()).replace(QString::fromWCharArray(L"\u001B"), " ");

    QList<Plasma::QueryMatch> matches;
    QMap<QString, QVariant> data;

    if (term.startsWith("::")) {
        term = term.mid(2);
        data.insert("browser", privateBrowser);
        QString url = searchEngineBaseUrl + QUrl::toPercentEncoding(term);
        data.insert("url", url);
        QString text = "Search" + searchEngineDisplayName + " for " + term + " in " + privateBrowserMode;
        context.addMatch(createMatch(text, data));
        if (searchSuggestions && privateWindowSearchSuggestions) {
            if (searchSuggestionChoice == "bing") {
                bingSearchSuggest(context, term, privateBrowser);
            }
        }
    } else if (term.startsWith(':')) {
        term = term.mid(1);
        QString text = "Search" + searchEngineDisplayName + " for " + term;
        data.insert("url", searchEngineBaseUrl + QUrl::toPercentEncoding(term));
        context.addMatch(createMatch(text, data));
        if (searchSuggestions) {
            if (searchSuggestionChoice == "bing") {
                bingSearchSuggest(context, term);
            }
        }
    } else if (openUrls && term.contains(QRegExp(R"(^.*\.[a-z]{2,5}$)"))) {
        QString text = "Go To  " + term;
        data.insert("url", !term.startsWith("http") ? "http://" + term : term);
        context.addMatch(createMatch(text, data, "globe"));
    } else if (term.startsWith("qws set") || term.startsWith("quickwebshortcuts set")) {
        const QString part = term.section("set", 1);
        for (const auto &entry : searchEngines.toStdMap()) {
            if (part.isEmpty() || entry.first.toLower().startsWith(part.trimmed().toLower())) {
                data.insert("engine", entry.second);
                context.addMatch(createMatch("Use " + entry.first + " as search engine", data, entry.first));
            }
        }
    }
}

void QuickWebShortcuts::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) {
    Q_UNUSED(context)

    QMap<QString, QVariant> payload = match.data().toMap();

    if (payload.count("url")) {
        system(qPrintable("$(" + payload.value("browser", "xdg-open").toString() + " " + payload.value("url").toString() + ") &"));
        qInfo() << "Opening url: " << "$(" + payload.value("browser", "xdg-open").toString()
                                      + " " + payload.value("url").toString() + ") &";
    } else {
        configGroup.writeEntry("url", payload.value("engine").toString());
    }
}

Plasma::QueryMatch QuickWebShortcuts::createMatch(const QString &text, const QMap<QString, QVariant> &data, const QString &icon) {
    Plasma::QueryMatch match(this);
    match.setIcon(icon.isEmpty() ? currentIcon : icons.value(icon, QIcon::fromTheme("globe")));
    match.setText(text);
    match.setData(data);
    return match;
}

void QuickWebShortcuts::bingSearchSuggest(Plasma::RunnerContext &context, const QString &term, const QString &browser) {
    if (term.size() < minimumLetterCount) return;
    QEventLoop loop;
    Bing bing(context, term, requiredData, bingMarket, browser);
    connect(&bing, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
}

K_EXPORT_PLASMA_RUNNER(quick_web_shortcuts, QuickWebShortcuts)

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "quick_web_shortcuts.moc"

