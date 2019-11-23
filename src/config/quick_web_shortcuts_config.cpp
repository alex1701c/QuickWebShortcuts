#include "quick_web_shortcuts_config.h"
#include "../SearchEngines.h"
#include <KSharedConfig>
#include <KPluginFactory>
#include <QtDebug>
#include <QtWidgets/QFileDialog>

K_PLUGIN_FACTORY(QuickWebShortcutsConfigFactory,
                 registerPlugin<QuickWebShortcutsConfig>("kcm_krunner_quickwebshortcuts");)

QuickWebShortcutsConfig::QuickWebShortcutsConfig(QWidget *parent, const QVariantList &args) : KCModule(parent, args) {
    m_ui = new QuickWebShortcutsConfigForm(this);
    auto *layout = new QGridLayout(this);
    layout->addWidget(m_ui, 0, 0);

    config = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("QuickWebShortcuts");

    // History GroupBox
    connect(m_ui->historyAll, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->historyQuick, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->historyNotClear, SIGNAL(clicked(bool)), this, SLOT(changed()));
    // Search Engines
    connect(m_ui->openURLS, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->showSearchEngineName, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->showPrivateNoteCheckBox, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->showSearchForCheckBox, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->showSearchForCheckBox, SIGNAL(clicked(bool)), this, SLOT(showSearchForClicked()));
    connect(m_ui->addSearchEngine, SIGNAL(clicked(bool)), this, SLOT(addSearchEngine()));
    connect(m_ui->triggerCharacterComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
    // Search Suggestions GroupBox
    connect(m_ui->googleRadioButton, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->googleRadioButton, SIGNAL(clicked(bool)), this, SLOT(validateSearchSuggestions()));
    connect(m_ui->bingRadioButton, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->bingRadioButton, SIGNAL(clicked(bool)), this, SLOT(validateSearchSuggestions()));
    connect(m_ui->duckDuckGoRadioButton, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->duckDuckGoRadioButton, SIGNAL(clicked(bool)), this, SLOT(validateSearchSuggestions()));
    connect(m_ui->disableRadioButton, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->disableRadioButton, SIGNAL(clicked(bool)), this, SLOT(validateSearchSuggestions()));
    connect(m_ui->privateWindowCheckBox, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->minimumLetterCountSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changed()));
    connect(m_ui->maxSearchSuggestionsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changed()));
    connect(m_ui->bingLocaleSelectComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
    connect(m_ui->googleLanguageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
    // Proxy Options
    connect(m_ui->noProxyRadioButton, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->httpProxyRadioButton, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->socks5ProxyRadioButton, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->noProxyRadioButton, SIGNAL(clicked(bool)), this, SLOT(validateProxyOptions()));
    connect(m_ui->httpProxyRadioButton, SIGNAL(clicked(bool)), this, SLOT(validateProxyOptions()));
    connect(m_ui->socks5ProxyRadioButton, SIGNAL(clicked(bool)), this, SLOT(validateProxyOptions()));
    connect(m_ui->hostNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->portLineEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->usernameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->passwordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->showErrorsCheckBox, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->testProxyConfigPushButton, SIGNAL(clicked(bool)), this, SLOT(validateProxyConnection()));

}

void QuickWebShortcutsConfig::load() {
    QString searchEngineName = config.readEntry("search_engine_name");
    auto searchEngineNames = SearchEngines::getDefaultSearchEngineNames();
    if (searchEngineName.isEmpty()) searchEngineName = "Google";
    // Load search engines
    for (const auto &item : SearchEngines::getAllSearchEngines()) {
        auto *browserItem = new SearchEngineItem(m_ui->groupBoxSearch, this);
        m_ui->searchEnginesItemLayout->addWidget(browserItem);
        browserItem->iconPushButton->setIcon(item.qIcon);
        browserItem->nameLineEdit->setText(item.name);
        browserItem->urlLineEdit->setText(item.url);
        browserItem->useRadioButton->setChecked(item.name == searchEngineName);
        browserItem->isDefault = item.isDefault;
        browserItem->isDefaultBased = item.isDefaultBased;
        browserItem->isEdited = false;
        browserItem->icon = item.icon;
        browserItem->iconPushButton->setIcon(item.icon.startsWith("/") ? QIcon(item.icon) : QIcon::fromTheme(item.icon));
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
    }
    m_ui->showSearchEngineName->setChecked(config.readEntry("show_name") == "true");
    m_ui->openURLS->setChecked(config.readEntry("open_urls", "true") == "true");
    m_ui->showSearchForCheckBox->setChecked(config.readEntry("show_search_for_note", "true") == "true");
    m_ui->showPrivateNoteCheckBox->setChecked(config.readEntry("show_private_window_note", "true") == "true");
    m_ui->triggerCharacterComboBox->setCurrentText(config.readEntry("trigger_character", ":"));
    showSearchForClicked();

    // Search Suggestions settings
    const QString searchSuggestionOption = config.readEntry("search_suggestions", "disabled");
    if (searchSuggestionOption == "google") {
        m_ui->googleRadioButton->setChecked(true);
    } else if (searchSuggestionOption == "bing") {
        m_ui->bingRadioButton->setChecked(true);
    } else if (searchSuggestionOption == "duckduckgo") {
        m_ui->duckDuckGoRadioButton->setChecked(true);
    } else {
        m_ui->disableRadioButton->setChecked(true);
    }
    m_ui->privateWindowCheckBox->setChecked(config.readEntry("private_window_search_suggestions", "false") == "true");
    m_ui->minimumLetterCountSpinBox->setValue(config.readEntry("minimum_letter_count", "3").toInt());
    m_ui->maxSearchSuggestionsSpinBox->setValue(config.readEntry("max_search_suggestions", "10").toInt());
    insertLocaleSelectData();
    m_ui->bingLocaleSelectComboBox->setCurrentIndex(m_ui->bingLocaleSelectComboBox->findData(
            config.readEntry("bing_locale", "en-us")));
    m_ui->googleLanguageComboBox->setCurrentIndex(m_ui->googleLanguageComboBox->findData(
            config.readEntry("google_locale", "en")));
    validateSearchSuggestions();

    // Proxy settings
    const QString proxy = config.readEntry("proxy");
    if (proxy == "http") m_ui->httpProxyRadioButton->setChecked(true);
    else if (proxy == "socks5") m_ui->socks5ProxyRadioButton->setChecked(true);
    else m_ui->noProxyRadioButton->setChecked(true);

    m_ui->hostNameLineEdit->setText(config.readEntry("proxy_hostname"));
    m_ui->portLineEdit->setText(config.readEntry("proxy_port"));
    m_ui->usernameLineEdit->setText(QString(QByteArray::fromHex(config.readEntry("proxy_username").toLocal8Bit())));
    m_ui->passwordLineEdit->setText(QString(QByteArray::fromHex(config.readEntry("proxy_password").toLocal8Bit())));
    m_ui->showErrorsCheckBox->setChecked(config.readEntry("proxy_show_errors", "true") == "true");
    validateProxyOptions();

    // Clear History settings
    QString historyOption = config.readEntry("clean_history", "quick");
    if (historyOption == "all") {
        m_ui->historyAll->setChecked(true);
    } else if (historyOption == "quick") {
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
        itemConfig.writeEntry("name", itemName);
        itemConfig.writeEntry("url", itemUrl);
        itemConfig.writeEntry("icon", item->icon);
        // For reference to original state (reset using defaults button)
        if (item->isDefault || item->isDefaultBased) {
            itemConfig.writeEntry("original_name", item->originalName);
        }
    }
    config.writeEntry("search_engine_name", selected);

    QString searchSuggestionsOption;
    if (m_ui->googleRadioButton->isChecked()) {
        searchSuggestionsOption = "google";
    } else if (m_ui->bingRadioButton->isChecked()) {
        searchSuggestionsOption = "bing";
    } else if (m_ui->duckDuckGoRadioButton->isChecked()) {
        searchSuggestionsOption = "duckduckgo";
    } else {
        searchSuggestionsOption = "disabled";
    }
    config.writeEntry("search_suggestions", searchSuggestionsOption);

    config.writeEntry("private_window_search_suggestions", m_ui->privateWindowCheckBox->isChecked());
    config.writeEntry("minimum_letter_count", m_ui->minimumLetterCountSpinBox->value());
    config.writeEntry("max_search_suggestions", m_ui->maxSearchSuggestionsSpinBox->value());
    config.writeEntry("bing_locale", m_ui->bingLocaleSelectComboBox->itemData(
            m_ui->bingLocaleSelectComboBox->currentIndex()));
    config.writeEntry("google_locale", m_ui->googleLanguageComboBox->itemData(
            m_ui->googleLanguageComboBox->currentIndex()));

    QString proxy;
    if (m_ui->httpProxyRadioButton->isChecked()) proxy = "http";
    else if (m_ui->socks5ProxyRadioButton->isChecked()) proxy = "socks5";
    else proxy = "disabled";
    config.writeEntry("proxy", proxy);

    config.writeEntry("proxy_hostname", m_ui->hostNameLineEdit->text());
    config.writeEntry("proxy_port", m_ui->portLineEdit->text());
    config.writeEntry("proxy_username", m_ui->usernameLineEdit->text().toLatin1().toHex());
    config.writeEntry("proxy_password", m_ui->passwordLineEdit->text().toLatin1().toHex());
    config.writeEntry("proxy_show_errors", m_ui->showErrorsCheckBox->isChecked());

    QString history;
    if (m_ui->historyAll->isChecked()) {
        history = "all";
    } else if (m_ui->historyQuick->isChecked()) {
        history = "quick";
    } else {
        history = "false";
    }
    config.writeEntry("clean_history", history);
    config.writeEntry("show_name", m_ui->showSearchEngineName->isChecked());
    config.writeEntry("open_urls", m_ui->openURLS->isChecked());
    config.writeEntry("show_search_for_note", m_ui->showSearchForCheckBox->isChecked());
    config.writeEntry("show_private_window_note", m_ui->showPrivateNoteCheckBox->isChecked());
    config.writeEntry("trigger_character", m_ui->triggerCharacterComboBox->currentText());
    emit changed(false);
}

void QuickWebShortcutsConfig::defaults() {
    m_ui->historyAll->setChecked(true);
    const int itemCount = m_ui->searchEnginesItemLayout->count();
    for (int i = 0; i < itemCount; ++i) {
        auto *item = reinterpret_cast<SearchEngineItem *>(m_ui->searchEnginesItemLayout->itemAt(i)->widget());

        if (item->isDefaultBased) {
            if ((item->isDefault && item->isEdited) || (!item->isDefault && item->isDefaultBased)) {
                item->nameLineEdit->setText(item->originalName);
                item->urlLineEdit->setText(item->originalURL);
                item->iconPushButton->setIcon(item->originalIcon.startsWith("/")
                                              ? QIcon(item->originalIcon) : QIcon::fromTheme(item->originalIcon));
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
    m_ui->minimumLetterCountSpinBox->setValue(3);
    m_ui->bingLocaleSelectComboBox->setCurrentIndex(m_ui->bingLocaleSelectComboBox->findData("en-us"));
    m_ui->googleLanguageComboBox->setCurrentIndex(m_ui->googleLanguageComboBox->findData("en"));
    m_ui->triggerCharacterComboBox->setCurrentText(":");

    showSearchForClicked();
    validateSearchSuggestions();
    validateProxyOptions();
    emit changed(true);
}

void QuickWebShortcutsConfig::addSearchEngine() {
    auto *item = new SearchEngineItem(m_ui->groupBoxSearch, this);
    m_ui->searchEnginesItemLayout->insertWidget(0, item);
    item->iconPushButton->setIcon(QIcon::fromTheme("globe"));
}

void QuickWebShortcutsConfig::deleteCurrentItem() {
    auto *item = reinterpret_cast<SearchEngineItem *>(this->sender()->parent());
    m_ui->searchEnginesItemLayout->removeWidget(item);
    item->deleteLater();
}

void QuickWebShortcutsConfig::validateSearchSuggestions() {
    const bool disabled = m_ui->disableRadioButton->isChecked();
    m_ui->privateWindowCheckBox->setDisabled(disabled);
    m_ui->minimumLetterCountSpinBox->setDisabled(disabled);
    m_ui->maxSearchSuggestionsSpinBox->setDisabled(disabled);
    m_ui->bingLocaleSelectComboBox->setHidden(!m_ui->bingRadioButton->isChecked());
    m_ui->googleLanguageComboBox->setHidden(!m_ui->googleRadioButton->isChecked());
}

void QuickWebShortcutsConfig::insertLocaleSelectData() {
    m_ui->bingLocaleSelectComboBox->addItem("ar-sa (Arabic)", "ar-sa");
    m_ui->bingLocaleSelectComboBox->addItem("da-dk (Danish)", "da-dk");
    m_ui->bingLocaleSelectComboBox->addItem("de-at (German-Austria)", "de-at");
    m_ui->bingLocaleSelectComboBox->addItem("de-ch (German-Switzerland)", "de-ch");
    m_ui->bingLocaleSelectComboBox->addItem("de-de (German-Germany)", "de-de");
    m_ui->bingLocaleSelectComboBox->addItem("en-au (English-Australia)", "en-au");
    m_ui->bingLocaleSelectComboBox->addItem("en-ca (English-Canada)", "en-ca");
    m_ui->bingLocaleSelectComboBox->addItem("en-gb (English-UK)", "en-gb");
    m_ui->bingLocaleSelectComboBox->addItem("en-id (English-Indonesia)", "en-id");
    m_ui->bingLocaleSelectComboBox->addItem("en-ie (English-Ireland)", "en-ie");
    m_ui->bingLocaleSelectComboBox->addItem("en-in (English-India)", "en-in");
    m_ui->bingLocaleSelectComboBox->addItem("en-my (English-Malaysia)", "en-my");
    m_ui->bingLocaleSelectComboBox->addItem("en-mx (English-Mexico)", "en-mx");
    m_ui->bingLocaleSelectComboBox->addItem("en-nz (English-New Zealand)", "en-nz");
    m_ui->bingLocaleSelectComboBox->addItem("en-ph (English-Philippines)", "en-ph");
    m_ui->bingLocaleSelectComboBox->addItem("en-us (English-United States)", "en-us");
    m_ui->bingLocaleSelectComboBox->addItem("en-za (English-South Africa)", "en-za");
    m_ui->bingLocaleSelectComboBox->addItem("es-ar (Spanish-Argentina)", "es-ar");
    m_ui->bingLocaleSelectComboBox->addItem("es-cl (Spanish-Chile)", "es-cl");
    m_ui->bingLocaleSelectComboBox->addItem("es-mx (Spanish-Mexico)", "es-mx");
    m_ui->bingLocaleSelectComboBox->addItem("es-es (Spanish-Spain)", "es-es");
    m_ui->bingLocaleSelectComboBox->addItem("es-us (Spanish-United States)", "es-us");
    m_ui->bingLocaleSelectComboBox->addItem("fi-fi (Finnish)", "fi-fi");
    m_ui->bingLocaleSelectComboBox->addItem("fr-be (French-Belgium)", "fr-be");
    m_ui->bingLocaleSelectComboBox->addItem("fr-ca (French-Canada)", "fr-ca");
    m_ui->bingLocaleSelectComboBox->addItem("fr-ch (French-Switzerland)", "fr-ch");
    m_ui->bingLocaleSelectComboBox->addItem("fr-fr (French-France)", "fr-fr");
    m_ui->bingLocaleSelectComboBox->addItem("it-it (Italian)", "it-it");
    m_ui->bingLocaleSelectComboBox->addItem("ja-jp (Japanese)", "ja-jp");
    m_ui->bingLocaleSelectComboBox->addItem("ko-kr (Korean)", "ko-kr");
    m_ui->bingLocaleSelectComboBox->addItem("nl-be (Dutch-Belgium)", "nl-be");
    m_ui->bingLocaleSelectComboBox->addItem("nl-nl (Dutch_Netherlands)", "nl-nl");
    m_ui->bingLocaleSelectComboBox->addItem("no-no (Norwegian)", "no-no");
    m_ui->bingLocaleSelectComboBox->addItem("pl-pl (Polish)", "pl-pl");
    m_ui->bingLocaleSelectComboBox->addItem("pt-pt (Portuguese-Portugal)", "pt-pt");
    m_ui->bingLocaleSelectComboBox->addItem("pt-br (Portuguese-Brazil)", "pt-br");
    m_ui->bingLocaleSelectComboBox->addItem("ru-ru (Russian)", "ru-ru");
    m_ui->bingLocaleSelectComboBox->addItem("sv-se (Swedish)", "sv-se");
    m_ui->bingLocaleSelectComboBox->addItem("tr-tr (Turkish)", "tr-tr");
    m_ui->bingLocaleSelectComboBox->addItem("zh-cn (Chinese)", "zh-cn");
    m_ui->bingLocaleSelectComboBox->addItem("zh-hk (Traditional Chinese-Hong Kong SAR)", "zh-hk");
    m_ui->bingLocaleSelectComboBox->addItem("zh-tw (Traditional Chinese-Taiwan)", "zh-tw");

    // https://sites.google.com/site/tomihasa/google-language-codes
    m_ui->googleLanguageComboBox->addItem("Afrikaans", "af");
    m_ui->googleLanguageComboBox->addItem("Akan", "ak");
    m_ui->googleLanguageComboBox->addItem("Albanian", "sq");
    m_ui->googleLanguageComboBox->addItem("Amharic", "am");
    m_ui->googleLanguageComboBox->addItem("Arabic", "ar");
    m_ui->googleLanguageComboBox->addItem("Armenian", "hy");
    m_ui->googleLanguageComboBox->addItem("Azerbaijani", "az");
    m_ui->googleLanguageComboBox->addItem("Basque", "eu");
    m_ui->googleLanguageComboBox->addItem("Belarusian", "be");
    m_ui->googleLanguageComboBox->addItem("Bemba", "bem");
    m_ui->googleLanguageComboBox->addItem("Bengali", "bn");
    m_ui->googleLanguageComboBox->addItem("Bihari", "bh");
    m_ui->googleLanguageComboBox->addItem("Bork, bork, bork!", "xx-bork");
    m_ui->googleLanguageComboBox->addItem("Bosnian", "bs");
    m_ui->googleLanguageComboBox->addItem("Breton", "br");
    m_ui->googleLanguageComboBox->addItem("Bulgarian", "bg");
    m_ui->googleLanguageComboBox->addItem("Cambodian", "km");
    m_ui->googleLanguageComboBox->addItem("Catalan", "ca");
    m_ui->googleLanguageComboBox->addItem("Cherokee", "chr");
    m_ui->googleLanguageComboBox->addItem("Chichewa", "ny");
    m_ui->googleLanguageComboBox->addItem("Chinese (Simplified)", "zh-CN");
    m_ui->googleLanguageComboBox->addItem("Chinese (Traditional)", "zh-TW");
    m_ui->googleLanguageComboBox->addItem("Corsican", "co");
    m_ui->googleLanguageComboBox->addItem("Croatian", "hr");
    m_ui->googleLanguageComboBox->addItem("Czech", "cs");
    m_ui->googleLanguageComboBox->addItem("Danish", "da");
    m_ui->googleLanguageComboBox->addItem("Dutch", "nl");
    m_ui->googleLanguageComboBox->addItem("Elmer Fudd", "xx-elmer");
    m_ui->googleLanguageComboBox->addItem("English", "en");
    m_ui->googleLanguageComboBox->addItem("Esperanto", "eo");
    m_ui->googleLanguageComboBox->addItem("Estonian", "et");
    m_ui->googleLanguageComboBox->addItem("Ewe", "ee");
    m_ui->googleLanguageComboBox->addItem("Faroese", "fo");
    m_ui->googleLanguageComboBox->addItem("Filipino", "tl");
    m_ui->googleLanguageComboBox->addItem("Finnish", "fi");
    m_ui->googleLanguageComboBox->addItem("French", "fr");
    m_ui->googleLanguageComboBox->addItem("Frisian", "fy");
    m_ui->googleLanguageComboBox->addItem("Ga", "gaa");
    m_ui->googleLanguageComboBox->addItem("Galician", "gl");
    m_ui->googleLanguageComboBox->addItem("Georgian", "ka");
    m_ui->googleLanguageComboBox->addItem("German", "de");
    m_ui->googleLanguageComboBox->addItem("Greek", "el");
    m_ui->googleLanguageComboBox->addItem("Guarani", "gn");
    m_ui->googleLanguageComboBox->addItem("Gujarati", "gu");
    m_ui->googleLanguageComboBox->addItem("Hacker", "xx-hacker");
    m_ui->googleLanguageComboBox->addItem("Haitian Creole", "ht");
    m_ui->googleLanguageComboBox->addItem("Hausa", "ha");
    m_ui->googleLanguageComboBox->addItem("Hawaiian", "haw");
    m_ui->googleLanguageComboBox->addItem("Hebrew", "iw");
    m_ui->googleLanguageComboBox->addItem("Hindi", "hi");
    m_ui->googleLanguageComboBox->addItem("Hungarian", "hu");
    m_ui->googleLanguageComboBox->addItem("Icelandic", "is");
    m_ui->googleLanguageComboBox->addItem("Igbo", "ig");
    m_ui->googleLanguageComboBox->addItem("Indonesian", "id");
    m_ui->googleLanguageComboBox->addItem("Interlingua", "ia");
    m_ui->googleLanguageComboBox->addItem("Irish", "ga");
    m_ui->googleLanguageComboBox->addItem("Italian", "it");
    m_ui->googleLanguageComboBox->addItem("Japanese", "ja");
    m_ui->googleLanguageComboBox->addItem("Javanese", "jw");
    m_ui->googleLanguageComboBox->addItem("Kannada", "kn");
    m_ui->googleLanguageComboBox->addItem("Kazakh", "kk");
    m_ui->googleLanguageComboBox->addItem("Kinyarwanda", "rw");
    m_ui->googleLanguageComboBox->addItem("Kirundi", "rn");
    m_ui->googleLanguageComboBox->addItem("Klingon", "xx-klingon");
    m_ui->googleLanguageComboBox->addItem("Kongo", "kg");
    m_ui->googleLanguageComboBox->addItem("Korean", "ko");
    m_ui->googleLanguageComboBox->addItem("Krio (Sierra Leone)", "kri");
    m_ui->googleLanguageComboBox->addItem("Kurdish", "ku");
    m_ui->googleLanguageComboBox->addItem("Kurdish (Soranî)", "ckb");
    m_ui->googleLanguageComboBox->addItem("Kyrgyz", "ky");
    m_ui->googleLanguageComboBox->addItem("Laothian", "lo");
    m_ui->googleLanguageComboBox->addItem("Latin", "la");
    m_ui->googleLanguageComboBox->addItem("Latvian", "lv");
    m_ui->googleLanguageComboBox->addItem("Lingala", "ln");
    m_ui->googleLanguageComboBox->addItem("Lithuanian", "lt");
    m_ui->googleLanguageComboBox->addItem("Lozi", "loz");
    m_ui->googleLanguageComboBox->addItem("Luganda", "lg");
    m_ui->googleLanguageComboBox->addItem("Luo", "ach");
    m_ui->googleLanguageComboBox->addItem("Macedonian", "mk");
    m_ui->googleLanguageComboBox->addItem("Malagasy", "mg");
    m_ui->googleLanguageComboBox->addItem("Malay", "ms");
    m_ui->googleLanguageComboBox->addItem("Malayalam", "ml");
    m_ui->googleLanguageComboBox->addItem("Maltese", "mt");
    m_ui->googleLanguageComboBox->addItem("Maori", "mi");
    m_ui->googleLanguageComboBox->addItem("Marathi", "mr");
    m_ui->googleLanguageComboBox->addItem("Mauritian Creole", "mfe");
    m_ui->googleLanguageComboBox->addItem("Moldavian", "mo");
    m_ui->googleLanguageComboBox->addItem("Mongolian", "mn");
    m_ui->googleLanguageComboBox->addItem("Montenegrin", "sr-ME");
    m_ui->googleLanguageComboBox->addItem("Nepali", "ne");
    m_ui->googleLanguageComboBox->addItem("Nigerian Pidgin", "pcm");
    m_ui->googleLanguageComboBox->addItem("Northern Sotho", "nso");
    m_ui->googleLanguageComboBox->addItem("Norwegian", "no");
    m_ui->googleLanguageComboBox->addItem("Norwegian (Nynorsk)", "nn");
    m_ui->googleLanguageComboBox->addItem("Occitan", "oc");
    m_ui->googleLanguageComboBox->addItem("Oriya", "or");
    m_ui->googleLanguageComboBox->addItem("Oromo", "om");
    m_ui->googleLanguageComboBox->addItem("Pashto", "ps");
    m_ui->googleLanguageComboBox->addItem("Persian", "fa");
    m_ui->googleLanguageComboBox->addItem("Pirate", "xx-pirate");
    m_ui->googleLanguageComboBox->addItem("Polish", "pl");
    m_ui->googleLanguageComboBox->addItem("Portuguese (Brazil)", "pt-BR");
    m_ui->googleLanguageComboBox->addItem("Portuguese (Portugal)", "pt-PT");
    m_ui->googleLanguageComboBox->addItem("Punjabi", "pa");
    m_ui->googleLanguageComboBox->addItem("Quechua", "qu");
    m_ui->googleLanguageComboBox->addItem("Romanian", "ro");
    m_ui->googleLanguageComboBox->addItem("Romansh", "rm");
    m_ui->googleLanguageComboBox->addItem("Runyakitara", "nyn");
    m_ui->googleLanguageComboBox->addItem("Russian", "ru");
    m_ui->googleLanguageComboBox->addItem("Scots Gaelic", "gd");
    m_ui->googleLanguageComboBox->addItem("Serbian", "sr");
    m_ui->googleLanguageComboBox->addItem("Serbo-Croatian", "sh");
    m_ui->googleLanguageComboBox->addItem("Sesotho", "st");
    m_ui->googleLanguageComboBox->addItem("Setswana", "tn");
    m_ui->googleLanguageComboBox->addItem("Seychellois Creole", "crs");
    m_ui->googleLanguageComboBox->addItem("Shona", "sn");
    m_ui->googleLanguageComboBox->addItem("Sindhi", "sd");
    m_ui->googleLanguageComboBox->addItem("Sinhalese", "si");
    m_ui->googleLanguageComboBox->addItem("Slovak", "sk");
    m_ui->googleLanguageComboBox->addItem("Slovenian", "sl");
    m_ui->googleLanguageComboBox->addItem("Somali", "so");
    m_ui->googleLanguageComboBox->addItem("Spanish", "es");
    m_ui->googleLanguageComboBox->addItem("Spanish (Latin American)", "es-419");
    m_ui->googleLanguageComboBox->addItem("Sundanese", "su");
    m_ui->googleLanguageComboBox->addItem("Swahili", "sw");
    m_ui->googleLanguageComboBox->addItem("Swedish", "sv");
    m_ui->googleLanguageComboBox->addItem("Tajik", "tg");
    m_ui->googleLanguageComboBox->addItem("Tamil", "ta");
    m_ui->googleLanguageComboBox->addItem("Tatar", "tt");
    m_ui->googleLanguageComboBox->addItem("Telugu", "te");
    m_ui->googleLanguageComboBox->addItem("Thai", "th");
    m_ui->googleLanguageComboBox->addItem("Tigrinya", "ti");
    m_ui->googleLanguageComboBox->addItem("Tonga", "to");
    m_ui->googleLanguageComboBox->addItem("Tshiluba", "lua");
    m_ui->googleLanguageComboBox->addItem("Tumbuka", "tum");
    m_ui->googleLanguageComboBox->addItem("Turkish", "tr");
    m_ui->googleLanguageComboBox->addItem("Turkmen", "tk");
    m_ui->googleLanguageComboBox->addItem("Twi", "tw");
    m_ui->googleLanguageComboBox->addItem("Uighur", "ug");
    m_ui->googleLanguageComboBox->addItem("Ukrainian", "uk");
    m_ui->googleLanguageComboBox->addItem("Urdu", "ur");
    m_ui->googleLanguageComboBox->addItem("Uzbek", "uz");
    m_ui->googleLanguageComboBox->addItem("Vietnamese", "vi");
    m_ui->googleLanguageComboBox->addItem("Welsh", "cy");
    m_ui->googleLanguageComboBox->addItem("Wolof", "wo");
    m_ui->googleLanguageComboBox->addItem("Xhosa", "xh");
    m_ui->googleLanguageComboBox->addItem("Yiddish", "yi");
    m_ui->googleLanguageComboBox->addItem("Yoruba", "yo");
    m_ui->googleLanguageComboBox->addItem("Zulu", "zu");
}

void QuickWebShortcutsConfig::validateProxyOptions() {
    m_ui->proxyDataContainerWidget->setHidden(m_ui->noProxyRadioButton->isChecked());
}

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
    manager->get(request);
    connect(manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(showProxyConnectionValidationResults(QNetworkReply * )));
    m_ui->proxyTestResultLabel->setText("Making request...");
}

void QuickWebShortcutsConfig::showProxyConnectionValidationResults(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        m_ui->proxyTestResultLabel->setText("No Error!\nYour Ip address is: \n" + reply->readAll() + "\nThe request took:\n"
                                            + QString::number(timeBeforeRequest.msecsTo(QTime::currentTime())) + " ms");
    } else {
        m_ui->proxyTestResultLabel->setText(
                QString(QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(int(reply->error()))) + ":\n" +
                reply->errorString());
    }
}

void QuickWebShortcutsConfig::itemSelected() {
    auto *sourceItem = reinterpret_cast<SearchEngineItem *>(this->sender()->parent());
    const int itemCount = m_ui->searchEnginesItemLayout->count();
    int selected = 0;
    for (int i = 0; i < itemCount; ++i) {
        auto *item = reinterpret_cast<SearchEngineItem *>(m_ui->searchEnginesItemLayout->itemAt(i)->widget());
        if (item->useRadioButton->isChecked()) ++selected;
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

SearchEngineItem::SearchEngineItem(QWidget *parent, QWidget *parentModule) : QWidget(parent), parentModule(parentModule) {
    setupUi(this);
    connect(this->useRadioButton, SIGNAL(clicked(bool)), parentModule, SLOT(changed()));
    connect(this->useRadioButton, SIGNAL(clicked(bool)), parentModule, SLOT(itemSelected()));
    connect(this->urlLineEdit, SIGNAL(textChanged(QString)), parentModule, SLOT(changed()));
    connect(this->nameLineEdit, SIGNAL(textChanged(QString)), parentModule, SLOT(changed()));
    connect(this->iconPushButton, SIGNAL(clicked(bool)), parentModule, SLOT(changed()));
    connect(this->deletePushButton, SIGNAL(clicked(bool)), parentModule, SLOT(changed()));
    connect(this->urlLineEdit, SIGNAL(textChanged(QString)), this, SLOT(edited()));
    connect(this->nameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(edited()));
    connect(this->iconPushButton, SIGNAL(clicked(bool)), this, SLOT(edited()));
    connect(this->iconPushButton, SIGNAL(clicked(bool)), this, SLOT(edited()));
    connect(this->iconPushButton, SIGNAL(clicked(bool)), this, SLOT(iconPicker()));
    connect(this->deletePushButton, SIGNAL(clicked(bool)), parentModule, SLOT(deleteCurrentItem()));
    connect(this->urlLineEdit, SIGNAL(textChanged(QString)), this, SLOT(extractNameFromUrl()));
}

void SearchEngineItem::extractNameFromUrl() {
    if (!this->nameLineEdit->text().isEmpty()) return;
    if (this->urlLineEdit->text().contains(QRegExp(R"(^(?:https?://)?(www\.)?(?:[\w-]+\.)(?:\.?[\w]{2,})+)"))) {
        QRegExp exp(R"(^(?:https?://)(www\.)?([^/]+)\.(?:\.?[\w]{2,})+/?)");
        exp.indexIn(this->urlLineEdit->text());
        QString res = exp.capturedTexts().last();
        res[0] = res[0].toUpper();
        this->nameLineEdit->setText(res);
    }
}

void SearchEngineItem::iconPicker() {
    QString iconPath = QFileDialog::getOpenFileName(this, tr("Select Icon"), "",
                                                    tr("Images (.*.jpg *.jpeg *.png *.ico *.svg *.svgz)"));
    if (!iconPath.isEmpty()) {
        this->originalIcon = this->icon;
        this->icon = iconPath;
        this->iconPushButton->setIcon(QIcon(this->icon));
    }
    this->iconPushButton->clearFocus();

}

#include "quick_web_shortcuts_config.moc"
