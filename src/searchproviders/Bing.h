#ifndef QUICKWEBSHORTCUTS_BING_H
#define QUICKWEBSHORTCUTS_BING_H


#include <QtNetwork/QNetworkReply>
#include <KRunner/AbstractRunner>
#include <QtCore>
#include <utility>
#include "RequiredData.h"

//#define TEST_PROXY

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
        if (data.proxy != nullptr) m_manager->setProxy(*this->data.proxy);
#ifdef  TEST_PROXY
        QNetworkRequest request(QUrl("https://ifconfig.me/ip"));
        m_manager->get(request);
        connect(m_manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(parseResponse(QNetworkReply * )));
#else
        QUrlQuery queryParameters;
        queryParameters.addQueryItem("query", this->query);
        queryParameters.addQueryItem("market", this->market);

        QNetworkRequest request(QUrl("https://api.bing.com/osjson.aspx?" +
                                     QUrl(queryParameters.query(QUrl::FullyEncoded).toUtf8()).toEncoded()));
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        m_manager->get(request);

        connect(m_manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(parseResponse(QNetworkReply * )));
#endif
    }

public Q_SLOTS:

    void parseResponse(QNetworkReply *reply) {
#ifdef TEST_PROXY
        qInfo() << reply->readAll();
        emit finished();
        return;
#endif
        if (reply->error() != QNetworkReply::NoError) {
            if (data.showNetworkErrors) {
                QProcess::startDetached("notify-send", QStringList(
                        {"-i", "globe", "QuickWebShortcuts ",
                         QString(QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(int(reply->error()))) + ":\n" +
                         reply->errorString()}));
            }
        } else if (m_context.isValid()) {
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
                        match.setText(data.searchOptionTemplate.arg(suggestion));
                        match.setRelevance((float) (19 - i) / 20);

                        QMap<QString, QVariant> runData;
                        runData.insert("url", data.searchEngine + QUrl::toPercentEncoding(suggestion));
                        if (!browserLaunchCommand.isEmpty()) runData.insert("browser", browserLaunchCommand);
                        match.setData(runData);
                        m_context.addMatch(match);
                    }
                }
            }
        }
        emit finished();
    }

Q_SIGNALS:

    void finished();
};

#endif
