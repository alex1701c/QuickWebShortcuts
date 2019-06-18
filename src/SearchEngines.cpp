//
// Created by alex on 18.06.19.
//

#include "SearchEngines.h"
#include <QMap>
#include <KConfigCore/KSharedConfig>
#include <KConfigCore/KConfigGroup>

QMap<QString, QString> SearchEngines::getDefaultSearchEngines() {
    QMap<QString, QString> engines = QMap<QString, QString>();
    engines.insert("Google", "https://www.google.com/search?q=");
    engines.insert("DuckDuckGo", "https://duckduckgo.com/?q=");
    engines.insert("Stackoverflow", "https://stackoverflow.com/search?q=");
    engines.insert("Bing", "https://www.bing.com/search?q=");
    engines.insert("Github", "https://github.com/search?q=");
    engines.insert("Youtube", "https://www.youtube.com/results?search_query=");
}

QMap<QString, QString> SearchEngines::getCustomSearchEngines() {
    QMap<QString, QString> engines = QMap<QString, QString>();
    KConfigGroup config = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("QuickWebShortcuts");
    KConfigGroup kse = config.group("CustomSearchEngines");
    for (const QString &key:config.group("CustomSearchEngines").keyList()) {
        engines.insert(key, kse.readEntry(key));
    }
}
