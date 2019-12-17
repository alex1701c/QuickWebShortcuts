#include "quick_web_shortcuts_config.h"
#include "searchengines/SearchEngines.h"
#include "api_language_utility.h"
#include <KSharedConfig>
#include <KPluginFactory>
#include <QtDebug>
#include <QtWidgets/QFileDialog>
#include <utilities.h>

K_PLUGIN_FACTORY(QuickWebShortcutsConfigFactory,
                 registerPlugin<QuickWebShortcutsConfig>("kcm_krunner_quickwebshortcuts");)

QuickWebShortcutsConfig::QuickWebShortcutsConfig(QWidget *parent, const QVariantList &args) : KCModule(parent, args) {
    m_ui = new QuickWebShortcutsConfigForm(this);
    auto *layout = new QGridLayout(this);
    layout->addWidget(m_ui, 0, 0);

    initializeConfigFile();
    config = KSharedConfig::openConfig(QDir::homePath() + "/.config/krunnerplugins/quickwebshortcutsrunnerrc")
            ->group("Config");
    config.config()->reparseConfiguration();

    // Initialize function pointers that require method overloading
    const auto changedSlotPointer = static_cast<void (QuickWebShortcutsConfig::*)()>(&QuickWebShortcutsConfig::changed);
    const auto comboBoxIndexChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
    const auto spinBoxValueChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);

    // History GroupBox
    connect(m_ui->historyAll, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->historyQuick, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->historyNotClear, &QCheckBox::clicked, this, changedSlotPointer);
    // Search Engines
    connect(m_ui->openURLS, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->showSearchEngineName, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->showPrivateNoteCheckBox, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->showSearchForCheckBox, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->showSearchForCheckBox, &QCheckBox::clicked, this, &QuickWebShortcutsConfig::showSearchForClicked);
    connect(m_ui->addSearchEngine, &QCheckBox::clicked, this, &QuickWebShortcutsConfig::addSearchEngine);
    connect(m_ui->addSearchEngine, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->triggerCharacterComboBox, comboBoxIndexChanged, this, changedSlotPointer);
    // Search Suggestions GroupBox
    connect(m_ui->googleRadioButton, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->googleRadioButton, &QCheckBox::clicked, this, &QuickWebShortcutsConfig::validateSearchSuggestions);
    connect(m_ui->bingRadioButton, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->bingRadioButton, &QCheckBox::clicked, this, &QuickWebShortcutsConfig::validateSearchSuggestions);
    connect(m_ui->duckDuckGoRadioButton, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->duckDuckGoRadioButton, &QCheckBox::clicked, this, &QuickWebShortcutsConfig::validateSearchSuggestions);
    connect(m_ui->disableRadioButton, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->disableRadioButton, &QCheckBox::clicked, this, &QuickWebShortcutsConfig::validateSearchSuggestions);
    connect(m_ui->privateWindowCheckBox, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->minimumLetterCountSpinBox, spinBoxValueChanged, this, changedSlotPointer);
    connect(m_ui->maxSearchSuggestionsSpinBox, spinBoxValueChanged, this, changedSlotPointer);
    connect(m_ui->bingLocaleSelectComboBox, comboBoxIndexChanged, this, changedSlotPointer);
    connect(m_ui->googleLanguageComboBox, comboBoxIndexChanged, this, changedSlotPointer);
// Signals/Slots that depend on the proxy feature to be enabled
#ifndef NO_PROXY_INTEGRATION
    wallet = Wallet::openWallet(Wallet::LocalWallet(), 0, Wallet::Synchronous);
    connect(m_ui->noProxyRadioButton, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->httpProxyRadioButton, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->socks5ProxyRadioButton, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->noProxyRadioButton, SIGNAL(clicked(bool)), this, SLOT(validateProxyOptions()));
    connect(m_ui->httpProxyRadioButton, SIGNAL(clicked(bool)), this, SLOT(validateProxyOptions()));
    connect(m_ui->socks5ProxyRadioButton, SIGNAL(clicked(bool)), this, SLOT(validateProxyOptions()));
    connect(m_ui->testProxyConfigPushButton, SIGNAL(clicked(bool)), this, SLOT(validateProxyConnection()));
    connect(m_ui->hostNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->portLineEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->usernameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->passwordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->showErrorsCheckBox, &QCheckBox::clicked, this, changedSlotPointer);
