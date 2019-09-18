#ifndef QUICKWEBSHORTCUTS_BING_H
#define QUICKWEBSHORTCUTS_BING_H


#include <QtNetwork/QNetworkReply>
#include <KRunner/AbstractRunner>
#include <QtCore>
#include <utility>
#include "RequiredData.h"

class Bing : public QObject {

Q_OBJECT

private:
    QNetworkAccessManager *m_manager;
    Plasma::RunnerContext m_context;
    const QString query;
    const QString market = "en-us";
    const QString browserLaunchCommand;
    RequiredData data;

public:
    Bing(Plasma::RunnerContext &mContext, QString mQuery, RequiredData &data, QString mMarket = "en-us",
         QString browserLaunchCommand = "") : m_context(mContext), query(std::move(mQuery)), market(std::move(mMarket)),
                                              browserLaunchCommand(std::move(browserLaunchCommand)), data(data) {

        m_manager = new QNetworkAccessManager(this);
        QUrlQuery queryParameters;
        queryParameters.addQueryItem("query", query);
        queryParameters.addQueryItem("market", market);

        QNetworkRequest request(QUrl("https://api.bing.com/osjson.aspx?" +
                                     QUrl(queryParameters.query(QUrl::FullyEncoded).toUtf8()).toEncoded()));
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        m_manager->get(request);

        connect(m_manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(parseResponse(QNetworkReply * )));

    }

public Q_SLOTS:

    void parseResponse(QNetworkReply *reply) {
        if (!m_context.isValid()) {
            emit finished();
            return;
        }
        qInfo() << "recieved response for: " << query;
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
                    Plasma::QueryMatch match(data.runner);
                    match.setIcon(data.icon);
                    match.setText("Search for " + suggestion);

                    QMap<QString, QVariant> runData;
                    runData.insert("url", data.searchEngine + QUrl::toPercentEncoding(suggestion));
                    if (!browserLaunchCommand.isEmpty()) runData.insert("browser", browserLaunchCommand);
                    match.setData(runData);
                    m_context.addMatch(match);
                }
            }
        }
        emit finished();
    }

Q_SIGNALS:

    void finished();
};

#endif
