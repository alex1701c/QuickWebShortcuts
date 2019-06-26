#include "SearchEngines.h"
#include <QMap>
#include <KConfigCore/KSharedConfig>
#include <KConfigCore/KConfigGroup>

QMap<QString, QString> SearchEngines::getDefaultSearchEngines(QMap<QString, QString> &engines) {
    engines.insert("Google", "https://www.google.com/search?q=");
    engines.insert("DuckDuckGo", "https://duckduckgo.com/?q=");
    engines.insert("Stackoverflow", "https://stackoverflow.com/search?q=");
    engines.insert("Bing", "https://www.bing.com/search?q=");
    engines.insert("Github", "https://github.com/search?q=");
    engines.insert("Youtube", "https://www.youtube.com/results?search_query=");
    return engines;
}

QMap<QString, QString> SearchEngines::getCustomSearchEngines(QMap<QString, QString> &engines) {
    KConfigGroup config = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("QuickWebShortcuts");
    KConfigGroup kse = config.group("CustomSearchEngines");
    for (const QString &key:config.group("CustomSearchEngines").keyList()) {
        engines.insert(key, kse.readEntry(key));
    }
    return engines;
}

QMap<QString, QString> SearchEngines::getDefaultSearchEngines() {
    QMap<QString, QString> engines;
    return getDefaultSearchEngines(engines);
}

QMap<QString, QString> SearchEngines::getCustomSearchEngines() {
    QMap<QString, QString> engines;
    return getCustomSearchEngines(engines);
}

QList<QString> SearchEngines::getDefaultSearchEngineNames() {
    return QList<QString>{"Google", "DuckDuckGo", "Stackoverflow", "Bing", "Github", "Youtube"};
}

QMap<QString, QString> SearchEngines::getIcons() {
    QMap<QString, QString> map;
    map.insert("Bing", "/usr/share/icons/bing.svg");
    map.insert("Google", "/usr/share/icons/google.svg");
    map.insert("DuckDuckGo", "/usr/share/icons/duckduckgo.svg");
    map.insert("Stackoverflow", "/usr/share/icons/stackoverflow.svg");
    map.insert("Github", "/usr/share/icons/github.svg");
    map.insert("Youtube", "/usr/share/icons/youtube.svg");
    return map;
}