#endif
}

QuickWebShortcutsConfig::~QuickWebShortcutsConfig() {
#ifndef NO_PROXY_INTEGRATION
    delete wallet;
#endif
}

void QuickWebShortcutsConfig::load() {
    QString searchEngineName = config.readEntry(Config::SearchEngineName);
    const auto searchEngineNames = SearchEngines::getDefaultSearchEngineNames();
    if (searchEngineName.isEmpty()) searchEngineName = "Google";
    // Load search engines
    for (const auto &item : SearchEngines::getAllSearchEngines()) {
        auto *browserItem = new SearchEngineItem(m_ui->groupBoxSearch);
        m_ui->searchEnginesItemLayout->addWidget(browserItem);
        browserItem->iconPushButton->setIcon(item.qIcon);
        browserItem->nameLineEdit->setText(item.name);
        browserItem->urlLineEdit->setText(item.url);
        browserItem->useRadioButton->setChecked(item.name == searchEngineName);
        browserItem->isDefault = item.isDefault;
        browserItem->isDefaultBased = item.isDefaultBased;
        browserItem->isEdited = false;
        browserItem->icon = item.icon;
        browserItem->iconPushButton->setIcon(QIcon::fromTheme(item.icon));
        if (item.isDefault) {
            browserItem->originalName = item.name;
            browserItem->originalURL = item.url;
            browserItem->originalIcon = item.icon;
        } else if (item.isDefaultBased) {
            browserItem->originalName = item.originalName;
            browserItem->originalURL = item.originalURL;
            browserItem->originalIcon = item.originalIcon;
        }
        browserItem->deletePushButton->setDisabled(item.isDefault || searchEngineNames.contains(item.originalName));
        connectSearchEngineSignals(browserItem);
    }
    m_ui->showSearchEngineName->setChecked(config.readEntry(Config::ShowName, true));
    m_ui->openURLS->setChecked(config.readEntry(Config::OpenUrls, true));
    m_ui->showSearchForCheckBox->setChecked(config.readEntry(Config::ShowSearchForNote, true));
    m_ui->showPrivateNoteCheckBox->setChecked(config.readEntry(Config::PrivateWindowNote, true));
    m_ui->triggerCharacterComboBox->setCurrentText(config.readEntry(Config::TriggerCharacter, Config::TriggerCharacterDefault));
    showSearchForClicked();

    // Search Suggestions settings
    const QString searchSuggestionOption = config.readEntry(Config::SearchSuggestions, Config::SearchSuggestionDisabled);
    if (searchSuggestionOption == Config::SearchSuggestionGoogle) {
        m_ui->googleRadioButton->setChecked(true);
    } else if (searchSuggestionOption == Config::SearchSuggestionBing) {
        m_ui->bingRadioButton->setChecked(true);
    } else if (searchSuggestionOption == Config::SearchSuggestionDuckDuckGo) {
        m_ui->duckDuckGoRadioButton->setChecked(true);
    } else {
        m_ui->disableRadioButton->setChecked(true);
    }
    m_ui->privateWindowCheckBox->setChecked(config.readEntry(Config::PrivateWindowSearchSuggestions, false));
    m_ui->minimumLetterCountSpinBox->setValue(config.readEntry(Config::MinimumLetterCount, Config::MinimumLetterCountDefault));
    m_ui->maxSearchSuggestionsSpinBox->setValue(config.readEntry(Config::MaxSuggestionResults, Config::MaxSuggestionResultsDefault));

    setBingLanguages(m_ui->bingLocaleSelectComboBox);
    setGoogleLanguages(m_ui->googleLanguageComboBox);
    m_ui->bingLocaleSelectComboBox->setCurrentIndex(m_ui->bingLocaleSelectComboBox->findData(
            config.readEntry(Config::BingMarket, Config::BingMarketDefault)));
    m_ui->googleLanguageComboBox->setCurrentIndex(m_ui->googleLanguageComboBox->findData(
            config.readEntry(Config::GoogleLocale, Config::GoogleLocaleDefault)));
    validateSearchSuggestions();
    // Shows general warnings, hidden by default
    m_ui->topLabel->setHidden(true);

#ifndef NO_PROXY_INTEGRATION
    // Proxy settings
    const QString proxy = config.readEntry(Config::Proxy);
    if (proxy == "http") m_ui->httpProxyRadioButton->setChecked(true);
    else if (proxy == "socks5") m_ui->socks5ProxyRadioButton->setChecked(true);
    else m_ui->noProxyRadioButton->setChecked(true);

    readKWalletEntries();
    validateProxyOptions();
#else
    m_ui->proxyDataContainerWidget->setHidden(true);
    m_ui->suggestionsProxyWidget->setHidden(true);
#endif

    // Clear History settings
    QString historyOption = config.readEntry(Config::CleanHistory, Config::CleanHistoryDefault);
    if (historyOption == Config::CleanHistoryAll) {
        m_ui->historyAll->setChecked(true);
    } else if (historyOption == Config::CleanHistoryQuick) {
        m_ui->historyQuick->setChecked(true);
    } else {
        m_ui->historyNotClear->setChecked(true);
    }

    emit changed(false);
}


