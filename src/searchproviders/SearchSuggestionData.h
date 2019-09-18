#ifndef QUICKWEBSHORTCUTS_SEARCHSUGGESTIONDATA_H
#define QUICKWEBSHORTCUTS_SEARCHSUGGESTIONDATA_H

#include <KRunner/AbstractRunner>

struct SearchSuggestionData {
public:
    Plasma::AbstractRunner *runner;
    Plasma::RunnerContext context;
    QString searchEngine;
    QString query;
    QIcon icon;
    QString browser;
    int maxResults = 10;
    QString bingMarket = "en-us";
};

#endif //QUICKWEBSHORTCUTS_SEARCHSUGGESTIONDATA_H
