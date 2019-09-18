#ifndef QUICKWEBSHORTCUTS_BING_H
#define QUICKWEBSHORTCUTS_BING_H


#include <QtNetwork/QNetworkReply>
#include <KRunner/AbstractRunner>
#include <QtCore>
#include <utility>

class Bing : public QObject {

Q_OBJECT

public:
    Bing(Plasma::AbstractRunner *mRunner, Plasma::RunnerContext &mContext,
         QString mSearchEngine, QString mQuery, QIcon mIcon, const int mMaxResults = 10, QString mMarket = "en-us")
            : m_runner(mRunner), m_context(mContext), m_searchEngine(std::move(mSearchEngine)),
              m_query(std::move(mQuery)), m_icon(std::move(mIcon)), m_maxResults(mMaxResults), m_market(std::move(mMarket)) {

        if (m_query.isEmpty()) {
            emit finished();
            return;
        }

        m_manager = new QNetworkAccessManager(this);
        QUrlQuery queryParameters;
        queryParameters.addQueryItem("query", m_query);
        queryParameters.addQueryItem("market", m_market);

        QNetworkRequest request(QUrl("https://api.bing.com/osjson.aspx?" +
                                     QUrl(queryParameters.query(QUrl::FullyEncoded).toUtf8()).toEncoded()));
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        m_manager->get(request);
        connect(m_manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(parseResponse(QNetworkReply * )));

    }

private Q_SLOTS:

    void parseResponse(QNetworkReply *reply) {
        const auto suggestionsObject = QJsonDocument::fromJson(reply->readAll());
        if (suggestionsObject.isArray()) {
            const auto rootArray = suggestionsObject.array();
            if (rootArray.at(1).isArray()) {
                const auto suggestionsArray = rootArray.at(1).toArray();
                const int suggestionCount = suggestionsArray.count();
                for (int i = 0; i < suggestionCount && i < m_maxResults; ++i) {
                    const QString suggestion = suggestionsArray.at(i).toString();
                    Plasma::QueryMatch match(m_runner);
                    match.setIcon(m_icon);
                    match.setText("Search for: " + suggestion);
                    match.setData(m_searchEngine + QUrl::toPercentEncoding(suggestion));
                    m_context.addMatch(match);
                }
            }
        }
        emit finished();
    }

Q_SIGNALS:

    void finished();

private:
    Plasma::AbstractRunner *m_runner;
    QNetworkAccessManager *m_manager;
    Plasma::RunnerContext m_context;
    const QString m_searchEngine;
    const QString m_query;
    const QIcon m_icon;
    const int m_maxResults = 10;
    const QString m_market = "en-us";
};

#endif
