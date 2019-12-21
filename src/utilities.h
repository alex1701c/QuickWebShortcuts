#ifndef QUICKWEBSHORTCUTS_UTILITIES_H
#define QUICKWEBSHORTCUTS_UTILITIES_H

#include <QtCore>
#include <QNetworkProxy>
#include "Config.h"

void initializeConfigFile() {
    const QString configFolder = QDir::homePath() + QStringLiteral("/.config/krunnerplugins/");
    const QDir configDir(configFolder);
    if (!configDir.exists()) configDir.mkpath(configFolder);
    // Create file
    QFile configFile(configFolder + Config::ConfigFile);
    if (!configFile.exists()) {
        configFile.open(QIODevice::WriteOnly);
        configFile.close();
    }
}

#ifndef NO_PROXY_INTEGRATION

#include <KWallet/KWallet>
#include <KNotifications/KNotification>

using KWallet::Wallet;

QNetworkProxy *getProxyFromConfig(const QString &proxyChoice) {
    if (proxyChoice != Config::ProxyDisabled) {
        auto *wallet = Wallet::openWallet(Wallet::LocalWallet(), 0, Wallet::Synchronous);
        auto *proxy = new QNetworkProxy();
        QByteArray hostName, port, username, password;
        if (KWallet::Wallet::isEnabled() && wallet->isOpen()) {
            wallet->readEntry(KWalletConfig::ProxyHostname, hostName);
            wallet->readEntry(KWalletConfig::ProxyPort, port);
            wallet->readEntry(KWalletConfig::ProxyUsername, username);
            wallet->readEntry(KWalletConfig::ProxyPassword, password);

            proxy->setType(proxyChoice == QLatin1String("http") ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy);
            proxy->setHostName(hostName);
            proxy->setPort(port.toInt());
            proxy->setUser(username);
            proxy->setPassword(password);
        } else {
            KNotification::event(KNotification::Error,
                                 QStringLiteral("Krunner-QuickWebShortcuts"),
                                 QStringLiteral("The Proxy credentials from KWallet could not be read, proceeding without!"),
                                 QStringLiteral("globe"));
            delete wallet;
            delete proxy;
            return nullptr;
        }

        delete wallet;
        if (!port.isEmpty() && !hostName.isEmpty()) {
            return proxy;
        } else {
            KNotification::event(KNotification::Error, QStringLiteral("Krunner-QuickWebShortcuts"),
                                 QStringLiteral("The Proxy credentials require at least a Hostname and Port, proceeding without!"),
                                 QStringLiteral("globe"));
        }

        delete proxy;
    }
    return nullptr;
}

#endif
#endif //QUICKWEBSHORTCUTS_UTILITIES_H
