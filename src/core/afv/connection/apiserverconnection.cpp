// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/afv/connection/apiserverconnection.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>
#include <QPointer>
#include <QScopedPointer>
#include <QUrl>
#include <QUrlQuery>

#include "qjsonwebtoken/qjsonwebtoken.h"

#include "config/buildconfig.h"
#include "misc/logmessage.h"
#include "misc/network/networkutils.h"
#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::config;

namespace swift::core::afv::connection
{
    const QStringList &CApiServerConnection::getLogCategories()
    {
        static const QStringList cats { CLogCategories::audio(), CLogCategories::vatsimSpecific() };
        return cats;
    }

    CApiServerConnection::CApiServerConnection(const QString &address, QObject *parent)
        : QObject(parent), m_addressUrl(address)
    {
        CLogMessage(this).debug(u"ApiServerConnection instantiated");
    }

    void CApiServerConnection::connectTo(const QString &username, const QString &password, const QString &client,
                                         const QUuid &networkVersion, ConnectionCallback callback)
    {
        if (isShuttingDown()) { return; }

        m_username = username;
        m_password = password;
        m_client = client;
        m_networkVersion = networkVersion;
        m_isAuthenticated = false;

        QUrl url(m_addressUrl);
        url.setPath("/api/v1/auth");

        QJsonObject obj { { "username", username },
                          { "password", password },
                          { "networkversion", networkVersion.toString() },
                          { "client", client } };

        QNetworkRequest request(url);
        QPointer<CApiServerConnection> myself(this);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // posted in QAM thread, reply is nullptr if called from another thread
        sApp->postToNetwork(request, CApplication::NoLogRequestId, QJsonDocument(obj).toJson(),
                            { this, [=](QNetworkReply *nwReply) {
                                 // called in "this" thread
                                 const QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nwReply);
                                 if (!myself || isShuttingDown()) // cppcheck-suppress knownConditionTrueFalse
                                 {
                                     return;
                                 }

                                 this->logRequestDuration(reply.data(), "authentication");
                                 if (reply->error() != QNetworkReply::NoError)
                                 {
                                     this->logReplyErrorMessage(reply.data(), "authentication error");
                                     callback(false);
                                     return;
                                 }

                                 // JWT authentication token
                                 m_serverToUserOffsetMs = 0;
                                 m_expiryLocalUtc = QDateTime(); // clean up

                                 m_jwt = reply->readAll().trimmed();
                                 qint64 lifeTimeSecs = -1;
                                 qint64 serverToUserOffsetSecs = -1;
                                 do {
                                     const QString jwtToken(m_jwt);
                                     const QJsonWebToken token = QJsonWebToken::fromTokenAndSecret(jwtToken, "");

                                     // get decoded header and payload
                                     // QString strHeader  = token.getHeaderQStr();
                                     // QString strPayload = token.getPayloadQStr();
                                     const QJsonDocument doc = token.getPayloadJDoc();
                                     if (doc.isEmpty() || !doc.isObject()) { break; }
                                     const qint64 validFromSecs = doc.object().value("nbf").toInt(-1);
                                     if (validFromSecs < 0) { break; }
                                     const qint64 localSecsSinceEpoch = QDateTime::currentSecsSinceEpoch();
                                     serverToUserOffsetSecs = validFromSecs - localSecsSinceEpoch;
                                     const qint64 serverExpirySecs = doc.object().value("exp").toInt();
                                     const qint64 expiryLocalUtc = serverExpirySecs - serverToUserOffsetSecs;
                                     lifeTimeSecs = expiryLocalUtc - localSecsSinceEpoch;
                                 }
                                 while (false);

                                 if (lifeTimeSecs > 0)
                                 {
                                     m_serverToUserOffsetMs = serverToUserOffsetSecs * 1000;
                                     m_expiryLocalUtc = QDateTime::currentDateTimeUtc().addSecs(lifeTimeSecs);
                                     m_isAuthenticated = true;
                                 }

                                 // connected, callback
                                 callback(m_isAuthenticated);
                             } });
    }

    PostCallsignResponseDto CApiServerConnection::addCallsign(const QString &callsign)
    {
        return this->postNoRequest<PostCallsignResponseDto>("/api/v1/users/" + m_username + "/callsigns/" + callsign);
    }

    void CApiServerConnection::removeCallsign(const QString &callsign)
    {
        this->deleteResource("/api/v1/users/" + m_username + "/callsigns/" + callsign);
    }

    void CApiServerConnection::updateTransceivers(const QString &callsign, const QVector<TransceiverDto> &transceivers)
    {
        if (!this->sendToNetworkIfAuthenticated()) { return; }
        QJsonArray array;
        for (const TransceiverDto &tx : transceivers) { array.append(tx.toJson()); }
        this->postNoResponse("/api/v1/users/" + m_username + "/callsigns/" + callsign + "/transceivers",
                             QJsonDocument(array));
    }

    void CApiServerConnection::forceDisconnect()
    {
        m_isAuthenticated = false;
        m_jwt.clear();
    }

    QVector<StationDto> CApiServerConnection::getAllAliasedStations()
    {
        const QVector<StationDto> stations = this->getAsVector<StationDto>("/api/v1/stations/aliased");
        return stations;
    }

    bool CApiServerConnection::setUrl(const QString &url)
    {
        if (stringCompare(m_addressUrl, url, Qt::CaseInsensitive)) { return false; }
        m_addressUrl = url;
        return true;
    }

    QByteArray CApiServerConnection::getWithResponse(const QNetworkRequest &request)
    {
        if (isShuttingDown()) { return {}; }

        QPointer<QEventLoop> loop(this->newEventLoop());
        QByteArray receivedData;

        // posted in QAM thread, reply is nullptr if called from another thread
        sApp->getFromNetwork(request,
                             { this, [=, &receivedData](QNetworkReply *nwReply) {
                                  const QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nwReply);

                                  // called in "this" thread
                                  if (loop && !isShuttingDown())
                                  {
                                      this->logRequestDuration(reply.data());
                                      if (reply->error() == QNetworkReply::NoError) { receivedData = reply->readAll(); }
                                      else { this->logReplyErrorMessage(reply.data()); }
                                  }
                                  if (loop) { loop->exit(); }
                              } });

        if (loop) { loop->exec(); }
        return receivedData;
    }

    QByteArray CApiServerConnection::postWithResponse(const QNetworkRequest &request, const QByteArray &data)
    {
        if (isShuttingDown()) { return {}; }

        QPointer<QEventLoop> loop(this->newEventLoop());
        QByteArray receivedData;

        // posted in QAM thread, reply is nullptr if called from another thread
        sApp->postToNetwork(request, CApplication::NoLogRequestId, data,
                            { this, [=, &receivedData](QNetworkReply *nwReply) {
                                 const QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nwReply);

                                 // called in "this" thread
                                 if (loop && !isShuttingDown())
                                 {
                                     this->logRequestDuration(reply.data());
                                     if (reply->error() == QNetworkReply::NoError) { receivedData = reply->readAll(); }
                                     else { this->logReplyErrorMessage(reply.data()); }
                                 }
                                 if (loop) { loop->exit(); }
                             } });

        if (loop) { loop->exec(); }
        return receivedData;
    }

    void CApiServerConnection::postNoResponse(const QString &resource, const QJsonDocument &json)
    {
        if (isShuttingDown()) { return; }
        this->checkExpiry();

        QUrl url(m_addressUrl);
        url.setPath(resource);
        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + m_jwt);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        // posted in QAM thread, reply is nullptr if called from another thread
        sApp->postToNetwork(request, CApplication::NoLogRequestId, json.toJson(),
                            { this, [=](QNetworkReply *nwReply) {
                                 // called in "this" thread
                                 const QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nwReply);
                                 if (isShuttingDown()) { return; }
                                 this->logRequestDuration(reply.data());
                                 if (reply->error() != QNetworkReply::NoError)
                                 {
                                     this->logReplyErrorMessage(reply.data());
                                 }
                             } });
    }

    void CApiServerConnection::deleteResource(const QString &resource)
    {
        if (isShuttingDown()) { return; }

        QUrl url(m_addressUrl);
        url.setPath(resource);

        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + m_jwt);

        // posted in QAM thread
        sApp->deleteResourceFromNetwork(request, CApplication::NoLogRequestId,
                                        { this, [=](QNetworkReply *nwReply) {
                                             // called in "this" thread
                                             const QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(
                                                 nwReply);
                                             if (isShuttingDown()) { return; }
                                             this->logRequestDuration(reply.data());
                                             if (reply->error() != QNetworkReply::NoError)
                                             {
                                                 this->logReplyErrorMessage(reply.data());
                                             }
                                         } });
    }

    void CApiServerConnection::checkExpiry()
    {
        if (!m_expiryLocalUtc.isValid() || QDateTime::currentDateTimeUtc() > m_expiryLocalUtc.addSecs(-5 * 60))
        {
            QPointer<CApiServerConnection> myself(this);
            this->connectTo(m_username, m_password, m_client, m_networkVersion,
                            { this, [=](bool authenticated) {
                                 if (!myself) { return; } // cppcheck-suppress knownConditionTrueFalse
                                 CLogMessage(this).info(u"API server authenticated '%1': %2")
                                     << m_username << boolToYesNo(authenticated);
                             } });
        }
    }

    void CApiServerConnection::logReplyErrorMessage(const QNetworkReply *reply, const QString &addMsg)
    {
        if (!reply) { return; }
        if (addMsg.isEmpty())
        {
            CLogMessage(this).warning(u"AFV network error for '%1' '%2': '%3'")
                << reply->url().toString() << CNetworkUtils::networkOperationToString(reply->operation())
                << reply->errorString();
        }
        else
        {
            CLogMessage(this).warning(u"AFV network error (%1) for '%2' '%3': '%4'")
                << addMsg << reply->url().toString() << CNetworkUtils::networkOperationToString(reply->operation())
                << reply->errorString();
        }
    }

    void CApiServerConnection::logRequestDuration(const QNetworkReply *reply, const QString &addMsg)
    {
        if (!CBuildConfig::isLocalDeveloperDebugBuild()) { return; }
        if (!reply) { return; }

        const qint64 d = CNetworkUtils::requestDuration(reply);
        if (d < 0) { return; }
        if (addMsg.isEmpty())
        {
            CLogMessage(this).info(u"AFV network request for '%1': %2ms") << reply->url().toString() << d;
        }
        else
        {
            CLogMessage(this).info(u"AFV network request (%1) for '%2': '%3'")
                << addMsg << reply->url().toString() << d;
        }
    }

    QEventLoop *CApiServerConnection::newEventLoop()
    {
        QEventLoop *loop = new QEventLoop(this);
        if (sApp)
        {
            QObject::connect(sApp, &CApplication::aboutToShutdown, loop, &QEventLoop::quit, Qt::QueuedConnection);
        }
        return loop;
    }

    bool CApiServerConnection::sendToNetworkIfAuthenticated() const { return m_isAuthenticated && !isShuttingDown(); }

    bool CApiServerConnection::isShuttingDown() { return !sApp || sApp->isShuttingDown(); }

} // namespace swift::core::afv::connection