void QuickWebShortcutsConfig::save() {
    // Remove old groups
    const int itemCount = m_ui->searchEnginesItemLayout->count();
    for (const auto &groupName:config.groupList().filter(QRegExp("^SearchEngine-"))) {
        config.group(groupName).deleteGroup();
    }
    // Write items to config
    QString selected;
    for (int i = 0; i < itemCount; ++i) {
        auto *item = reinterpret_cast<SearchEngineItem *>(m_ui->searchEnginesItemLayout->itemAt(i)->widget());

        const QString itemName = item->nameLineEdit->text();
        if (item->useRadioButton->isChecked()) selected = itemName;
        if (item->isDefault && !item->isEdited) continue;
        const QString itemUrl = item->urlLineEdit->text();

        if (itemName.isEmpty() || itemUrl.isEmpty()) continue;

        auto itemConfig = config.group("SearchEngine-" + itemName);
        itemConfig.writeEntry(SearchEngineConfig::Name, itemName);
        itemConfig.writeEntry(SearchEngineConfig::Url, itemUrl);
        itemConfig.writeEntry(SearchEngineConfig::Icon, item->icon);
        // For reference to original state (reset using defaults button)
        if (item->isDefault || item->isDefaultBased) {
            itemConfig.writeEntry(SearchEngineConfig::OriginalName, item->originalName);
        }
    }
    config.writeEntry(Config::SearchEngineName, selected);

    QString searchSuggestionsOption;
    if (m_ui->googleRadioButton->isChecked()) {
        searchSuggestionsOption = Config::SearchSuggestionGoogle;
    } else if (m_ui->bingRadioButton->isChecked()) {
        searchSuggestionsOption = Config::SearchSuggestionBing;
    } else if (m_ui->duckDuckGoRadioButton->isChecked()) {
        searchSuggestionsOption = Config::SearchSuggestionDuckDuckGo;
    } else {
        searchSuggestionsOption = Config::SearchSuggestionDisabled;
    }
    config.writeEntry(Config::SearchSuggestions, searchSuggestionsOption);

    config.writeEntry(Config::PrivateWindowSearchSuggestions, m_ui->privateWindowCheckBox->isChecked());
    config.writeEntry(Config::MinimumLetterCount, m_ui->minimumLetterCountSpinBox->value());
    config.writeEntry(Config::MaxSuggestionResults, m_ui->maxSearchSuggestionsSpinBox->value());
    config.writeEntry(Config::BingMarket, m_ui->bingLocaleSelectComboBox->itemData(
            m_ui->bingLocaleSelectComboBox->currentIndex()));
    config.writeEntry(Config::GoogleLocale, m_ui->googleLanguageComboBox->itemData(
            m_ui->googleLanguageComboBox->currentIndex()));

    QString proxy;
    if (m_ui->httpProxyRadioButton->isChecked()) proxy = "http";
    else if (m_ui->socks5ProxyRadioButton->isChecked()) proxy = "socks5";
    else proxy = Config::ProxyDisabled;
    config.writeEntry(Config::Proxy, proxy);

#ifndef NO_PROXY_INTEGRATION
    saveKWalletEntries();
    config.writeEntry(Config::ProxyShowErrors, m_ui->showErrorsCheckBox->isChecked());
#endif

    QString history;
    if (m_ui->historyAll->isChecked()) {
        history = Config::CleanHistoryAll;
    } else if (m_ui->historyQuick->isChecked()) {
        history = Config::CleanHistoryQuick;
    } else {
        history = Config::CleanHistoryNone;
    }
    config.writeEntry(Config::CleanHistory, history);
    config.writeEntry(Config::ShowName, m_ui->showSearchEngineName->isChecked());
    config.writeEntry(Config::OpenUrls, m_ui->openURLS->isChecked());
    config.writeEntry(Config::ShowSearchForNote, m_ui->showSearchForCheckBox->isChecked());
    config.writeEntry(Config::PrivateWindowNote, m_ui->showPrivateNoteCheckBox->isChecked());
    config.writeEntry(Config::TriggerCharacter, m_ui->triggerCharacterComboBox->currentText());

    config.config()->sync();

    emit changed(false);
}

