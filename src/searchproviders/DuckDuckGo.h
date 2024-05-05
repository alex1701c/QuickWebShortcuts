#ifndef QUICKWEBSHORTCUTS_DUCKDUCKGO_H
#define QUICKWEBSHORTCUTS_DUCKDUCKGO_H


#include <utility>
#include <QTextDocument>
#include <QNetworkAccessManager>
#include <QMetaEnum>
#include <KNotifications/KNotification>
#include "RequiredData.h"
#include <KRunner/RunnerContext>

class DuckDuckGo : public QObject {

Q_OBJECT

private:
    QNetworkAccessManager *manager;
    KRunner::RunnerContext context;
    const QString query;
    const QString browserLaunchCommand;
    RequiredData data;
    QNetworkReply *reply;

public:
    DuckDuckGo(KRunner::RunnerContext &context, QString query, RequiredData data,
               QString browserLaunchCommand = "") : context(context), query(std::move(query)),
                                                    browserLaunchCommand(std::move(browserLaunchCommand)), data(std::move(data)) {
        manager = new QNetworkAccessManager(this);
        if (data.proxy) {
            manager->setProxy(*this->data.proxy);
        }

        QUrlQuery queryParameters;
        queryParameters.addQueryItem(QStringLiteral("q"), this->query);

        QNetworkRequest request(QUrl(QStringLiteral("https://lite.duckduckgo.com/lite/?") +
                                     QUrl(queryParameters.query(QUrl::FullyEncoded).toUtf8()).toEncoded()));
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
        request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader,
                          QStringLiteral("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:69.0) Gecko/20100101 Firefox/69.0"));

        reply = manager->get(request);

        connect(manager, &QNetworkAccessManager::finished, this, &DuckDuckGo::parseResponse);
        QTimer::singleShot(4000, reply, [this]() {
            reply->abort();
        });
    }

    virtual ~DuckDuckGo() {
        delete reply;
    }

public Q_SLOTS:

    void parseResponse() {
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
            // Parse html content
            QList<QStringList> urlList;
            QRegularExpression linkRegex(QStringLiteral("<a [^>]*rel=\"nofollow\"[^>]*href=\"([^\"]+)\"[^>]*>(.*)</a>"));
            const QString res = reply->readAll();
            QRegularExpressionMatchIterator it = linkRegex.globalMatch(res);
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
                KRunner::QueryMatch match(data.runner);
                match.setIcon(data.icon);
                match.setText(QString(currentList.at(1))
                                      .remove(QLatin1String("http://"))
                                      .remove(QLatin1String("https://"))
                                      .remove(QLatin1String("www.")));
                match.setRelevance((float) (19 - i) / 20);

                QMap<QString, QVariant> runData;
                runData.insert(QStringLiteral("url"), currentList.at(1));
                if (!browserLaunchCommand.isEmpty()) runData.insert(QStringLiteral("browser"), browserLaunchCommand);
                match.setData(runData);
                context.addMatch(match);
            }
        }

        Q_EMIT finished();
    }

Q_SIGNALS:

    void finished();
};

#endif //QUICKWEBSHORTCUTS_DUCKDUCKGO_H
