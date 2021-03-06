#ifndef QUICKWEBSHORTCUTSCONFIG_H
#define QUICKWEBSHORTCUTSCONFIG_H

#include "ui_quick_web_shortcuts_config.h"
#include <KCModule>
#include <KConfigCore/KConfigGroup>
#include "SearchEngineItem.h"
#include <QtNetwork>

#ifndef NO_PROXY_INTEGRATION

#include <KWallet/KWallet>

using KWallet::Wallet;

#endif

class QuickWebShortcutsConfigForm : public QWidget, public Ui::QuickWebShortcutsConfigUi {
Q_OBJECT

public:
    explicit QuickWebShortcutsConfigForm(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }

};

class QuickWebShortcutsConfig : public KCModule {
Q_OBJECT

public:
    explicit QuickWebShortcutsConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());
    ~QuickWebShortcutsConfig() override;

public Q_SLOTS:

    void save() override;
    void load() override;
    void defaults() override;

    void addSearchEngine();
    void connectSearchEngineSignals(SearchEngineItem *);
    void deleteCurrentItem();
    void validateSearchSuggestions();
    void validateProxyOptions();
    void itemSelected();
    void showSearchForClicked();

    void validateProxyConnection();
    void showProxyConnectionValidationResults(QNetworkReply *reply);

    void readKWalletEntries();
    void saveKWalletEntries();


private:
    QuickWebShortcutsConfigForm *m_ui;
    KConfigGroup config;
    QIcon globeIcon = QIcon::fromTheme("globe");
#ifndef NO_PROXY_INTEGRATION
    QTime timeBeforeRequest;
    Wallet *wallet;
#endif
};

#endif
