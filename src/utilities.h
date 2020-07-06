#ifndef QUICKWEBSHORTCUTS_UTILITIES_H
#define QUICKWEBSHORTCUTS_UTILITIES_H

#include <QtCore>
#include <QNetworkProxy>
#include "Config.h"

void initializeConfigFile() {
    const QString configFolder = QDir::homePath() + QStringLiteral("/.config/krunnerplugins/");
    const QDir configDir(configFolder);
    if (!configDir.exists()) { configDir.mkpath(configFolder); }
    // Create file
    QFile configFile(configFolder + Config::ConfigFile);
    if (!configFile.exists()) {
        configFile.open(QIODevice::WriteOnly);
        configFile.close();
    }
}

QString loadPrivateBrowser() {
    // Read entry for private browsing launch command
    QString privateBrowser;
    QString browser = KSharedConfig::openConfig(QDir::homePath() + QStringLiteral("/.kde/share/config/kdeglobals"))
        ->group("General").readEntry("BrowserApplication");
    if (browser.isEmpty()) {
        browser = KSharedConfig::openConfig(QDir::homePath() + QStringLiteral("/./config/kdeglobals"))
            ->group("General").readEntry("BrowserApplication");
    }
    if (!browser.isEmpty()) {
        const KSharedConfig::Ptr browserConfig = KSharedConfig::openConfig(QStringLiteral("/usr/share/applications/") + browser);
        const auto groupList = browserConfig->groupList();
        for (const auto &group: groupList) {
            if (group.contains(QStringLiteral("incognito"), Qt::CaseInsensitive) ||
                group.contains(QStringLiteral("private"), Qt::CaseInsensitive)) {
                privateBrowser = browserConfig->group(group).readEntry("Exec");
            }
        }
    }
    return privateBrowser.isEmpty() ? QStringLiteral("firefox --private-window") : privateBrowser;
}

SearchEngine getDefaultSearchEngine() {
    SearchEngine defaultEngine;
    defaultEngine.qIcon = QIcon::fromTheme(QStringLiteral("google"));
    defaultEngine.name = QStringLiteral("Google");
    defaultEngine.url = QStringLiteral("https://www.google.com/search?q=");
    return defaultEngine;
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
