#ifndef QUICKWEBSHORTCUTS_DUCKDUCKGO_H
#define QUICKWEBSHORTCUTS_DUCKDUCKGO_H


#include <QtNetwork>
#include <QtCore>
#include <utility>
#include <QTextDocument>
#include "RequiredData.h"

class DuckDuckGo : public QObject {

Q_OBJECT

private:
    QNetworkAccessManager *m_manager;
    Plasma::RunnerContext m_context;
    const QString query;
    const QString browserLaunchCommand;
    RequiredData data;

public:
    DuckDuckGo(Plasma::RunnerContext &mContext, QString query, RequiredData data,
               QString browserLaunchCommand = "") : m_context(mContext), query(std::move(query)),
                                                    browserLaunchCommand(std::move(browserLaunchCommand)), data(std::move(data)) {
        m_manager = new QNetworkAccessManager(this);
        if (data.proxy != nullptr) m_manager->setProxy(*this->data.proxy);

        QUrlQuery queryParameters;
        queryParameters.addQueryItem("q", this->query);

        QNetworkRequest request(QUrl("https://duckduckgo.com/lite/?" +
                                     QUrl(queryParameters.query(QUrl::FullyEncoded).toUtf8()).toEncoded()));
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader,
                          "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:69.0) Gecko/20100101 Firefox/69.0");

        auto initialReply = m_manager->get(request);

        connect(m_manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(parseResponse(QNetworkReply * )));
        QTimer::singleShot(2000, initialReply, [initialReply]() {
            initialReply->abort();
        });
    }


public Q_SLOTS:

    void parseResponse(QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::OperationCanceledError) {
            emit finished();
            delete reply;
            return;
        }
        if (reply->error() != QNetworkReply::NoError) {
            if (data.showNetworkErrors) {
                QProcess::startDetached("notify-send", QStringList(
                        {"-i", "globe", "QuickWebShortcuts ",
                         QString(QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(int(reply->error()))) + ":\n" +
                         reply->errorString()}));
            }
        } else if (m_context.isValid()) {
            // Parse html content
            QList<QStringList> urlList;
            QRegularExpression linkRegex("<a rel=\"nofollow\" href=\"([^\"]+)\" class='result-link'>(.*)</a>");
            QRegularExpressionMatchIterator it = linkRegex.globalMatch(reply->readAll());
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                if (match.hasMatch()) {
                    urlList.append(match.capturedTexts());
                }
            }
            // Create matches
            const int listCount = urlList.count();
            for (int i = 0; i < listCount && i < data.maxResults; ++i) {
                const QStringList &currentList = urlList.at(i);
                Plasma::QueryMatch match(data.runner);
                match.setIcon(data.icon);
                match.setText(QString(currentList.at(1)).remove("http://").remove("https://").remove("www."));
                match.setRelevance((float) (19 - i) / 20);

                QMap<QString, QVariant> runData;
                runData.insert("url", currentList.at(1));
                if (!browserLaunchCommand.isEmpty()) runData.insert("browser", browserLaunchCommand);
                match.setData(runData);
                m_context.addMatch(match);
            }
        }

        delete reply;
        emit finished();
    }

Q_SIGNALS:

    void finished();
};

#endif //QUICKWEBSHORTCUTS_DUCKDUCKGO_H
