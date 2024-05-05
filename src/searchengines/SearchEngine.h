#ifndef QUICKWEBSHORTCUTS_SEARCHENGINE_H
#define QUICKWEBSHORTCUTS_SEARCHENGINE_H

#include <QIcon>
#include <QString>

struct SearchEngine {
    QString name;
    QString url;
    QString icon;
    QString originalName;
    QString originalURL;
    QString originalIcon;
    QIcon qIcon;
    bool isDefault = false;
    bool isDefaultBased = false;
};

#endif // QUICKWEBSHORTCUTS_SEARCHENGINE_H
