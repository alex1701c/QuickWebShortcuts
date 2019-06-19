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

/*
 * DONE Delete functionality for manually added search engines
 * DONE Handle defaults inside GUI
 * DONE Extract Search Engine config to file
 * DONE Read custom search engines for Krunner settings(inside runner)
 * DONE Extract Name from URL ?
 * DONE Handle event for add search engine in GUI
 * DONE Make Items in ComboBox editable
 * DONE Handle save in GUI
 * DONE Handle Defaults in GUI
 * DONE: Mark current in Gui
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

void QuickWebShortcuts::reloadConfiguration() {
    configGroup = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("QuickWebShortcuts");
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

void QuickWebShortcuts::init() {
    SearchEngines::getDefaultSearchEngines(searchEngines);
    reloadConfiguration();
    connect(this, SIGNAL(teardown()), this, SLOT(matchSessionFinished()));
}

void QuickWebShortcuts::match(Plasma::RunnerContext &context) {
    QString term = context.query();
    if (!context.isValid())
        return;

    QList<Plasma::QueryMatch> matches;
    if (term.startsWith(':')) {
        term = term.mid(1);
        QString text = "Search for " + term;
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
        matches.append(createMatch(text, data));
    }
    if (term.startsWith("quickwebshortcuts set") || term.startsWith("qws set")) {
        QString part = term.section("set", 1);
        for (const auto &entry : searchEngines.toStdMap()) {
            if (part.isEmpty() || entry.first.toLower().startsWith(part.trimmed().toLower())) {
                matches.append(createMatch("Use " + entry.first + " as search engine", "engine|" + entry.second));
            }
        }
    }

    context.addMatches(matches);

}

void QuickWebShortcuts::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) {
    Q_UNUSED(context)
    QString payload = match.data().toString();
    std::cout << payload.toStdString() << std::endl;
    if (payload.startsWith("http")) {
        system(qPrintable("$(xdg-open " + match.data().toString() + ") 2>&1 &"));
    } else if (payload.startsWith("engine|")) {
        QString searchUrl = payload.split("engine|")[1];
        configGroup.writeEntry("url", searchUrl);
        configGroup.sync();
    }
}

Plasma::QueryMatch QuickWebShortcuts::createMatch(const QString &text, const QString &data) {
    Plasma::QueryMatch match(this);
    match.setIconName("globe");
    match.setText(text);
    match.setData(data);
    return match;
}

K_EXPORT_PLASMA_RUNNER(quick_web_shortcuts, QuickWebShortcuts)

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "quick_web_shortcuts.moc"