void QuickWebShortcutsConfig::defaults() {
    m_ui->historyQuick->setChecked(true);
    const int itemCount = m_ui->searchEnginesItemLayout->count();
    for (int i = 0; i < itemCount; ++i) {
        auto *item = reinterpret_cast<SearchEngineItem *>(m_ui->searchEnginesItemLayout->itemAt(i)->widget());

        if (item->isDefaultBased) {
            if ((item->isDefault && item->isEdited) || (!item->isDefault && item->isDefaultBased)) {
                item->nameLineEdit->setText(item->originalName);
                item->urlLineEdit->setText(item->originalURL);
                item->iconPushButton->setIcon(QIcon::fromTheme(item->originalIcon));
                item->isEdited = false;
            }
            item->useRadioButton->setChecked(item->originalName == "Google");
        } else {
            item->useRadioButton->setChecked(false);
            continue;
        }

    }
    m_ui->showSearchEngineName->setChecked(false);
    m_ui->showPrivateNoteCheckBox->setChecked(true);
    m_ui->showSearchForCheckBox->setChecked(true);
    m_ui->openURLS->setChecked(true);
    m_ui->privateWindowCheckBox->setChecked(false);
    m_ui->disableRadioButton->setChecked(true);
    m_ui->noProxyRadioButton->setChecked(true);
    m_ui->minimumLetterCountSpinBox->setValue(Config::MinimumLetterCountDefault);
    m_ui->bingLocaleSelectComboBox->setCurrentIndex(m_ui->bingLocaleSelectComboBox->findData(Config::BingMarketDefault));
    m_ui->googleLanguageComboBox->setCurrentIndex(m_ui->googleLanguageComboBox->findData(Config::GoogleLocaleDefault));
    m_ui->triggerCharacterComboBox->setCurrentText(Config::TriggerCharacterDefault);

    showSearchForClicked();
    validateSearchSuggestions();
    validateProxyOptions();
    emit changed(true);
}

