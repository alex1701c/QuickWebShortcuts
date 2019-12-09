#ifndef QUICKWEBSHORTCUTSCONFIG_H
#define QUICKWEBSHORTCUTSCONFIG_H

#include "ui_quick_web_shortcuts_config.h"
#include "ui_browser_item.h"
#include <KCModule>
#include <KConfigCore/KConfigGroup>
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

class SearchEngineItem : public QWidget, public Ui::SearchEngineItemUi {
Q_OBJECT

public:
    QWidget *parentModule;
    QString originalName, originalURL, originalIcon, icon;
    bool isDefault = false;
    bool isEdited = false;
    bool isDefaultBased = false;

    explicit SearchEngineItem(QWidget *parent, QWidget *parentModule);

public Q_SLOTS:

    void extractNameFromUrl();

    void edited() { this->isEdited = true; }

    void iconPicker();
};

class QuickWebShortcutsConfig : public KCModule {
Q_OBJECT

public:
    explicit QuickWebShortcutsConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());

    ~QuickWebShortcutsConfig() override;

    KConfigGroup config;
#ifndef NO_PROXY_INTEGRATION
    QTime timeBeforeRequest;
    Wallet *wallet;
#endif

public Q_SLOTS:

    void save() override;

    void load() override;

    void defaults() override;

    void insertLocaleSelectData();

    void addSearchEngine();

    void deleteCurrentItem();

    void validateSearchSuggestions();

    void validateProxyOptions();

    void itemSelected();

    void showSearchForClicked();

    // Disable proxy options of the KWallet library is not available
    void validateProxyConnection();

    void showProxyConnectionValidationResults(QNetworkReply *reply);

    void readKWalletEntries();

    void saveKWalletEntries();


private:
    QuickWebShortcutsConfigForm *m_ui;
};

#endif
