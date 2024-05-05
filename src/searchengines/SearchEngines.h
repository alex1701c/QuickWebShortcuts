#ifndef QUICKWEBSHORTCUTS_SEARCHENGINES_H
#define QUICKWEBSHORTCUTS_SEARCHENGINES_H


#include <QMap>
#include <QDir>
#include <KSharedConfig>
#include <QComboBox>
#include <Config.h>
#include "SearchEngine.h"

class SearchEngines {

public:
    static QMap<QString, QString> getDefaultSearchEngines() {
        // Shorter macro
#define s(str) QStringLiteral(str)
        return {
                {s("Google"),        s("https://www.google.com/search?q=")},
                {s("DuckDuckGo"),    s("https://duckduckgo.com/?q=")},
                {s("Stackoverflow"), s("https://stackoverflow.com/search?q=")},
                {s("Bing"),          s("https://www.bing.com/search?q=")},
                {s("Github"),        s("https://github.com/search?q=")},
                {s("Youtube"),       s("https://www.youtube.com/results?search_query=")},
        };
    }


    static QList<SearchEngine> getAllSearchEngines() {
        QList<SearchEngine> searchEngines;
        const auto rootConfig = KSharedConfig::openConfig(
                QDir::homePath() + QStringLiteral("/.config/krunnerplugins/") + Config::ConfigFile)
                ->group(Config::RootGroup);
        auto defaultEngines = getDefaultSearchEngines();
        const auto fallbackIcon = QIcon::fromTheme(QStringLiteral("globe"));
        const auto filteredGroups = rootConfig.groupList().filter(QRegularExpression(QStringLiteral("^SearchEngine-")));
        for (const auto &groupName: filteredGroups) {
            const auto config = rootConfig.group(groupName);
            SearchEngine engine;
            engine.name = config.readEntry(SearchEngineConfig::Name);
            engine.url = config.readEntry(SearchEngineConfig::Url);
            engine.icon = config.readEntry(SearchEngineConfig::Icon);
            if (!config.readEntry(SearchEngineConfig::OriginalName).isEmpty()) {
                engine.originalName = config.readEntry(SearchEngineConfig::OriginalName);
                engine.originalURL = defaultEngines.value(engine.originalName);
                engine.originalIcon = engine.originalName.toLower();
                engine.isDefaultBased = true;
                if (engine.icon.isEmpty()) {
                    engine.icon = QString(engine.originalName).toLower();
                }
                defaultEngines.remove(engine.originalName);
            }
            engine.qIcon = QIcon::fromTheme(engine.icon, fallbackIcon);
            searchEngines.append(engine);
        }
        // If a default engine got edited they are already removed from the map
        for (const auto &item : defaultEngines.toStdMap()) {
            SearchEngine engine;
            engine.name = item.first;
            engine.url = item.second;
            engine.icon = item.first.toLower();
            engine.qIcon = QIcon::fromTheme(engine.icon);
            engine.isDefault = true;
            engine.isDefaultBased = true;
            searchEngines.append(engine);
        }
        return searchEngines;
    }


};

#endif //QUICKWEBSHORTCUTS_SEARCHENGINES_H
