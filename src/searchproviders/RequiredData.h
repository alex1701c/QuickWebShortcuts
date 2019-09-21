#ifndef QUICKWEBSHORTCUTS_REQUIREDDATA_H
#define QUICKWEBSHORTCUTS_REQUIREDDATA_H

#include <QtNetwork/QNetworkAccessManager>

class RequiredData {
public:
    QNetworkProxy *proxy;
    Plasma::AbstractRunner *runner;
    QString searchEngine;
    QIcon icon;
    int maxResults;
    bool showNetworkErrors = true;
};

#endif //QUICKWEBSHORTCUTS_REQUIREDDATA_H
