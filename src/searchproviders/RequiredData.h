#ifndef QUICKWEBSHORTCUTS_REQUIREDDATA_H
#define QUICKWEBSHORTCUTS_REQUIREDDATA_H

#include <QNetworkAccessManager>
#include <QIcon>

class RequiredData {
public:
    QNetworkProxy *proxy = nullptr;
    Plasma::AbstractRunner *runner;
    QString searchEngine;
    bool isWebShortcut;
    QIcon icon;
    int maxResults;
    bool showNetworkErrors = true;
    QString searchOptionTemplate;
};

#endif //QUICKWEBSHORTCUTS_REQUIREDDATA_H