void QuickWebShortcutsConfig::addSearchEngine() {
    auto *item = new SearchEngineItem(m_ui->groupBoxSearch);
    m_ui->searchEnginesItemLayout->insertWidget(0, item);
    item->iconPushButton->setIcon(QIcon::fromTheme("globe"));
}

void QuickWebShortcutsConfig::connectSearchEngineSignals(SearchEngineItem *item) {
    const auto changedSlotPointer = static_cast<void (QuickWebShortcutsConfig::*)()>(&QuickWebShortcutsConfig::changed);
    connect(item, &SearchEngineItem::changed, this, changedSlotPointer);
    connect(item, &SearchEngineItem::itemSelected, this, &QuickWebShortcutsConfig::itemSelected);
    connect(item, &SearchEngineItem::itemSelected, this, changedSlotPointer);
    connect(item, &SearchEngineItem::deleteCurrentItem, this, &QuickWebShortcutsConfig::deleteCurrentItem);
    connect(item, &SearchEngineItem::deleteCurrentItem, this, changedSlotPointer);
}

void QuickWebShortcutsConfig::deleteCurrentItem() {
    auto *item = reinterpret_cast<SearchEngineItem *>(this->sender());
    m_ui->searchEnginesItemLayout->removeWidget(item);
    item->deleteLater();
}

void QuickWebShortcutsConfig::validateSearchSuggestions() {
    const auto origin = reinterpret_cast<QRadioButton *>(sender());
    // If the current selection is clicked again it should still be selected
    if (origin != nullptr && !origin->isChecked()) {
        origin->setChecked(true);
        return;
    }
    const bool disabled = m_ui->disableRadioButton->isChecked();
    m_ui->privateWindowCheckBox->setDisabled(disabled);
    m_ui->minimumLetterCountSpinBox->setDisabled(disabled);
    m_ui->maxSearchSuggestionsSpinBox->setDisabled(disabled);
    m_ui->bingLocaleSelectComboBox->setHidden(!m_ui->bingRadioButton->isChecked());
    m_ui->googleLanguageComboBox->setHidden(!m_ui->googleRadioButton->isChecked());
}


void QuickWebShortcutsConfig::validateProxyOptions() {
    m_ui->proxyDataContainerWidget->setHidden(m_ui->noProxyRadioButton->isChecked());
}

void QuickWebShortcutsConfig::itemSelected() {
    auto *sourceItem = reinterpret_cast<SearchEngineItem *>(this->sender());
    const int itemCount = m_ui->searchEnginesItemLayout->count();
    int selected = 0;
    for (int i = 0; i < itemCount; ++i) {
        auto *item = reinterpret_cast<SearchEngineItem *>(m_ui->searchEnginesItemLayout->itemAt(i)->widget());
        if (item->useRadioButton->isChecked()) {
            ++selected;
        }
    }
    if (selected == 2) {
        // Another item is selected
        for (int i = 0; i < itemCount; ++i) {
            auto *item = reinterpret_cast<SearchEngineItem *>(m_ui->searchEnginesItemLayout->itemAt(i)->widget());
            if (item->useRadioButton->isChecked() && item != sourceItem) {
                item->useRadioButton->setChecked(false);
            }
        }
    } else {
        // The same item was clicked => it has to be selected, otherwise no item is checked
        sourceItem->useRadioButton->setChecked(true);
    }
}

void QuickWebShortcutsConfig::showSearchForClicked() {
    m_ui->showSearchEngineName->setDisabled(!m_ui->showSearchForCheckBox->isChecked());
}

#ifndef NO_PROXY_INTEGRATION

