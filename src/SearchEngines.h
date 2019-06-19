//
// Created by alex on 18.06.19.
//
#include <QMap>


#ifndef QUICKWEBSHORTCUTS_SEARCHENGINES_H
#define QUICKWEBSHORTCUTS_SEARCHENGINES_H


#include <QtCore/QString>

class SearchEngines {

public:
    static QMap<QString, QString> getDefaultSearchEngines(QMap<QString, QString> engines = QMap<QString, QString>());

    static QMap<QString, QString> getCustomSearchEngines(QMap<QString, QString> engines = QMap<QString, QString>());
};


#endif //QUICKWEBSHORTCUTS_SEARCHENGINES_H
