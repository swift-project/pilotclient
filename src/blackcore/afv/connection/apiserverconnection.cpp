/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "apiserverconnection.h"
#include "blackmisc/network/external/qjsonwebtoken.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QScopedPointer>

namespace BlackCore
{
    namespace Afv
    {
        namespace Connection
        {
            ApiServerConnection::ApiServerConnection(const QString &address, QObject *parent) :
                QObject(parent),
                m_address(address)
            {
                qDebug() << "ApiServerConnection instantiated";
            }

            bool ApiServerConnection::connectTo(const QString &username, const QString &password, const QUuid &networkVersion)
            {
                m_username = username;
                m_password = password;
                m_networkVersion = networkVersion;
                m_isAuthenticated = false;
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
                QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nam->post(request, QJsonDocument(obj).toJson()));
                while (! reply->isFinished()) { loop.exec(); }

                qDebug() << "POST api/v1/auth (" << m_watch.elapsed() << "ms)";
                if (reply->error() != QNetworkReply::NoError)
                {
                    qWarning() << reply->errorString();
                    return false;
                }

                // JWT authentication token
                m_serverToUserOffsetMs = 0;
                m_expiryLocalUtc = QDateTime(); // clean up

                m_jwt = reply->readAll().trimmed();
                qint64 lifeTimeSecs = -1;
                qint64 serverToUserOffsetSecs = -1;
                do
                {
                    const QString jwtToken(m_jwt);
                    QJsonWebToken token = QJsonWebToken::fromTokenAndSecret(jwtToken, "");

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

                return m_isAuthenticated;
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

            QVector<StationDto> ApiServerConnection::getAllAliasedStations()
            {
                getAsVector<StationDto>("/api/v1/stations/aliased");
                return {};
            }

            void ApiServerConnection::postNoResponse(const QString &resource, const QJsonDocument &json)
            {
                if (isShuttingDown()) { return; } // avoid crash
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
                QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nam->post(request, json.toJson()));
                while (! reply->isFinished()) { loop.exec(); }
                qDebug() << "POST" << resource << "(" << m_watch.elapsed() << "ms)";

                if (reply->error() != QNetworkReply::NoError)
                {
                    qWarning() << "POST" << resource << "failed:" << reply->errorString();
                    return;
                }
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
                QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nam->deleteResource(request));
                while (! reply->isFinished()) { loop.exec(); }
                qDebug() << "DELETE" << resource << "(" << m_watch.elapsed() << "ms)";

                if (reply->error() != QNetworkReply::NoError)
                {
                    qWarning() << "DELETE" << resource << "failed:" << reply->errorString();
                }
            }

            void ApiServerConnection::checkExpiry()
            {
                if (QDateTime::currentDateTimeUtc() > m_expiryLocalUtc.addSecs(-5 * 60))
                {
                    connectTo(m_username, m_password, m_networkVersion);
                }
            }

            bool ApiServerConnection::isShuttingDown()
            {
                return !sApp || sApp->isShuttingDown();
            }

        } // ns
    } // ns
}// ns

