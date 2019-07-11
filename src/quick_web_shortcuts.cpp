#include "quick_web_shortcuts.h"
#include <KLocalizedString>
#include <QtGui/QtGui>
#include <KSharedConfig>
#include <iostream>
#include "SearchEngines.h"

QuickWebShortcuts::QuickWebShortcuts(QObject *parent, const QVariantList &args)
        : Plasma::AbstractRunner(parent, args) {
    setObjectName("Quick Web Shortcuts");
    setSpeed(NormalSpeed);
    setHasRunOptions(true);
    setPriority(HighestPriority);
}

QuickWebShortcuts::~QuickWebShortcuts() = default;

void QuickWebShortcuts::init() {
    SearchEngines::getDefaultSearchEngines(searchEngines);
    configGroup = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("QuickWebShortcuts");
    QString browser = KSharedConfig::openConfig(QDir::homePath() + "/.kde/share/config/kdeglobals")->group("General")
            .readEntry("BrowserApplication");
    KSharedConfig::Ptr browserConfig = KSharedConfig::openConfig("/usr/share/applications/" + browser);

    for (const auto &group: browserConfig->groupList()) {
        if (QString(group).toLower().contains("incognito") || QString(group).toLower().contains("private")) {
            privateBrowser = browserConfig->group(group).readEntry("Exec");
        }
    }
    if (privateBrowser.isEmpty()) {
        privateBrowser = "firefox --private-window";
    }
    reloadConfiguration();
    icons = SearchEngines::getIcons();

    connect(this, SIGNAL(prepare()), this, SLOT(prepareForMatchSession()));
    connect(this, SIGNAL(teardown()), this, SLOT(matchSessionFinished()));
}


void QuickWebShortcuts::prepareForMatchSession() {
    for (auto &key:searchEngines.keys()) {
        if (searchEngines.value(key) == configGroup.readEntry("url", "https://www.google.com/search?q=")) {
            searchEngine = key;
        }
    }
}

void QuickWebShortcuts::reloadConfiguration() {
    SearchEngines::getCustomSearchEngines(searchEngines);
}

void QuickWebShortcuts::matchSessionFinished() {
    if (configGroup.readEntry("clean_history") != "false") {
        QString history = configGroup.parent().parent().group("General").readEntry("history");
        QString filteredHistory = "";
        if (configGroup.readEntry("clean_history") == "all") {
            filteredHistory = history.replace(QRegExp(R"([a-z]{0,5}: ?[^,]+,?)"), "");
        } else {
            for (const auto &item : history.split(',')) {
                if (!item.startsWith(':')) {
                    filteredHistory += item + ",";
                }
            }
        }
        if (filteredHistory.size() ==
            KSharedConfig::openConfig("krunnerrc")->group("General").readEntry("history").size()) {
            return;
        }
        QFile f(QString(getenv("HOME")) + "/.config/krunnerrc");
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
    QString term = context.query();
    if (!context.isValid()) return;

    QList<Plasma::QueryMatch> matches;
    QMap<QString, QVariant> data;


    if (term.startsWith("::")) {
        QString name = configGroup.readEntry("show_name", "false") == "true" ? " " + searchEngine : "";
        term = term.mid(2);
        data.insert("browser", privateBrowser);
        QString url = configGroup.readEntry("url", "https://www.google.com/search?q=") + QUrl::toPercentEncoding(term);
        data.insert("url", url);
        const QString mode = privateBrowser.contains("private") ? "private window" : "incognito mode";
        QString text = "Search" + name + " for " + term + " in " + mode;
        matches.append(createMatch(text, data));
    } else if (term.startsWith(':')) {
        QString name = configGroup.readEntry("show_name", "false") == "true" ? " " + searchEngine : "";
        term = term.mid(1);
        QString text = "Search" + name + " for " + term;
        QString url = configGroup.readEntry("url", "https://www.google.com/search?q=") + QUrl::toPercentEncoding(term);
        data.insert("url", url);
        matches.append(createMatch(text, data));
    } else if (configGroup.readEntry("open_urls", "true") == "true" && term.contains(QRegExp(R"(^.*\.[a-z]{2,5}$)"))) {
        QString text = "Go To  " + term;
        QString url;
        if (!term.startsWith("http")) {
            url = "http://" + term;
        } else {
            url = term;
        }
        data.insert("url", url);
        matches.append(createMatch(text, data, "globe"));
    } else if (term.startsWith("quickwebshortcuts set") || term.startsWith("qws set")) {
        QString part = term.section("set", 1);
        for (const auto &entry : searchEngines.toStdMap()) {
            if (part.isEmpty() || entry.first.toLower().startsWith(part.trimmed().toLower())) {
                data.insert("engine", entry.second);
                matches.append(
                        createMatch("Use " + entry.first + " as search engine", data, entry.first));
            }
        }
    }

    context.addMatches(matches);

}

void QuickWebShortcuts::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) {
    Q_UNUSED(context)
    QMap<QString, QVariant> payload = match.data().toMap();

    if (payload.count("url")) {
        if (!payload.count("browser")) {
            system(qPrintable("$(xdg-open " + payload.value("url").toString() + ") &"));
        } else {
            system(qPrintable("$(" + payload.value("browser").toString() + " " +
                              payload.value("url").toString() + ")  &"));
        }
    } else if (payload.count("engine")) {
        QString searchUrl = payload.value("engine").toString();
        configGroup.writeEntry("url", searchUrl);
        configGroup.sync();
    }
}

Plasma::QueryMatch
QuickWebShortcuts::createMatch(const QString &text, const QMap<QString, QVariant> &data, const QString &iconKey) {
    Plasma::QueryMatch match(this);
    if (iconKey.isEmpty()) {
        match.setIconName(icons.value(searchEngine, "globe"));
    } else {
        match.setIconName(icons.value(iconKey, "globe"));
    }
    match.setText(text);
    match.setData(data);
    return match;
}

K_EXPORT_PLASMA_RUNNER(quick_web_shortcuts, QuickWebShortcuts)

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "quick_web_shortcuts.moc"

