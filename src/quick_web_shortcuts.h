#ifndef QUICKWEBSHORTCUTS_H
#define QUICKWEBSHORTCUTS_H

#include "searchengines/SearchEngine.h"
#include <KConfigGroup>
#include <KRunner/AbstractRunner>
#include <KSharedConfig>
#include <QFileSystemWatcher>
#include <QRegularExpression>
#include <QtNetwork/QNetworkAccessManager>
#include <searchproviders/RequiredData.h>

#if KRUNNER_VERSION_MAJOR == 5
#include <QAction>
#else
#include <KRunner/Action>
#endif

class QuickWebShortcuts : public KRunner::AbstractRunner
{
    Q_OBJECT

private:
    // General variables
    QFileSystemWatcher watcher;
    SearchEngine currentSearchEngine;
#if KRUNNER_VERSION_MAJOR == 5
    QList<QAction *> normalActions, privateActions;
#else
    KRunner::Actions normalActions, privateActions;
#endif
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
    QuickWebShortcuts(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
    ~QuickWebShortcuts() override;

private:
    KRunner::QueryMatch createMatch(const QString &text, const QMap<QString, QVariant> &data);
    void searchSuggest(KRunner::RunnerContext &context, const QString &term, const QString &browser = "");

protected Q_SLOTS:
    void init() override;
    void filterHistory();
    void reloadPluginConfiguration(const QString &file = QString());
    void match(KRunner::RunnerContext &context) override;
    void run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) override;
};

#endif
