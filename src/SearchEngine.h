#ifndef QUICKWEBSHORTCUTS_SEARCHENGINE_H
#define QUICKWEBSHORTCUTS_SEARCHENGINE_H


class SearchEngine {
public:
    QString name;
    QString url;
    QString icon;
    QString originalName;
    QString originalURL;
    QString originalIcon;
    QIcon qIcon;
    bool isDefault = false;
};


#endif //QUICKWEBSHORTCUTS_SEARCHENGINE_H
