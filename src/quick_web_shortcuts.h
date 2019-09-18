#ifndef QUICKWEBSHORTCUTS_H
#define QUICKWEBSHORTCUTS_H

#include <KRunner/AbstractRunner>
#include <searchproviders/SearchSuggestionData.h>

class QuickWebShortcuts : public Plasma::AbstractRunner {
Q_OBJECT

public:
    QuickWebShortcuts(QObject *parent, const QVariantList &args);

    void reloadConfiguration() override;

    KConfigGroup configGroup;
    bool openUrls, searchSuggestions, privateWindowSearchSuggestions;
    int minimumLetterCount, maxSuggestionResults;
    QString privateBrowser, privateBrowserMode, searchEngine, searchEngineBaseUrl, searchEngineDisplayName, searchSuggestionChoice;
    QString bingMarket;
    QIcon currentIcon;
    QMap<QString, QIcon> icons;

    QMap<QString, QString> searchEngines;

    Plasma::QueryMatch createMatch(const QString &text, const QMap<QString, QVariant> &data, const QString &icon = "");

protected Q_SLOTS:

    void init() override;

    void matchSessionFinished();

public: // Plasma::AbstractRunner API
    void match(Plasma::RunnerContext &context) override;

    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) override;
};

#endif
