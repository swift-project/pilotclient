#include "apiserverconnection.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>

ApiServerConnection::ApiServerConnection(const QString &address, QObject *parent) :
    QObject(parent),
    m_address(address)
{
    qDebug() << "ApiServerConnection instantiated";
}

void ApiServerConnection::connectTo(const QString &username, const QString &password, const QUuid &networkVersion)
{
    m_username = username;
    m_password = password;
    m_networkVersion = networkVersion;
    m_watch.start();

    QUrl url(m_address);
    url.setPath("/api/v1/auth");

    QJsonObject obj
    {
        {"username", username},
        {"password", password},
        {"networkversion", networkVersion.toString()},
    };

    QNetworkAccessManager *nam = sApp->getNetworkAccessManager();
    QEventLoop loop;
    connect(nam, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = nam->post(request, QJsonDocument(obj).toJson());
    while(! reply->isFinished() ) { loop.exec(); }
    qDebug() << "POST api/v1/auth (" << m_watch.elapsed() << "ms)";

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning() << reply->errorString();
        return;
    }

    m_jwt = reply->readAll().trimmed();
    // TODO JwtSecurityToken. Now we assume its 6 hours
    m_serverToUserOffset = 0;
    m_expiryLocalUtc = QDateTime::currentDateTimeUtc().addSecs( 6 * 60 * 60);
    m_isAuthenticated = true;
}

PostCallsignResponseDto ApiServerConnection::addCallsign(const QString &callsign)
{
    return postNoRequest<PostCallsignResponseDto>("/api/v1/users/" + m_username + "/callsigns/" + callsign);
}

void ApiServerConnection::removeCallsign(const QString &callsign)
{
    deleteResource("/api/v1/users/" + m_username + "/callsigns/" + callsign);
}

void ApiServerConnection::updateTransceivers(const QString &callsign, const QVector<TransceiverDto> &transceivers)
{
    QJsonArray array;
    for (const TransceiverDto &tx : transceivers)
    {
        array.append(tx.toJson());
    }

    postNoResponse("/api/v1/users/" + m_username + "/callsigns/" + callsign + "/transceivers", QJsonDocument(array));
}

void ApiServerConnection::forceDisconnect()
{
    m_isAuthenticated = false;
    m_jwt.clear();
}

void ApiServerConnection::postNoResponse(const QString &resource, const QJsonDocument &json)
{
    if (! m_isAuthenticated)
    {
        qDebug() << "Not authenticated";
        return;
    }

    checkExpiry();

    m_watch.start();
    QUrl url(m_address);
    url.setPath(resource);
    QNetworkAccessManager *nam = sApp->getNetworkAccessManager();
    QEventLoop loop;
    connect(nam, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + m_jwt);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = nam->post(request, json.toJson());
    while(! reply->isFinished() ) { loop.exec(); }
    qDebug() << "POST" << resource << "(" << m_watch.elapsed() << "ms)";

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning() << "POST" << resource << "failed:" << reply->errorString();
        return;
    }

    reply->deleteLater();
}

void ApiServerConnection::deleteResource(const QString &resource)
{
    if (! m_isAuthenticated) { return; }

    m_watch.start();
    QUrl url(m_address);
    url.setPath(resource);

    QNetworkAccessManager *nam = sApp->getNetworkAccessManager();
    QEventLoop loop;
    connect(nam, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + m_jwt);
    QNetworkReply *reply = nam->deleteResource(request);
    while(! reply->isFinished() ) { loop.exec(); }
    qDebug() << "DELETE" << resource << "(" << m_watch.elapsed() << "ms)";

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning() << "DELETE" << resource << "failed:" << reply->errorString();
        return;
    }

    reply->deleteLater();
}

void ApiServerConnection::checkExpiry()
{
    if (QDateTime::currentDateTimeUtc() > m_expiryLocalUtc.addSecs(-5 * 60))
    {
        connectTo(m_username, m_password, m_networkVersion);
    }
}


