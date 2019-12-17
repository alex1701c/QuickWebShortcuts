#ifndef QUICKWEBSHORTCUTS_SEARCHENGINEITEM_H
#define QUICKWEBSHORTCUTS_SEARCHENGINEITEM_H

#include "ui_browser_item.h"

#include <QtCore>
#include <QWidget>
#include <KConfigWidgets/KCModule>

class SearchEngineItem : public QWidget, public Ui::SearchEngineItemUi {
Q_OBJECT

public:
    QString originalName, originalURL, originalIcon, icon;
    bool isDefault = false;
    bool isEdited = false;
    bool isDefaultBased = false;

    explicit SearchEngineItem(QWidget *parent);

public Q_SLOTS:

    void extractNameFromUrl();

    void edited() { this->isEdited = true; }

    void iconPicker();

Q_SIGNALS:

    void changed();

    void itemSelected();

    void deleteCurrentItem();
};

#endif //QUICKWEBSHORTCUTS_SEARCHENGINEITEM_H
