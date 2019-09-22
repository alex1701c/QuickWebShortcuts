#include <QMap>

#ifndef QUICKWEBSHORTCUTS_SEARCHENGINES_H
#define QUICKWEBSHORTCUTS_SEARCHENGINES_H


#include <QtCore/QString>
#include <QMap>
#include <KSharedConfig>
#include <QtWidgets/QComboBox>

class SearchEngines {

public:
    static QMap<QString, QString> getDefaultSearchEngines(QMap<QString, QString> &engines) {
        engines.insert("Google", "https://www.google.com/search?q=");
        engines.insert("DuckDuckGo", "https://duckduckgo.com/?q=");
        engines.insert("Stackoverflow", "https://stackoverflow.com/search?q=");
        engines.insert("Bing", "https://www.bing.com/search?q=");
        engines.insert("Github", "https://github.com/search?q=");
        engines.insert("Youtube", "https://www.youtube.com/results?search_query=");
        return engines;
    }

    static QMap<QString, QString> getDefaultSearchEngines() {
        QMap<QString, QString> engines;
        return getDefaultSearchEngines(engines);
    }

    static QMap<QString, QString> getCustomSearchEngines() {
        QMap<QString, QString> engines;
        return getCustomSearchEngines(engines);
    }

    static QMap<QString, QString> getCustomSearchEngines(QMap<QString, QString> &engines) {
        KConfigGroup config = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("QuickWebShortcuts");
        KConfigGroup kse = config.group("CustomSearchEngines");
        for (const QString &key:config.group("CustomSearchEngines").keyList()) {
            engines.insert(key, kse.readEntry(key));
        }
        return engines;
    }

    static QList<QString> getDefaultSearchEngineNames() {
        return {"Google", "DuckDuckGo", "Stackoverflow", "Bing", "Github", "Youtube"};
    }

    static QMap<QString, QIcon> getIcons() {
        return {
                {"globe",         QIcon::fromTheme("globe")},
                {"Bing",          QIcon("/usr/share/icons/bing.svg")},
                {"Google",        QIcon("/usr/share/icons/google.svg")},
                {"DuckDuckGo",    QIcon("/usr/share/icons/duckduckgo.svg")},
                {"Stackoverflow", QIcon("/usr/share/icons/stackoverflow.svg")},
                {"Github",        QIcon("/usr/share/icons/github.svg")},
                {"Youtube",       QIcon("/usr/share/icons/youtube.svg")},
        };
    }

};

#endif //QUICKWEBSHORTCUTS_SEARCHENGINES_H
