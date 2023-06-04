#ifndef QUICKWEBSHORTCUTS_H
#define QUICKWEBSHORTCUTS_H

#include <KRunner/AbstractRunner>
#include <searchproviders/RequiredData.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QFileSystemWatcher>
#include <KSharedConfig>
#include <KConfigGroup>
#include <QRegularExpression>
#include "searchengines/SearchEngine.h"

class QuickWebShortcuts : public KRunner::AbstractRunner {
Q_OBJECT

private:
    // General variables
    QFileSystemWatcher watcher;
    SearchEngine currentSearchEngine;
    QList<QAction *> normalActions, privateActions;
    bool isWebShortcut;

    QRegularExpression shortUrlRegex;
    QRegularExpression urlRegex;
    QRegularExpression removeHistoryRegex;

    QString privateBrowser, privateBrowserMode, searchOptionTemplate, searchSuggestionChoice, triggerCharacter, privateWindowTrigger;
    KConfigGroup generalKrunnerConfig;

    // Search suggestion variables
    bool searchSuggestions, privateWindowSearchSuggestions;
    int minimumLetterCount;
    QString bingMarket, googleLocale;

    // Fallback Icon
    const QIcon globeIcon = QIcon::fromTheme(QStringLiteral("globe"));

    // History cleaning config variables
    bool cleanAll, cleanQuick, cleanNone;
    bool wasActive = true;

    RequiredData requiredData;

public:
    QuickWebShortcuts(QObject *parent, const KPluginMetaData &pluginMetaData, const QVariantList &args);
    ~QuickWebShortcuts() override;

private:
    KRunner::QueryMatch createMatch(const QString &text, const QMap<QString, QVariant> &data, bool useGlobe = false);
    void searchSuggest(KRunner::RunnerContext &context, const QString &term, const QString &browser = "");

protected Q_SLOTS:
    void init() override;
    void filterHistory();
    void reloadPluginConfiguration(const QString &file = QString());
    void match(KRunner::RunnerContext &context) override;
    QList<QAction *> actionsForMatch(const KRunner::QueryMatch &match) override;
    void run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) override;
};

#endif
