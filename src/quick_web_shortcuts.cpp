#include "quick_web_shortcuts.h"
#include <QtGui/QtGui>
#include <KSharedConfig>
#include <iostream>
#include "SearchEngines.h"
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
    reloadConfiguration();
    connect(this, SIGNAL(teardown()), this, SLOT(matchSessionFinished()));
}

void QuickWebShortcuts::reloadConfiguration() {
    configGroup = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("QuickWebShortcuts");

    // Read entry for private browsing launch command
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
    // Load search engines
    const QString searchEngineName = configGroup.readEntry("search_engine_name");
    for (auto &engine:SearchEngines::getAllSearchEngines()) {
        if (engine.name == searchEngineName) {
            currentSearchEngine = engine;
            break;
        }
    }

    // Load general settings
    openUrls = configGroup.readEntry("open_urls", "true") == "true";
    if (configGroup.readEntry("show_search_for_note") == "false") {
        searchOptionTemplate = "%1";
    } else if (configGroup.readEntry("show_name") == "true") {
        searchOptionTemplate = "Search " + currentSearchEngine.name + " for %1";
    } else {
        searchOptionTemplate = "Search for %1";
    }
    if (configGroup.readEntry("private_window_note", "true") == "true") {
        privateBrowserMode = privateBrowser.contains("private") ? " in private window" : " in incognito mode";
    } else {
        privateBrowserMode = "";
    }

    // Search suggestions settings
    searchSuggestionChoice = configGroup.readEntry("search_suggestions", "disabled");
    searchSuggestions = searchSuggestionChoice != "disabled";
    privateWindowSearchSuggestions = searchSuggestions && configGroup.readEntry("private_window_search_suggestions") == "true";
    minimumLetterCount = configGroup.readEntry("minimum_letter_count", "3").toInt();
    maxSuggestionResults = configGroup.readEntry("max_search_suggestions", "10").toInt();
    bingMarket = configGroup.readEntry("bing_locale", "en-us");
    googleLocale = configGroup.readEntry("google_locale", "en");

    // RequiredData is for all search providers required and does not need to be updated
    // outside of the reloadConfiguration method
    requiredData.searchEngine = currentSearchEngine.url;
    requiredData.icon = currentSearchEngine.qIcon;
    requiredData.runner = this;
    requiredData.maxResults = maxSuggestionResults;
    requiredData.searchOptionTemplate = searchOptionTemplate;

    // Proxy settings
    const QString proxyChoice = configGroup.readEntry("proxy", "disabled");
    if (proxyChoice != "disabled") {
        proxy = new QNetworkProxy();
        if (proxyChoice == "http") proxy->setType(QNetworkProxy::HttpProxy);
        else proxy->setType(QNetworkProxy::Socks5Proxy);
        proxy->setHostName(configGroup.readEntry("proxy_hostname"));
        proxy->setPort(configGroup.readEntry("proxy_port").toInt());
        proxy->setUser(QByteArray::fromHex(configGroup.readEntry("proxy_username").toLocal8Bit()));
        proxy->setPassword(QByteArray::fromHex(configGroup.readEntry("proxy_password").toLocal8Bit()));
        requiredData.proxy = proxy;
        requiredData.showNetworkErrors = configGroup.readEntry("proxy_show_errors", "true") == "true";
    } else {
        requiredData.proxy = nullptr;
    }
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

    QMap<QString, QVariant> data;

    if (term.startsWith("::")) {
        term = term.mid(2);
        data.insert("browser", privateBrowser);
        QString url = currentSearchEngine.url + QUrl::toPercentEncoding(term);
        data.insert("url", url);
        context.addMatch(createMatch(searchOptionTemplate.arg(term) + privateBrowserMode, data));
        if (searchSuggestions && privateWindowSearchSuggestions) {
            if (term.size() < minimumLetterCount) return;
            if (searchSuggestionChoice == "bing") {
                bingSearchSuggest(context, term, privateBrowser);
            } else if (searchSuggestionChoice == "google") {
                googleSearchSuggest(context, term, privateBrowser);
            } else {
                duckDuckGoSearchSuggest(context, term, privateBrowser);
            }
        }
    } else if (term.startsWith(':')) {
        term = term.mid(1);
        data.insert("url", currentSearchEngine.url + QUrl::toPercentEncoding(term));
        context.addMatch(createMatch(searchOptionTemplate.arg(term), data));
        if (searchSuggestions) {
            if (term.size() < minimumLetterCount) return;
            if (searchSuggestionChoice == "bing") {
                bingSearchSuggest(context, term);
            } else if (searchSuggestionChoice == "google") {
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

    QMap<QString, QVariant> payload = match.data().toMap();

    if (payload.count("url")) {
        system(qPrintable("$(" + payload.value("browser", "xdg-open").toString() + " '" + payload.value("url").toString() + "') &"));
    } else {
        configGroup.writeEntry("url", payload.value("engine").toString());
    }
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

