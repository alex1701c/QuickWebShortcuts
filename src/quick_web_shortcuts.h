#ifndef QUICKWEBSHORTCUTS_H
#define QUICKWEBSHORTCUTS_H

#include <KRunner/AbstractRunner>
#include <searchproviders/RequiredData.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QFileSystemWatcher>
#include <KSharedConfig>
#include "searchengines/SearchEngine.h"

class QuickWebShortcuts : public Plasma::AbstractRunner {
Q_OBJECT

private:
    // General variables
    QFileSystemWatcher watcher;
    SearchEngine currentSearchEngine;

    QRegExp shortUrlRegex;
    QRegExp urlRegex;
    QRegExp removeHistoryRegex;

    QString privateBrowser, privateBrowserMode, searchOptionTemplate, searchSuggestionChoice, triggerCharacter, privateWindowTrigger;
    bool openUrls;
    KConfigGroup generalKrunnerConfig;

    // Search suggestion variables
    bool searchSuggestions, privateWindowSearchSuggestions;
    int minimumLetterCount;
    QString bingMarket, googleLocale;

    // Fallback Icon
    const QIcon globeIcon = QIcon::fromTheme(QStringLiteral("globe"));

    // History cleaning config variables
    bool cleanAll, cleanQuick, cleanNone;
    bool wasActive = false;

    RequiredData requiredData;

public:
    QuickWebShortcuts(QObject *parent, const QVariantList &args);

    ~QuickWebShortcuts() override;

private:
    Plasma::QueryMatch createMatch(const QString &text, const QMap<QString, QVariant> &data, bool useGlobe = false);

    void searchSuggest(Plasma::RunnerContext &context, const QString &term, const QString &browser = "");

protected Q_SLOTS:

    void init() override;

    void matchSessionFinished();

    void reloadPluginConfiguration(const QString &file = "");

    void match(Plasma::RunnerContext &context) override;

    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) override;
};

#endif
