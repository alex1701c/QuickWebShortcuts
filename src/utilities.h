#ifndef QUICKWEBSHORTCUTS_UTILITIES_H
#define QUICKWEBSHORTCUTS_UTILITIES_H

#include <QtCore>
#include "Config.h"

void initializeConfigFile() {
    const QString configFolder = QDir::homePath() + "/.config/krunnerplugins/";
    const QDir configDir(configFolder);
    if (!configDir.exists()) configDir.mkpath(configFolder);
    // Create file
    QFile configFile(configFolder + Config::ConfigFile);
    if (!configFile.exists()) {
        configFile.open(QIODevice::WriteOnly);
        configFile.close();
    }
}

#endif //QUICKWEBSHORTCUTS_UTILITIES_H
