/******************************************************************************
 *  Copyright (C) 2019 by Alex <alexkp12355@gmail.com>                        *
 *                                                                            *
 *  This library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as published  *
 *  by the Free Software Foundation; either version 2 of the License or (at   *
 *  your option) any later version.                                           *
 *                                                                            *
 *  This library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 *  Library General Public License for more details.                          *
 *                                                                            *
 *  You should have received a copy of the GNU Lesser General Public License  *
 *  along with this library; see the file COPYING.LIB.                        *
 *  If not, see <http://www.gnu.org/licenses/>.                               *
 *****************************************************************************/

#include "quick_web_shortcuts_config.h"
#include "../SearchEngines.h"
#include <KSharedConfig>
#include <KPluginFactory>
#include <krunner/abstractrunner.h>
#include <QtDebug>
#include <QtWidgets/QGridLayout>
#include <iostream>
#include <QtWidgets/QComboBox>

K_PLUGIN_FACTORY(QuickWebShortcutsConfigFactory,
                 registerPlugin<QuickWebShortcutsConfig>("kcm_krunner_quickwebshortcuts");)

QuickWebShortcutsConfigForm::QuickWebShortcutsConfigForm(QWidget *parent) : QWidget(parent) {
    setupUi(this);
}

QuickWebShortcutsConfig::QuickWebShortcutsConfig(QWidget *parent, const QVariantList &args) :
        KCModule(parent, args) {
    m_ui = new QuickWebShortcutsConfigForm(this);
    auto *layout = new QGridLayout(this);
    layout->addWidget(m_ui, 0, 0);

    config = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("QuickWebShortcuts");
    icons = SearchEngines::getIcons();

    for (const auto &item : SearchEngines::getDefaultSearchEngines().toStdMap()) {
        m_ui->searchEngines->addItem(item.first, item.second);
        if (icons.contains(item.first)) {
            m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, QIcon(icons.value(item.first)));
        } else {
            m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, QIcon::fromTheme("globe"));
        }
    }
    for (const auto &item : SearchEngines::getCustomSearchEngines().toStdMap()) {
        int duplicate = m_ui->searchEngines->findText(item.first);
        if (duplicate != -1) {
            m_ui->searchEngines->setItemText(duplicate, item.first);
            m_ui->searchEngines->setItemData(duplicate, item.second);
        } else {
            m_ui->searchEngines->addItem(item.first, item.second);
            if (icons.contains(item.first)) {
                m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, QIcon(icons.value(item.first)));
            } else {
                m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, QIcon::fromTheme("globe"));
            }
        }
    }
    m_ui->deleteButton->setVisible(false);
    m_ui->showSearchEngineName->setChecked(config.readEntry("show_name", "false") == "true");

    connect(m_ui->searchEngineURL, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->searchEngines, SIGNAL(currentTextChanged(QString)), this, SLOT(comboBoxEditTextChanged()));
    connect(m_ui->searchEngineName, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->searchEngines, SIGNAL(currentTextChanged(QString)), this, SLOT(changed()));
    connect(m_ui->historyAll, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->historyQuick, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->historyNotClear, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->showSearchEngineName, SIGNAL(clicked(bool)), this, SLOT(changed()));

    connect(m_ui->searchEngineURL, SIGNAL(textChanged(QString)), this, SLOT(extractNameFromURL()));
    connect(m_ui->deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteCurrentItem()));
    connect(m_ui->searchEnginesEditable, SIGNAL(clicked(bool)), this, SLOT(enableEditingOfExisting()));
    connect(m_ui->addSearchEngine, SIGNAL(clicked(bool)), this, SLOT(addSearchEngine()));

    load();

}

void QuickWebShortcutsConfig::load() {
    KCModule::load();
    QString historyOption = config.readEntry("clean_history", "all");
    if (historyOption == "all") {
        m_ui->historyAll->setChecked(true);
    } else if (historyOption == "quick") {
        m_ui->historyQuick->setChecked(true);
    } else {
        m_ui->historyNotClear->setChecked(true);
    }
    int current = m_ui->searchEngines->findData(config.readEntry("url", "https://www.google.com/search?q="));
    m_ui->searchEngines->setCurrentIndex(current);
    m_ui->searchEngines->setFocus();
    emit changed(false);
}


void QuickWebShortcutsConfig::extractNameFromURL() {
    if (m_ui->searchEngineURL->text().contains(QRegExp(R"(^(?:https?://)?(www\.)?(?:[\w-]+\.)(?:\.?[\w]{2,})+)"))) {
        QRegExp exp(R"(^(?:https?://)(www\.)?([^/]+)\.(?:\.?[\w]{2,})+/?)");
        exp.indexIn(m_ui->searchEngineURL->text());
        QString res = exp.capturedTexts().last();
        res[0] = res[0].toUpper();
        m_ui->searchEngineName->setText(res);
        m_ui->searchEngineName->setEnabled(true);
    }
}

