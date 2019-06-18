/******************************************************************************
 *  Copyright (C) 2013 – 2018 by David Baum <david.baum@naraesk.eu>           *
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
    m_ui->searchEngines->addItem("Google", "https://www.google.com/search?q=");
    m_ui->searchEngines->addItem("DuckDuckGo", "https://duckduckgo.com/?q=");
    m_ui->searchEngines->addItem("Stackoverflow", "https://stackoverflow.com/search?q=");
    m_ui->searchEngines->addItem("Bing", "https://www.bing.com/search?q=");
    m_ui->searchEngines->addItem("Github", "https://github.com/search?q=");
    m_ui->searchEngines->addItem("Youtube", "https://www.youtube.com/results?search_query=");
    KConfigGroup kse = config.group("CustomSearchEngines");
    for (const QString &key:kse.keyList()) {
        m_ui->searchEngines->addItem(key, kse.readEntry(key));
    }

    connect(m_ui->searchEngineURL, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->searchEngines, SIGNAL(currentTextChanged(QString)), this, SLOT(comboBoxEditTextChanged()));
    connect(m_ui->searchEngineName, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->searchEngines, SIGNAL(currentTextChanged(QString)), this, SLOT(changed()));
    connect(m_ui->historyAll, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->historyQuick, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->historyNotClear, SIGNAL(clicked(bool)), this, SLOT(changed()));

    connect(m_ui->searchEngineURL, SIGNAL(textChanged(QString)), this, SLOT(extractNameFromURL()));
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

    /*if (!m_ui->searchEngines->currentText().endsWith(" (current)")) {
        QString text = m_ui->searchEngines->currentText().append(" (current)");
        m_ui->searchEngines->setItemText(current, text);
    }*/
    changed();
    m_ui->searchEngines->setFocus();
    emit changed(false);
}


void QuickWebShortcutsConfig::extractNameFromURL() {
    if (m_ui->searchEngineURL->text().contains(QRegExp(R"(^(?:https?://)?(?:[\w]+\.)(?:\.?[\w]{2,})+)"))) {
        QRegExp exp(R"(^(?:https?://)?([\w]+)\.(?:\.?[\w]{2,})+)");
        exp.indexIn(m_ui->searchEngineURL->text());
        QString res = exp.capturedTexts().last();
        res[0] = res[0].toUpper();
        m_ui->searchEngineName->setText(res);
        m_ui->searchEngineName->setEnabled(true);
    }
}

void QuickWebShortcutsConfig::save() {

    KCModule::save();
    //TODO Detect changes if existing search engines are edited
    config.writeEntry("url", m_ui->searchEngines->itemData(m_ui->searchEngines->currentIndex()));
    QString history;
    if (m_ui->historyAll->isChecked()) {
        history = "all";
    } else if (m_ui->historyQuick->isChecked()) {
        history = "quick";
    } else {
        history = "false";
    }
    config.writeEntry("clean_history", history);
    config.sync();
    emit changed(false);
}

void QuickWebShortcutsConfig::defaults() {
    m_ui->historyAll->setChecked(true);
    m_ui->searchEngines->setCurrentIndex(m_ui->searchEngines->findData("https://www.google.com/search?q="));
    emit changed(true);
}

void QuickWebShortcutsConfig::enableEditingOfExisting() {
    bool enabled = m_ui->searchEnginesEditable->isChecked();
    searchEnginesEdited = true;
    m_ui->searchEngines->setEditable(enabled);
    for (int i = 0; i < m_ui->searchEngines->count(); i++) {
        qDebug() << m_ui->searchEngines->itemText(i) << "\n";
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

    m_ui->searchEngines->setItemText(m_ui->searchEngines->currentIndex(), m_ui->searchEngines->currentText());
}

#include "quick_web_shortcuts_config.moc"
