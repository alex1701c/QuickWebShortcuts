#include <QtWidgets/QFileDialog>
#include "SearchEngineItem.h"

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
        QString res = exp.capturedTexts().at(2);
        res[0] = res[0].toUpper();
        if (!res.isEmpty() && res != "Www") this->nameLineEdit->setText(res);
    }
}

void SearchEngineItem::iconPicker() {
    const QString iconPath = QFileDialog::getOpenFileName(this, tr("Select Icon"), "",
                                                          tr("Images (.*.jpg *.jpeg *.png *.ico *.svg *.svgz)"));
    if (!iconPath.isEmpty()) {
        this->originalIcon = this->icon;
        this->icon = iconPath;
        this->iconPushButton->setIcon(QIcon(this->icon));
    }
    this->iconPushButton->clearFocus();

}
