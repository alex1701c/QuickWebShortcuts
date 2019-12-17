#ifndef QUICKWEBSHORTCUTS_GOOGLE_H
#define QUICKWEBSHORTCUTS_GOOGLE_H

#include <QtNetwork/QNetworkReply>
#include <QtCore>
#include <KNotifications/KNotification>
#include <utility>
#include "RequiredData.h"

class Google : public QObject {

Q_OBJECT

private:
    QNetworkAccessManager *manager;
    Plasma::RunnerContext context;
    const QString query;
    const QString language;
    const QString browserLaunchCommand;
    RequiredData data;

public:
    Google(Plasma::RunnerContext &context,
           QString query,
           RequiredData data,
           QString language = "en",
           QString browserLaunchCommand = ""
    ) : context(context), query(std::move(query)), language(std::move(language)),
        browserLaunchCommand(std::move(browserLaunchCommand)), data(std::move(data)) {

        manager = new QNetworkAccessManager(this);
        if (data.proxy != nullptr) {
            manager->setProxy(*this->data.proxy);
        }

        QUrlQuery queryParameters;
        queryParameters.addQueryItem("q", this->query);
        queryParameters.addQueryItem("hl", this->language);
        queryParameters.addQueryItem("output", "toolbar");

        QNetworkRequest request(QUrl("https://clients1.google.com/complete/search?" +
                                     QUrl(queryParameters.query(QUrl::FullyEncoded).toUtf8()).toEncoded()));
        request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        const auto initialReply = manager->get(request);

        connect(manager, &QNetworkAccessManager::finished, this, &Google::parseResponse);
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
            const QString xmlContent = reply->readAll();
            QXmlStreamReader reader(xmlContent);
            QStringList suggestions;
            if (!reader.hasError()) {
                reader.readNextStartElement();
                if (reader.name() == "toplevel") {
                    while (reader.readNextStartElement()) {
                        reader.readNext();
                        const QString suggestion = reader.attributes().value("data").toString();
                        if (suggestion != query) suggestions.append(suggestion);
                        reader.readElementText();
                        reader.skipCurrentElement();

                    }
                }
            }
            const int suggestionCount = suggestions.count();
            for (int i = 0; i < suggestionCount && i < data.maxResults; ++i) {
                const QString &suggestion = suggestions.at(i);
                Plasma::QueryMatch match(data.runner);
                match.setIcon(data.icon);
                match.setText(data.searchOptionTemplate.arg(suggestion));
                match.setRelevance((float) (19 - i) / 20);

                QMap<QString, QVariant> runData;
                runData.insert("url", data.searchEngine + QUrl::toPercentEncoding(suggestion));
                if (!browserLaunchCommand.isEmpty()) {
                    runData.insert("browser", browserLaunchCommand);
                }
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

#endif //QUICKWEBSHORTCUTS_GOOGLE_H
