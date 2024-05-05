#ifndef QUICKWEBSHORTCUTS_REQUIREDDATA_H
#define QUICKWEBSHORTCUTS_REQUIREDDATA_H

#include <KRunner/AbstractRunner>
#include <QIcon>
#include <QNetworkAccessManager>

class RequiredData
{
public:
    QNetworkProxy *proxy = nullptr;
    KRunner::AbstractRunner *runner;
    QString searchEngine;
    bool isWebShortcut;
    QIcon icon;
    int maxResults;
    bool showNetworkErrors = true;
    QString searchOptionTemplate;
};

#endif // QUICKWEBSHORTCUTS_REQUIREDDATA_H
