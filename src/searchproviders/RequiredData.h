#ifndef QUICKWEBSHORTCUTS_REQUIREDDATA_H
#define QUICKWEBSHORTCUTS_REQUIREDDATA_H

#include <QtNetwork/QNetworkAccessManager>

class RequiredData {
public:
    QNetworkProxy *proxy = nullptr;
    Plasma::AbstractRunner *runner;
    QString searchEngine;
    QIcon icon;
    int maxResults = 10;
};

#endif //QUICKWEBSHORTCUTS_REQUIREDDATA_H
