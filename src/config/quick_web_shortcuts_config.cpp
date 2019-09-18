#include "quick_web_shortcuts_config.h"
#include "../SearchEngines.h"
#include <KSharedConfig>
#include <KPluginFactory>
#include <QtDebug>

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


    // Search Engine GroupBox
    connect(m_ui->searchEngineURL, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->searchEngines, SIGNAL(currentTextChanged(QString)), this, SLOT(comboBoxEditTextChanged()));
    connect(m_ui->searchEngineName, SIGNAL(textChanged(QString)), this, SLOT(changed()));
    connect(m_ui->searchEngines, SIGNAL(currentTextChanged(QString)), this, SLOT(changed()));
    connect(m_ui->historyAll, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->historyQuick, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->historyNotClear, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->showSearchEngineName, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->openURLS, SIGNAL(clicked(bool)), this, SLOT(changed()));
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
    connect(m_ui->bingLocaleSelectComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));
    // Clear History GroupBox
    connect(m_ui->searchEngineURL, SIGNAL(textChanged(QString)), this, SLOT(extractNameFromURL()));
    connect(m_ui->deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteCurrentItem()));
    connect(m_ui->searchEnginesEditable, SIGNAL(clicked(bool)), this, SLOT(enableEditingOfExisting()));
    connect(m_ui->addSearchEngine, SIGNAL(clicked(bool)), this, SLOT(addSearchEngine()));
}

void QuickWebShortcutsConfig::load() {
    // Load search engines
    for (const auto &item : SearchEngines::getDefaultSearchEngines().toStdMap()) {
        m_ui->searchEngines->addItem(item.first, item.second);
        if (icons.contains(item.first)) {
            m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, icons.value(item.first));
        } else {
            m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, icons.value("globe"));
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
                m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, icons.value(item.first));
            } else {
                m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, icons.value("globe"));
            }
        }
    }
    QString url = config.readEntry("url");
    if (url.isEmpty()) url = "https://www.google.com/search?q=";
    int current = m_ui->searchEngines->findData(url);
    m_ui->searchEngines->setCurrentIndex(current);
    m_ui->searchEngines->setFocus();
    m_ui->deleteButton->setVisible(false);
    m_ui->showSearchEngineName->setChecked(config.readEntry("show_name", "false") == "true");
    m_ui->openURLS->setChecked(config.readEntry("open_urls", "true") == "true");

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
    insertLocaleSelectData();
    m_ui->bingLocaleSelectComboBox->setCurrentIndex(m_ui->bingLocaleSelectComboBox->findData(
            config.readEntry("bing_locale", "en-us")));
    m_ui->bingLocaleSelectComboBox->setHidden(searchSuggestionOption != "bing");
    validateSearchSuggestions();

    // Clear History settings
    QString historyOption = config.readEntry("clean_history", "all");
    if (historyOption == "all") {
        m_ui->historyAll->setChecked(true);
    } else if (historyOption == "quick") {
        m_ui->historyQuick->setChecked(true);
    } else {
        m_ui->historyNotClear->setChecked(true);
    }

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
    // Save search engines
    // TODO Clean up this mess :-)
    const int searchEnginesCount = m_ui->searchEngines->count();
    for (int i = 0; i < searchEnginesCount; ++i) {
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
    config.writeEntry("bing_locale", m_ui->bingLocaleSelectComboBox->itemData(
            m_ui->bingLocaleSelectComboBox->currentIndex()));

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
    config.writeEntry("open_urls", m_ui->openURLS->isChecked() ? "true" : "false");
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
        m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, icons.value(item.first));
    }
    for (const auto &item : SearchEngines::getCustomSearchEngines().toStdMap()) {
        m_ui->searchEngines->addItem(item.first, item.second);
        m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, icons.value("globe"));
    }

    m_ui->openURLS->setChecked(true);
    m_ui->showSearchEngineName->setChecked(false);
    m_ui->deleteButton->setVisible(false);
    m_ui->searchEnginesEditable->setChecked(false);
    m_ui->searchEngines->setEditable(false);
    m_ui->searchEngines->setCurrentIndex(m_ui->searchEngines->findData("https://www.google.com/search?q="));
    m_ui->privateWindowCheckBox->setChecked(false);
    m_ui->disableRadioButton->setChecked(true);
    m_ui->minimumLetterCountSpinBox->setValue(3);
    m_ui->bingLocaleSelectComboBox->setCurrentIndex(m_ui->bingLocaleSelectComboBox->findData("en-us"));

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
    m_ui->searchEngines->setItemIcon(m_ui->searchEngines->count() - 1, icons.value("globe"));
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

void QuickWebShortcutsConfig::validateSearchSuggestions() {
    const bool disabled = m_ui->disableRadioButton->isChecked();
    m_ui->privateWindowCheckBox->setDisabled(disabled);
    m_ui->minimumLetterCountSpinBox->setDisabled(disabled);
    if (disabled) m_ui->privateWindowCheckBox->setChecked(false);
    m_ui->bingLocaleSelectComboBox->setHidden(!m_ui->bingRadioButton->isChecked());
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
}

#include "quick_web_shortcuts_config.moc"
