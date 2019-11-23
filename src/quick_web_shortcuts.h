#ifndef QUICKWEBSHORTCUTS_H
#define QUICKWEBSHORTCUTS_H

#include <KRunner/AbstractRunner>
#include <searchproviders/RequiredData.h>
#include <QtNetwork/QNetworkAccessManager>
#include "SearchEngine.h"

class QuickWebShortcuts : public Plasma::AbstractRunner {
Q_OBJECT

public:
    QuickWebShortcuts(QObject *parent, const QVariantList &args);

    void reloadConfiguration() override;

    // General variables
    KConfigGroup configGroup;
    SearchEngine currentSearchEngine;
    const QRegExp urlRegex = QRegExp(R"(^.*\.[a-z]{2,5}$)");
    QString privateBrowser, privateBrowserMode, searchOptionTemplate, searchSuggestionChoice, triggerCharacter, privateWindowTrigger;
    bool openUrls;
    // Search suggestion variables
    bool searchSuggestions, privateWindowSearchSuggestions;
    int minimumLetterCount, maxSuggestionResults;
    QString bingMarket, googleLocale;
    // Fallback Icon
    const QIcon globeIcon = QIcon::fromTheme("globe");
    // History cleaning config variables
    bool cleanAll, cleanQuick, cleanNone;
    bool wasActive = false;

    RequiredData requiredData;

    Plasma::QueryMatch createMatch(const QString &text, const QMap<QString, QVariant> &data, bool useGlobe = false);

    void bingSearchSuggest(Plasma::RunnerContext &context, const QString &term, const QString &browser = "");

    void googleSearchSuggest(Plasma::RunnerContext &context, const QString &term, const QString &browser = "");

    void duckDuckGoSearchSuggest(Plasma::RunnerContext &context, const QString &term, const QString &browser = "");

protected Q_SLOTS:

    void init() override;

    void matchSessionFinished();

public: // Plasma::AbstractRunner API
    void match(Plasma::RunnerContext &context) override;

    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) override;
};

#endif
