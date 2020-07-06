#include <QtWidgets/QFileDialog>
#include <QCheckBox>
#include "SearchEngineItem.h"

SearchEngineItem::SearchEngineItem(QWidget *parent) : QWidget(parent) {
    setupUi(this);
    connect(this->useRadioButton, &QRadioButton::clicked, this, &SearchEngineItem::changed);
    connect(this->useRadioButton, &QRadioButton::clicked, this, &SearchEngineItem::itemSelected);
    connect(this->urlLineEdit, &QLineEdit::textChanged, this, &SearchEngineItem::changed);
    connect(this->nameLineEdit, &QLineEdit::textChanged, this, &SearchEngineItem::changed);
    connect(this->iconPushButton, &QCheckBox::clicked, this, &SearchEngineItem::changed);
    connect(this->deletePushButton, &QCheckBox::clicked, this, &SearchEngineItem::changed);
    connect(this->urlLineEdit, &QLineEdit::textChanged, this, &SearchEngineItem::edited);
    connect(this->nameLineEdit, &QLineEdit::textChanged, this, &SearchEngineItem::edited);
    connect(this->iconPushButton, &QCheckBox::clicked, this, &SearchEngineItem::edited);
    connect(this->iconPushButton, &QCheckBox::clicked, this, &SearchEngineItem::edited);
    connect(this->iconPushButton, &QCheckBox::clicked, this, &SearchEngineItem::iconPicker);
    connect(this->deletePushButton, &QCheckBox::clicked, this, &SearchEngineItem::deleteCurrentItem);
    connect(this->urlLineEdit, &QLineEdit::textChanged, this, &SearchEngineItem::extractNameFromUrl);
}

void SearchEngineItem::extractNameFromUrl() {
    if (!this->nameLineEdit->text().isEmpty()) return;
    QRegularExpression exp(QStringLiteral(R"(^(?:https?://)(www\.)?([^/]+)\.(?:\.?[\w]{2,})+/?)"));
    if (this->urlLineEdit->text().contains(exp)) {
        const auto regexMatch = exp.match(this->urlLineEdit->text());
        QString res = regexMatch.captured(2);
        res[0] = res[0].toUpper();
        if (!res.isEmpty() && res != QLatin1String("Www")) {
            this->nameLineEdit->setText(res);
        }
    }
}

void SearchEngineItem::iconPicker() {
    const QString iconPath = QFileDialog::getOpenFileName(this, tr("Select Icon"), QString(),
                                                          tr("Images (.*.jpg *.jpeg *.png *.ico *.svg *.svgz)"));
    if (!iconPath.isEmpty()) {
        this->originalIcon = this->icon;
        this->icon = iconPath;
        this->iconPushButton->setIcon(QIcon(this->icon));
    }
    this->iconPushButton->clearFocus();
}
