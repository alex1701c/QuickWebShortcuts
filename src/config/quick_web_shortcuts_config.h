#ifndef TRANSLATORCONFIG_H
#define TRANSLATORCONFIG_H

#include "ui_quick_web_shortcuts_config.h"
#include <KCModule>
#include <KConfigCore/KConfigGroup>


class QuickWebShortcutsConfigForm : public QWidget, public Ui::QuickWebShortcutsConfigUi {
Q_OBJECT

public:
    explicit QuickWebShortcutsConfigForm(QWidget *parent);
};

class QuickWebShortcutsConfig : public KCModule {
Q_OBJECT

public:
    explicit QuickWebShortcutsConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());

    KConfigGroup config;

    QMap<QString, QIcon> icons;

public Q_SLOTS:

    void save() override;

    void load() override;

    void defaults() override;

    void insertLocaleSelectData();

    void extractNameFromURL();

    void enableEditingOfExisting();

    void addSearchEngine();

    void comboBoxEditTextChanged();

    void deleteCurrentItem();

    void validateSearchSuggestions();

    void validateProxyOptions();

private:
    QuickWebShortcutsConfigForm *m_ui;
};

#endif
