#ifndef APISERVERCONNECTION_H
#define APISERVERCONNECTION_H

#include "blackcore/afv/dto.h"
#include "blackcore/application.h"

#include <QString>
#include <QNetworkAccessManager>
#include <QObject>
#include <QElapsedTimer>
#include <QUuid>
#include <QDebug>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>

// TODO:
// - JWT refresh

class ApiServerConnection : public QObject
{
    Q_OBJECT

public:
    enum ServerError
    {
        NoError
    };

    ApiServerConnection(const QString &address, QObject *parent = nullptr);

    bool isAuthenticated() const { return m_isAuthenticated; }
    void connectTo(const QString &username, const QString &password, const QUuid &networkVersion);

    PostCallsignResponseDto addCallsign(const QString &callsign);
    void removeCallsign(const QString &callsign);

    void updateTransceivers(const QString &callsign, const QVector<TransceiverDto> &transceivers);

    void forceDisconnect();

private:
    template<typename TResponse>
    TResponse postNoRequest(const QString &resource)
    {
        if (! m_isAuthenticated)
        {
            qDebug() << "Not authenticated";
            return {};
        }

        checkExpiry();

        QNetworkAccessManager *nam = sApp->getNetworkAccessManager();

        m_watch.start();
        QUrl url(m_address);
        url.setPath(resource);
        QEventLoop loop;
        connect(nam, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + m_jwt);
        QNetworkReply *reply = nam->post(request, QByteArray());
        while(! reply->isFinished() ) { loop.exec(); }
        qDebug() << "POST" << resource << "(" << m_watch.elapsed() << "ms)";

        if (reply->error() != QNetworkReply::NoError)
        {
            qWarning() << "POST" << resource << "failed:" << reply->errorString();
            return {};
        }

        const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        TResponse response = TResponse::fromJson(doc.object());

        reply->deleteLater();
        return response;
    }

    void postNoResponse(const QString &resource, const QJsonDocument &json);
    void deleteResource(const QString &resource);
    void checkExpiry();

    const QString m_address;
    QByteArray m_jwt;
    QString m_username;
    QString m_password;
    QUuid m_networkVersion;
    QDateTime m_expiryLocalUtc;
    qint64 m_serverToUserOffset;

    bool m_isAuthenticated = false;

    QElapsedTimer m_watch;
};

#endif // APISERVERCONNECTION_H
