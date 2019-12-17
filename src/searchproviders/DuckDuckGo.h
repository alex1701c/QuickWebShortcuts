#ifndef QUICKWEBSHORTCUTS_DUCKDUCKGO_H
#define QUICKWEBSHORTCUTS_DUCKDUCKGO_H


#include <QtNetwork>
#include <QtCore>
#include <utility>
#include <QTextDocument>
#include <KNotifications/KNotification>
#include "RequiredData.h"

class DuckDuckGo : public QObject {

Q_OBJECT

private:
    QNetworkAccessManager *manager;
    Plasma::RunnerContext context;
    const QString query;
    const QString browserLaunchCommand;
    RequiredData data;

public:
    DuckDuckGo(Plasma::RunnerContext &context, QString query, RequiredData data,
               QString browserLaunchCommand = "") : context(context), query(std::move(query)),
                                                    browserLaunchCommand(std::move(browserLaunchCommand)), data(std::move(data)) {
        manager = new QNetworkAccessManager(this);
        if (data.proxy != nullptr) {
            manager->setProxy(*this->data.proxy);
        }

        QUrlQuery queryParameters;
        queryParameters.addQueryItem("q", this->query);

        QNetworkRequest request(QUrl("https://duckduckgo.com/lite/?" +
                                     QUrl(queryParameters.query(QUrl::FullyEncoded).toUtf8()).toEncoded()));
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader,
                          "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:69.0) Gecko/20100101 Firefox/69.0");

        const auto initialReply = manager->get(request);

        connect(manager, &QNetworkAccessManager::finished, this, &DuckDuckGo::parseResponse);
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
                KNotification::event(KNotification::Error,
                                     "Krunner-QuickWebShortcuts",
                                     QString(QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(int(reply->error()))) +
                                     ":\n" +
                                     reply->errorString(), "globe");
            }
        } else if (context.isValid()) {
            // Parse html content
            QList<QStringList> urlList;
            QRegularExpression linkRegex("<a rel=\"nofollow\" href=\"([^\"]+)\" class='result-link'>(.*)</a>");
            QRegularExpressionMatchIterator it = linkRegex.globalMatch(reply->readAll());
            while (it.hasNext() && urlList.size() < data.maxResults) {
                const QRegularExpressionMatch match = it.next();
                if (match.hasMatch()) {
                    urlList.append(match.capturedTexts());
                }
            }
            // Create matches
            const int listCount = urlList.count();
            for (int i = 0; i < listCount; ++i) {
                const QStringList &currentList = urlList.at(i);
                Plasma::QueryMatch match(data.runner);
                match.setIcon(data.icon);
                match.setText(QString(currentList.at(1)).remove("http://").remove("https://").remove("www."));
                match.setRelevance((float) (19 - i) / 20);

                QMap<QString, QVariant> runData;
                runData.insert("url", currentList.at(1));
                if (!browserLaunchCommand.isEmpty()) runData.insert("browser", browserLaunchCommand);
                match.setData(runData);
                context.addMatch(match);
            }
        }

        delete reply;
        emit finished();
    }

Q_SIGNALS:

    void finished();
};

#endif //QUICKWEBSHORTCUTS_DUCKDUCKGO_H
