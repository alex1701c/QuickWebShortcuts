#ifndef QUICKWEBSHORTCUTS_BING_H
#define QUICKWEBSHORTCUTS_BING_H


#include <QTimer>
#include <QMetaEnum>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <KRunner/AbstractRunner>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <KNotifications/KNotification>
#include <utility>
#include "RequiredData.h"

//#define TEST_PROXY

class Bing: public QObject {

Q_OBJECT

private:
    QNetworkAccessManager *manager;
    KRunner::RunnerContext context;
    const QString query;
    const QString market;
    const QString browserLaunchCommand;
    RequiredData data;
    QNetworkReply *reply;

public:
    Bing(KRunner::RunnerContext &context, QString query, RequiredData &data, QString market = "en-us",
         QString browserLaunchCommand = "")
        : context(context), query(std::move(query)), market(std::move(market)),
          browserLaunchCommand(std::move(browserLaunchCommand)), data(data) {

        manager = new QNetworkAccessManager(this);
        if (data.proxy) {
            manager->setProxy(*this->data.proxy);
        }
#ifdef  TEST_PROXY
        QNetworkRequest request(QUrl("https://ifconfig.me/ip"));
        m_manager->get(request);
        connect(manager, &QNetworkAccessManager::finished, this, &Bing::parseResponse);
#else
        QUrlQuery queryParameters;
        queryParameters.addQueryItem(QStringLiteral("query"), this->query);
        queryParameters.addQueryItem(QStringLiteral("market"), this->market);

        QNetworkRequest request(QUrl(QStringLiteral("https://api.bing.com/osjson.aspx?") +
            QUrl(queryParameters.query(QUrl::FullyEncoded).toUtf8()).toEncoded()));
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));

        reply = manager->get(request);

        connect(manager, &QNetworkAccessManager::finished, this, &Bing::parseResponse);

        QTimer::singleShot(2000, this, [this]() {
            reply->abort();
        });
#endif
    }
    virtual ~Bing() {
        delete reply;
    }

public Q_SLOTS:

    void parseResponse() {
#ifdef TEST_PROXY
        qInfo() << reply->readAll();
        Q_EMIT finished();
        return;
#endif
        if (reply->error() == QNetworkReply::OperationCanceledError) {
            Q_EMIT finished();
            return;
        }
        if (reply->error() != QNetworkReply::NoError) {
            if (data.showNetworkErrors) {
                KNotification::event(KNotification::Error,
                                     QStringLiteral("Krunner-QuickWebShortcuts"),
                                     QString(QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(int(reply->error()))) +
                                     QStringLiteral(":\n") +
                                     reply->errorString(), QStringLiteral("globe"));
            }
        } else if (context.isValid()) {
            const auto suggestionsObject = QJsonDocument::fromJson(reply->readAll());
            if (suggestionsObject.isArray()) {
                const auto rootArray = suggestionsObject.array();
                if (rootArray.at(1).isArray()) {
                    const auto suggestionsArray = rootArray.at(1).toArray();
                    const int suggestionCount = suggestionsArray.count();
                    for (int i = 0; i < suggestionCount && i < data.maxResults; ++i) {
                        const QString suggestion = suggestionsArray.at(i).toString();
                        if (suggestion == query) {
                            ++data.maxResults;
                            continue;
                        }
                        KRunner::QueryMatch match(data.runner);
                        match.setIcon(data.icon);
                        match.setText(data.searchOptionTemplate.arg(suggestion));
                        match.setRelevance((float) (19 - i) / 20);

                        QMap<QString, QVariant> runData;
                        QString url;
                        if (data.isWebShortcut) {
                            url = QString(data.searchEngine).replace(QStringLiteral("\\{@}"), QUrl::toPercentEncoding(suggestion));
                        } else {
                            url = data.searchEngine + QUrl::toPercentEncoding(suggestion);
                        }
                        runData.insert(QStringLiteral("url"), url);
                        if (!browserLaunchCommand.isEmpty()) { runData.insert(QStringLiteral("browser"), browserLaunchCommand); }
                        match.setData(runData);
                        context.addMatch(match);
                    }
                }
            }
        }
        Q_EMIT finished(); // NOLINT(readability-misleading-indentation)
    }

Q_SIGNALS:

    void finished();
};

#endif
