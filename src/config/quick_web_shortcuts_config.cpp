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
    m_ui->searchEngines->addItem("Google : https://www.google.com/search?q=");
    m_ui->searchEngines->addItem("DuckDuckGo : https://duckduckgo.com/?q=");
    m_ui->searchEngines->addItem("Stackoverflow : https://stackoverflow.com/search?q=");
    m_ui->searchEngines->addItem("Bing : https://www.bing.com/search?q=");
    m_ui->searchEngines->addItem("Github : https://github.com/search?q=");
    m_ui->searchEngines->addItem("Youtube : https://www.youtube.com/results?search_query=");
    load();
}

void QuickWebShortcutsConfig::load() {
    KCModule::load();

    KSharedConfig::Ptr cfg = KSharedConfig::openConfig(QStringLiteral("krunnerrc"));
    KConfigGroup grp = cfg->group("Runners");
    grp = KConfigGroup(&grp, "Translator");

    emit changed(false);
}

void QuickWebShortcutsConfig::save() {
    emit changed(false);
}

#include "quick_web_shortcuts_config.moc"