void QuickWebShortcutsConfig::save() {

    KCModule::save();

    for (int i = 0; i < m_ui->searchEngines->count(); i++) {
        QString text = m_ui->searchEngines->itemText(i);
        QString data = m_ui->searchEngines->itemData(i).toString();

        if (text.contains(": ")) {
            auto split = text.split(": ");
            data = split.last();
            config.group("CustomSearchEngines").writeEntry(split.first(), split.last());
            if (text == m_ui->searchEngines->currentText()) {
                config.writeEntry("url", split.last());
            }
        } else {
            if (text == m_ui->searchEngines->currentText()) {
                config.writeEntry("url", data);
            }
        }
    }

    QString history;
    if (m_ui->historyAll->isChecked()) {
        history = "all";
    } else if (m_ui->historyQuick->isChecked()) {
        history = "quick";
    } else {
        history = "false";
    }
    config.writeEntry("clean_history", history);
    config.writeEntry("show_name", m_ui->showSearchEngineName->isChecked() ? "true" : "false");
    config.sync();
    emit changed(false);
}

void QuickWebShortcutsConfig::defaults() {
    m_ui->historyAll->setChecked(true);
    while (m_ui->searchEngines->currentIndex() != -1) {
        m_ui->searchEngines->removeItem(m_ui->searchEngines->currentIndex());
    }
    // Delete all changed default entries, keep custom, refresh combo box with default + custom
    auto keys = config.group("CustomSearchEngines").keyList();
    QStringList defaults = SearchEngines::getDefaultSearchEngineNames();
    for (const auto &key : keys) {
        if (defaults.contains(key)) {
            config.group("CustomSearchEngines").deleteEntry(key);
        }
    }

    for (const auto &item : SearchEngines::getDefaultSearchEngines().toStdMap()) {
        m_ui->searchEngines->addItem(item.first, item.second);
        m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, QIcon(icons.value(item.first)));
    }
    for (const auto &item : SearchEngines::getCustomSearchEngines().toStdMap()) {
        m_ui->searchEngines->addItem(item.first, item.second);
        m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, QIcon::fromTheme("globe"));
    }

    m_ui->showSearchEngineName->setChecked(false);
    m_ui->deleteButton->setVisible(false);
    m_ui->searchEnginesEditable->setChecked(false);
    m_ui->searchEngines->setEditable(false);
    m_ui->searchEngines->setCurrentIndex(m_ui->searchEngines->findData("https://www.google.com/search?q="));

    emit changed(true);
}

void QuickWebShortcutsConfig::enableEditingOfExisting() {
    bool enabled = m_ui->searchEnginesEditable->isChecked();
    m_ui->deleteButton->setVisible(enabled);
    m_ui->searchEngines->setEditable(enabled);
    for (int i = 0; i < m_ui->searchEngines->count(); i++) {
        if (enabled) {
            if (!m_ui->searchEngines->itemText(i).contains(": ")) {
                m_ui->searchEngines->setItemText(i, m_ui->searchEngines->itemText(i) + ": " +
                                                    m_ui->searchEngines->itemData(i).toString());
            }
        } else {
            if (m_ui->searchEngines->itemText(i).endsWith(": " + m_ui->searchEngines->itemData(i).toString())) {
                QStringList res = m_ui->searchEngines->itemText(i).split(": ");
                if (res.last() == m_ui->searchEngines->itemData(i)) {
                    m_ui->searchEngines->setItemText(i, res.first());
                }
            }
        }
    }
}

void QuickWebShortcutsConfig::addSearchEngine() {
    QString name = m_ui->searchEngineName->text();
    QString url = m_ui->searchEngineURL->text();
    if (!name.isEmpty() && !url.isEmpty()) {
        config.group("CustomSearchEngines").writeEntry(name, url);
        m_ui->searchEngineName->setText("");
        m_ui->searchEngineURL->setText("");
        if (!m_ui->searchEnginesEditable->isChecked()) {
            m_ui->searchEngines->addItem(name, url);
        } else {
            m_ui->searchEngines->addItem(name + ": " + url, url);
        }
    }
}

void QuickWebShortcutsConfig::comboBoxEditTextChanged() {
    bool deletable = true;
    for (const auto &key:SearchEngines::getDefaultSearchEngineNames()) {
        if (m_ui->searchEngines->currentText().split(":").first() == key) {
            deletable = false;
            break;
        }
    }
    m_ui->deleteButton->setEnabled(deletable);
    m_ui->searchEngines->setItemText(m_ui->searchEngines->currentIndex(), m_ui->searchEngines->currentText());
}

void QuickWebShortcutsConfig::deleteCurrentItem() {
    QString text = m_ui->searchEngines->currentText();
    QString name = text;
    if (text.contains(": ")) {
        auto split = text.split(": ");
        name = split.first().trimmed();
    }

    config.group("CustomSearchEngines").deleteEntry(name);
    m_ui->searchEngines->removeItem(m_ui->searchEngines->currentIndex());
}

#include "quick_web_shortcuts_config.moc"
