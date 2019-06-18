//
// Created by alex on 18.06.19.
//

#ifndef QUICKWEBSHORTCUTS_SEARCHENGINES_H
#define QUICKWEBSHORTCUTS_SEARCHENGINES_H


#include <QtCore/QString>

class SearchEngines {

public:
    static QMap<QString, QString> getDefaultSearchEngines();
    static QMap<QString, QString> getCustomSearchEngines();
};


#endif //QUICKWEBSHORTCUTS_SEARCHENGINES_H
