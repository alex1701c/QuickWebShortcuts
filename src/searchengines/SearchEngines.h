#include <QMap>

#ifndef QUICKWEBSHORTCUTS_SEARCHENGINES_H
#define QUICKWEBSHORTCUTS_SEARCHENGINES_H


#include <QtCore/QString>
#include <QMap>
#include <KSharedConfig>
#include <QtWidgets/QComboBox>
#include <Config.h>
#include "SearchEngine.h"

class SearchEngines {

public:
    static QMap<QString, QString> getDefaultSearchEngines() {
        return {
                {"Google",        "https://www.google.com/search?q="},
                {"DuckDuckGo",    "https://duckduckgo.com/?q="},
                {"Stackoverflow", "https://stackoverflow.com/search?q="},
                {"Bing",          "https://www.bing.com/search?q="},
                {"Github",        "https://github.com/search?q="},
                {"Youtube",       "https://www.youtube.com/results?search_query="},
        };
    }

    static QList<QString> getDefaultSearchEngineNames() {
        return {"Google", "DuckDuckGo", "Stackoverflow", "Bing", "Github", "Youtube"};
    }

    static QMap<QString, QString> getIconNames() {
        return {
                {"Bing",          "bing"},
                {"Google",        "google"},
                {"DuckDuckGo",    "duckduckgo"},
                {"Stackoverflow", "stackoverflow"},
                {"Github",        "github"},
                {"Youtube",       "youtube"},
        };
    }

    static QList<SearchEngine> getAllSearchEngines() {
        QList<SearchEngine> searchEngines;
        const auto rootConfig = KSharedConfig::openConfig(QDir::homePath() + "/.config/krunnerplugins/" + Config::ConfigFile)
                ->group(Config::RootGroup);
        auto defaultEngines = getDefaultSearchEngines();
        const auto iconNames = getIconNames();
        for (const auto &groupName:rootConfig.groupList().filter(QRegExp("^SearchEngine-"))) {
            const auto config = rootConfig.group(groupName);
            SearchEngine engine;
            engine.name = config.readEntry(SearchEngineConfig::Name);
            engine.url = config.readEntry(SearchEngineConfig::Url);
            engine.icon = config.readEntry(SearchEngineConfig::Icon);
            if (engine.icon.isEmpty()) engine.icon = config.readEntry(SearchEngineConfig::OriginalIcon);
            if (engine.icon.isEmpty()) engine.icon = "globe";
            engine.qIcon = engine.icon.startsWith("/") ? QIcon(engine.icon) : QIcon::fromTheme(engine.icon);
            if (!config.readEntry(SearchEngineConfig::OriginalName).isEmpty()) {
                engine.originalName = config.readEntry(SearchEngineConfig::OriginalName);
                engine.originalURL = defaultEngines.value(engine.originalName);
                engine.originalIcon = iconNames.value(engine.originalName);
                engine.isDefaultBased = true;
                defaultEngines.remove(engine.originalName);
            }
            searchEngines.append(engine);
        }
        for (const auto &item : defaultEngines.toStdMap()) {
            SearchEngine engine;
            engine.name = item.first;
            engine.url = item.second;
            engine.icon = iconNames.value(item.first);
            engine.qIcon = engine.icon.startsWith("/") ? QIcon(engine.icon) : QIcon::fromTheme(engine.icon);
            engine.isDefault = true;
            engine.isDefaultBased = true;
            searchEngines.append(engine);
        }
        return searchEngines;
    }


};

#endif //QUICKWEBSHORTCUTS_SEARCHENGINES_H
