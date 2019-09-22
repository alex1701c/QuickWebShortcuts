#ifndef QUICKWEBSHORTCUTSCONFIG_H
#define QUICKWEBSHORTCUTSCONFIG_H

#include "ui_quick_web_shortcuts_config.h"
#include "ui_browser_item.h"
#include <KCModule>
#include <KConfigCore/KConfigGroup>
#include <QtNetwork>

class QuickWebShortcutsConfigForm : public QWidget, public Ui::QuickWebShortcutsConfigUi {
Q_OBJECT

public:
    explicit QuickWebShortcutsConfigForm(QWidget *parent) : QWidget(parent) {
        setupUi(this);
    }

};

class BrowserItem : public QWidget, public Ui::BrowserItemUi {
Q_OBJECT

public:
    QWidget *parentModule;

    explicit BrowserItem(QWidget *parent, QWidget *parentModule);

public Q_SLOTS:

    void extractNameFromUrl();
};

class QuickWebShortcutsConfig : public KCModule {
Q_OBJECT

public:
    explicit QuickWebShortcutsConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());

    KConfigGroup config;

    QMap<QString, QIcon> icons;
    QTime timeBeforeRequest;

public Q_SLOTS:

    void save() override;

    void load() override;

    void defaults() override;

    void insertLocaleSelectData();

    void enableEditingOfExisting();

    void addSearchEngine();

    void deleteCurrentItem();

    void validateSearchSuggestions();

    void validateProxyOptions();

    void validateProxyConnection();

    void showProxyConnectionValidationResults(QNetworkReply *reply);

private:
    QuickWebShortcutsConfigForm *m_ui;
};

#endif
