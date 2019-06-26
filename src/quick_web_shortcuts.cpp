/*
   Copyright %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/
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
    configGroup = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("QuickWebShortcuts");
    SearchEngines::getCustomSearchEngines(searchEngines);
}

void QuickWebShortcuts::matchSessionFinished() {
    if (!wasActive) return;
    if (configGroup.readEntry("clean_history") != "false") {
        wasActive = false;
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
    if (!context.isValid())
        return;

    QList<Plasma::QueryMatch> matches;
    if (term.startsWith(':')) {
        term = term.mid(1);
        QString name = configGroup.readEntry("show_name", "false") == "true" ? " " + searchEngine : "";
        QString text = "Search" + name + " for " + term;
        QString data = configGroup.readEntry("url", "https://www.google.com/search?q=") + QUrl::toPercentEncoding(term);
        matches.append(createMatch(text, data));
    }
    if (term.contains(QRegExp(R"(^.*\.[a-z]{2,5}$)"))) {
        QString text = "Go To  " + term;
        QString data;
        if (!term.startsWith("http")) {
            data = "http://" + term;
        } else {
            data = term;
        }
        matches.append(createMatch(text, data, "globe"));
    }
    if (term.startsWith("quickwebshortcuts set") || term.startsWith("qws set")) {
        QString part = term.section("set", 1);
        for (const auto &entry : searchEngines.toStdMap()) {
            if (part.isEmpty() || entry.first.toLower().startsWith(part.trimmed().toLower())) {
                matches.append(
                        createMatch("Use " + entry.first + " as search engine", "engine|" + entry.second, entry.first));
            }
        }
    }

    context.addMatches(matches);

}

void QuickWebShortcuts::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) {
    Q_UNUSED(context)
    QString payload = match.data().toString();
    wasActive = true;
    if (payload.startsWith("http")) {
        system(qPrintable("$(xdg-open " + match.data().toString() + ") 2>&1 &"));
    } else if (payload.startsWith("engine|")) {
        QString searchUrl = payload.split("engine|")[1];
        configGroup.writeEntry("url", searchUrl);
        configGroup.sync();
    }
}

Plasma::QueryMatch QuickWebShortcuts::createMatch(const QString &text, const QString &data, const QString &iconKey) {
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

