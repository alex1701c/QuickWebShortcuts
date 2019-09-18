#ifndef QUICKWEBSHORTCUTS_REQUIREDDATA_H
#define QUICKWEBSHORTCUTS_REQUIREDDATA_H

class RequiredData {
public:
    Plasma::AbstractRunner *runner;
    QString searchEngine;
    QIcon icon;
    int maxResults = 10;
};

#endif //QUICKWEBSHORTCUTS_REQUIREDDATA_H
