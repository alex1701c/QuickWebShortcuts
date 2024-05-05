#ifndef QUICKWEBSHORTCUTS_GOOGLE_H
#define QUICKWEBSHORTCUTS_GOOGLE_H

#include <QNetworkReply>
#include <QUrlQuery>
#include <QXmlStreamReader>
#include <KNotifications/KNotification>
#include <utility>
#include <KRunner/QueryMatch>
#include <KRunner/RunnerContext>

#include "RequiredData.h"

class Google : public QObject {

Q_OBJECT

private:
    QNetworkAccessManager *manager;
    KRunner::RunnerContext context;
    const QString query;
    const QString language;
    const QString browserLaunchCommand;
    RequiredData data;
    QNetworkReply *reply;

public:
    Google(KRunner::RunnerContext &context,
           QString query,
           RequiredData data,
           QString language = "en",
           QString browserLaunchCommand = ""
    ) : context(context), query(std::move(query)), language(std::move(language)),
        browserLaunchCommand(std::move(browserLaunchCommand)), data(std::move(data)) {

        manager = new QNetworkAccessManager(this);
        if (data.proxy) {
            manager->setProxy(*this->data.proxy);
        }

        QUrlQuery queryParameters;
        queryParameters.addQueryItem(QStringLiteral("q"), this->query);
        queryParameters.addQueryItem(QStringLiteral("hl"), this->language);
        queryParameters.addQueryItem(QStringLiteral("output"), "toolbar");

        QNetworkRequest request(QUrl(QStringLiteral("https://clients1.google.com/complete/search?") +
                                     QUrl(queryParameters.query(QUrl::FullyEncoded).toUtf8()).toEncoded()));
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));

        reply = manager->get(request);

        connect(manager, &QNetworkAccessManager::finished, this, &Google::parseResponse);
        QTimer::singleShot(2000, reply, [this]() {
            reply->abort();
        });
    }

    virtual ~Google() {
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
            const QString xmlContent = reply->readAll();
            QXmlStreamReader reader(xmlContent);
            QStringList suggestions;
            if (!reader.hasError()) {
                reader.readNextStartElement();
                if (reader.name() == QLatin1String("toplevel")) {
                    while (reader.readNextStartElement()) {
                        reader.readNext();
                        const QString suggestion = reader.attributes().value(QStringLiteral("data")).toString();
                        if (suggestion != query) suggestions.append(suggestion);
                        reader.readElementText();
                        reader.skipCurrentElement();

                    }
                }
            }
            const int suggestionCount = suggestions.count();
            for (int i = 0; i < suggestionCount && i < data.maxResults; ++i) {
                const QString &suggestion = suggestions.at(i);
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
                if (!browserLaunchCommand.isEmpty()) {
                    runData.insert(QStringLiteral("browser"), browserLaunchCommand);
                }
                match.setData(runData);
                context.addMatch(match);
            }

        }
        Q_EMIT finished();
    }

Q_SIGNALS:

    void finished();
};

#endif //QUICKWEBSHORTCUTS_GOOGLE_H