void QuickWebShortcutsConfig::validateProxyConnection() {
    auto *manager = new QNetworkAccessManager(this);
    auto *proxy = new QNetworkProxy();
    if (m_ui->httpProxyRadioButton->isChecked()) proxy->setType(QNetworkProxy::HttpProxy);
    else proxy->setType(QNetworkProxy::Socks5Proxy);
    proxy->setHostName(m_ui->hostNameLineEdit->text());
    proxy->setPort(m_ui->portLineEdit->text().toInt());
    proxy->setUser(m_ui->usernameLineEdit->text());
    proxy->setPassword(m_ui->passwordLineEdit->text());
    manager->setProxy(*proxy);
    QNetworkRequest request(QUrl("https://ifconfig.me/ip"));
    timeBeforeRequest = QTime::currentTime();
    auto initialReply = manager->get(request);
    connect(manager, &QNetworkAccessManager::finished, this, &QuickWebShortcutsConfig::showProxyConnectionValidationResults);
    m_ui->proxyTestResultLabel->setText("Making request...");

    QTimer::singleShot(5000, initialReply, [initialReply]() {
        if (!initialReply->isFinished()) {
            initialReply->abort();
        }
    });
}

void QuickWebShortcutsConfig::showProxyConnectionValidationResults(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        const int msecDuration = timeBeforeRequest.msecsTo(QTime::currentTime());
        QString text = "No Error!\nYour Ip address is: \n" + reply->readAll() + "\nThe request took:\n"
                       + QString::number(msecDuration) + " ms";
        if (msecDuration >= 2000) text.append("\n Warning! A duration longer that 2000ms will result in a timeout!");
        m_ui->proxyTestResultLabel->setText(text);
    } else {
        m_ui->proxyTestResultLabel->setText(
                QString(QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(int(reply->error()))) + ":\n" +
                reply->errorString());
    }

    delete reply;
}

void QuickWebShortcutsConfig::readKWalletEntries() {
    if (KWallet::Wallet::isEnabled() && wallet->isOpen()) {
        QByteArray hostName;
        wallet->readEntry(KWalletConfig::ProxyHostname, hostName);
        m_ui->hostNameLineEdit->setText(hostName);
        QByteArray port;
        wallet->readEntry(KWalletConfig::ProxyPort, port);
        m_ui->portLineEdit->setText(port);
        QByteArray username;
        wallet->readEntry(KWalletConfig::ProxyUsername, username);
        m_ui->usernameLineEdit->setText(username);
        QByteArray password;
        wallet->readEntry(KWalletConfig::ProxyPassword, password);
        m_ui->passwordLineEdit->setText(password);
        m_ui->showErrorsCheckBox->setChecked(config.readEntry(Config::ProxyShowErrors, true));
    } else {
        m_ui->topLabel->setText("KWallet is disabled or could not be opened, proxy credentials can not be stored!");
        m_ui->topLabel->setHidden(false);
    }
}

void QuickWebShortcutsConfig::saveKWalletEntries() {
    if (KWallet::Wallet::isEnabled() && wallet->isOpen()) {
        wallet->writeEntry(KWalletConfig::ProxyHostname, m_ui->hostNameLineEdit->text().toLocal8Bit());
        wallet->writeEntry(KWalletConfig::ProxyPort, m_ui->portLineEdit->text().toLocal8Bit());
        wallet->writeEntry(KWalletConfig::ProxyUsername, m_ui->usernameLineEdit->text().toLocal8Bit());
        wallet->writeEntry(KWalletConfig::ProxyPassword, m_ui->passwordLineEdit->text().toLocal8Bit());
    }
}

#else

// For internal qt calls
void QuickWebShortcutsConfig::validateProxyConnection() {}

void QuickWebShortcutsConfig::showProxyConnectionValidationResults(QNetworkReply *reply) { Q_UNUSED(reply) }

void QuickWebShortcutsConfig::readKWalletEntries() {}

void QuickWebShortcutsConfig::saveKWalletEntries() {}

#endif


#include "quick_web_shortcuts_config.moc"
