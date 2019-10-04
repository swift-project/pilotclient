/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "apiserverconnection.h"

#include "blackmisc/network/networkutils.h"
#include "blackmisc/network/external/qjsonwebtoken.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/logcategory.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QScopedPointer>
#include <QMetaEnum>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore
{
    namespace Afv
    {
        namespace Connection
        {
            const CLogCategoryList &CApiServerConnection::getLogCategories()
            {
                static const CLogCategoryList cats { CLogCategory::audio(), CLogCategory::vatsimSpecific() };
                return cats;
            }

            CApiServerConnection::CApiServerConnection(const QString &address, QObject *parent) :
                QObject(parent),
                m_address(address)
            {
                CLogMessage(this).debug(u"ApiServerConnection instantiated");
            }

            bool CApiServerConnection::connectTo(const QString &username, const QString &password, const QUuid &networkVersion)
            {
                if (isShuttingDown()) { return false; }

                m_username = username;
                m_password = password;
                m_networkVersion  = networkVersion;
                m_isAuthenticated = false;

                QUrl url(m_address);
                url.setPath("/api/v1/auth");

                QJsonObject obj
                {
                    {"username", username},
                    {"password", password},
                    {"networkversion", networkVersion.toString()},
                };

                QNetworkRequest request(url);
                request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
                QEventLoop loop(sApp);

                // posted in QAM thread
                QNetworkReply *reply = sApp->postToNetwork(request, CApplication::NoLogRequestId, QJsonDocument(obj).toJson(),
                {
                    this, [ & ](QNetworkReply * nwReply)
                    {
                        // called in "this" thread
                        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nwReply);
                        if (isShuttingDown()) { return; }
                        this->logRequestDuration(reply.data(), "authentication");
                        if (reply->error() != QNetworkReply::NoError)
                        {
                            this->logReplyErrorMessage(reply.data(), "authentication error");
                            loop.exit();
                            return;
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

                        loop.exit();
                    }
                });

                if (reply) { loop.exec(); }
                return m_isAuthenticated;
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
                QJsonArray array;
                for (const TransceiverDto &tx : transceivers)
                {
                    array.append(tx.toJson());
                }
                this->postNoResponse("/api/v1/users/" + m_username + "/callsigns/" + callsign + "/transceivers", QJsonDocument(array));
            }

            void CApiServerConnection::forceDisconnect()
            {
                m_isAuthenticated = false;
                m_jwt.clear();
            }

            QVector<StationDto> CApiServerConnection::getAllAliasedStations()
            {
                this->getAsVector<StationDto>("/api/v1/stations/aliased");
                return {};
            }

            QByteArray CApiServerConnection::getWithResponse(const QNetworkRequest &request)
            {
                if (isShuttingDown()) { return {}; }

                QEventLoop loop(sApp);
                QByteArray receivedData;

                // posted in QAM thread
                QNetworkReply *reply = sApp->getFromNetwork(request,
                {
                    this, [ & ](QNetworkReply * nwReply)
                    {
                        // called in "this" thread
                        if (!isShuttingDown())
                        {
                            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nwReply);
                            if (isShuttingDown()) { return; }
                            this->logRequestDuration(reply.data());
                            if (reply->error() == QNetworkReply::NoError)
                            {
                                receivedData = reply->readAll();
                            }
                            else
                            {
                                this->logReplyErrorMessage(reply.data());
                            }
                        }
                        loop.exit();
                    }
                });

                if (!reply) { return {}; }
                loop.exec();
                return receivedData;
            }

            QByteArray CApiServerConnection::postWithResponse(const QNetworkRequest &request, const QByteArray &data)
            {
                if (isShuttingDown()) { return {}; }

                QEventLoop loop(sApp);
                QByteArray receivedData;

                // posted in QAM thread
                QNetworkReply *reply = sApp->postToNetwork(request, CApplication::NoLogRequestId, data,
                {
                    this, [ & ](QNetworkReply * nwReply)
                    {
                        // called in "this" thread
                        if (!isShuttingDown())
                        {
                            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nwReply);
                            if (isShuttingDown()) { return; }
                            this->logRequestDuration(reply.data());
                            if (reply->error() == QNetworkReply::NoError)
                            {
                                receivedData = reply->readAll();
                            }
                            else
                            {
                                this->logReplyErrorMessage(reply.data());
                            }
                        }
                        loop.exit();
                    }
                });

                if (!reply) { return {}; }
                loop.exec();
                return receivedData;
            }

            void CApiServerConnection::postNoResponse(const QString &resource, const QJsonDocument &json)
            {
                if (isShuttingDown()) { return; } // avoid crash
                if (!m_isAuthenticated)
                {
                    CLogMessage(this).debug(u"AFV not authenticated");
                    return;
                }

                this->checkExpiry();

                QUrl url(m_address);
                url.setPath(resource);
                QNetworkRequest request(url);
                request.setRawHeader("Authorization", "Bearer " + m_jwt);
                request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

                // posted in QAM thread
                sApp->postToNetwork(request, CApplication::NoLogRequestId, json.toJson(),
                {
                    this, [ & ](QNetworkReply * nwReply)
                    {
                        // called in "this" thread
                        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nwReply);
                        if (isShuttingDown()) { return; }
                        this->logRequestDuration(reply.data());
                        if (reply->error() != QNetworkReply::NoError)
                        {
                            this->logReplyErrorMessage(reply.data());
                        }
                    }
                });
            }

            void CApiServerConnection::deleteResource(const QString &resource)
            {
                if (isShuttingDown())   { return; }
                if (!m_isAuthenticated) { return; }

                QUrl url(m_address);
                url.setPath(resource);

                QNetworkRequest request(url);
                request.setRawHeader("Authorization", "Bearer " + m_jwt);

                // posted in QAM thread
                sApp->deleteResourceFromNetwork(request, CApplication::NoLogRequestId,
                {
                    this, [ & ](QNetworkReply * nwReply)
                    {
                        // called in "this" thread
                        QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nwReply);
                        if (isShuttingDown()) { return; }
                        this->logRequestDuration(reply.data());
                        if (reply->error() != QNetworkReply::NoError)
                        {
                            this->logReplyErrorMessage(reply.data());
                        }
                    }
                });
            }

            void CApiServerConnection::checkExpiry()
            {
                if (QDateTime::currentDateTimeUtc() > m_expiryLocalUtc.addSecs(-5 * 60))
                {
                    connectTo(m_username, m_password, m_networkVersion);
                }
            }

            void CApiServerConnection::logReplyErrorMessage(const QNetworkReply *reply, const QString &addMsg)
            {
                if (!reply) { return; }
                if (addMsg.isEmpty())
                {
                    CLogMessage(this).warning(u"AFV network error for '%1' '%2': '%3'") << reply->url().toString() << CNetworkUtils::networkOperationToString(reply->operation()) << reply->errorString();
                }
                else
                {
                    CLogMessage(this).warning(u"AFV network error (%1) for '%2' '%3': '%4'") << addMsg << reply->url().toString() << CNetworkUtils::networkOperationToString(reply->operation()) << reply->errorString();
                }
            }

            void CApiServerConnection::logRequestDuration(const QNetworkReply *reply, const QString &addMsg)
            {
                if (!reply) { return; }
                const qint64 d = CNetworkUtils::requestDuration(reply);
                if (d < 0) { return; }
                if (addMsg.isEmpty())
                {
                    CLogMessage(this).info(u"AFV network request for '%1': %2ms") << reply->url().toString() << d;
                }
                else
                {
                    CLogMessage(this).info(u"AFV network request (%1) for '%2': '%3'") << addMsg << reply->url().toString() << d;
                }
            }

            bool CApiServerConnection::isShuttingDown()
            {
                return !sApp || sApp->isShuttingDown();
            }

        } // ns
    } // ns
}// ns

