/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_CONNECTION_APISERVERCONNECTION_H
#define BLACKCORE_AFV_CONNECTION_APISERVERCONNECTION_H

#include "blackcore/afv/dto.h"
#include "blackcore/application.h"
#include "blackmisc/slot.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/logcategorylist.h"

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

namespace BlackCore
{
    namespace Afv
    {
        namespace Connection
        {
            //! Callback for
            using ConnectionCallback = BlackMisc::CSlot<void(bool)>;

            //! A server connection
            class CApiServerConnection : public QObject
            {
                Q_OBJECT

            public:
                //! Server errors
                enum ServerError
                {
                    NoError
                };

                //! Categories
                static const BlackMisc::CLogCategoryList &getLogCategories();

                //! Constructor
                CApiServerConnection(const QString &address, QObject *parent = nullptr);

                //! User authenticated?
                bool isAuthenticated() const { return m_isAuthenticated; }

                //! Connect to network
                //! \remark ASYNC, calling callback when done
                void connectTo(const QString &username, const QString &password, const QString &client, const QUuid &networkVersion, ConnectionCallback callback);

                //! Add callsign to network
                PostCallsignResponseDto addCallsign(const QString &callsign);

                //! Remove callsign from network
                void removeCallsign(const QString &callsign);

                //! Update transceivers
                void updateTransceivers(const QString &callsign, const QVector<TransceiverDto> &transceivers);

                //! Force disconnect from network
                void forceDisconnect();

                //! All aliased stations
                QVector<StationDto> getAllAliasedStations();

                //! Set the URL
                bool setUrl(const QString &url);

                //! Get the URL
                const QString &getUrl() const { return m_addressUrl; }

                //! User data @{
                const QString &getUserName() const { return m_username; }
                const QString &getPassword() const { return m_password; }
                const QString &getClient()   const { return m_client; }
                const QUuid   &getNetworkVersion() const { return m_networkVersion; }
                //! @}

            private:
                //! Post to resource
                template<typename TResponse>
                TResponse postNoRequest(const QString &resource)
                {
                    if (!this->sendToNetworkIfAuthenticated())
                    {
                        // BlackMisc::CLogMessage(this).debug(u"AFV not authenticated");
                        return {};
                    }

                    this->checkExpiry();

                    QUrl url(m_addressUrl);
                    url.setPath(resource);
                    QNetworkRequest request(url);
                    request.setRawHeader("Authorization", "Bearer " + m_jwt);

                    const QByteArray receivedData = this->postWithResponse(request);
                    const QJsonDocument doc = QJsonDocument::fromJson(receivedData);
                    const TResponse response = TResponse::fromJson(doc.object());
                    return response;
                }

                //! Get resource and return as vector
                template<typename TResponse>
                QVector<TResponse> getAsVector(const QString &resource)
                {
                    if (!this->sendToNetworkIfAuthenticated())
                    {
                        // BlackMisc::CLogMessage(this).debug(u"AFV not authenticated");
                        return {};
                    }

                    this->checkExpiry();

                    QUrl url(m_addressUrl);
                    url.setPath(resource);
                    QNetworkRequest request(url);
                    request.setRawHeader("Authorization", "Bearer " + m_jwt);

                    const QByteArray receivedData = this->getWithResponse(request);
                    const QJsonDocument jsonDoc = QJsonDocument::fromJson(receivedData);
                    QVector<TResponse> dtos;
                    if (jsonDoc.isArray())
                    {
                        const QJsonArray rootArray = jsonDoc.array();
                        for (const auto &o : rootArray)
                        {
                            const QJsonObject d = o.toObject();
                            const TResponse dto = TResponse::fromJson(d);
                            dtos.push_back(dto);
                        }
                    }
                    return dtos;
                }

                //! Pseudo synchronous post request returning data
                QByteArray getWithResponse(const QNetworkRequest &request);

                //! Pseudo synchronous post request returning data
                QByteArray postWithResponse(const QNetworkRequest &request, const QByteArray &data = {});

                //! Post but do NOT wait for response
                void postNoResponse(const QString &resource, const QJsonDocument &json);

                //! Delete and do NOT wait for response
                void deleteResource(const QString &resource);

                //! Session expired, then re-login
                void checkExpiry();

                //! Message if reply has error
                void logReplyErrorMessage(const QNetworkReply *reply, const QString &addMsg = {});

                //! Message if reply has error
                void logRequestDuration(const QNetworkReply *reply, const QString &addMsg = {});

                //! Get QLoop for network access, using class must delete the loop
                QEventLoop *newEventLoop();

                //! Send to network
                bool sendToNetworkIfAuthenticated() const;

                //! Application shutting down
                static bool isShuttingDown();

                QString       m_addressUrl;
                QByteArray    m_jwt;
                QString       m_username;
                QString       m_password;
                QUuid         m_networkVersion;
                QString       m_client;
                QDateTime     m_expiryLocalUtc;
                qint64        m_serverToUserOffsetMs;
                bool          m_isAuthenticated = false;
            };
        } // ns
    } // ns
} // ns

#endif // guard
