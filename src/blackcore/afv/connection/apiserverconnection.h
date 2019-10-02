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
            //! A server connection
            class ApiServerConnection : public QObject
            {
                Q_OBJECT

            public:
                //! Server errors
                enum ServerError
                {
                    NoError
                };

                //! Constructor
                ApiServerConnection(const QString &address, QObject *parent = nullptr);

                //! User authenticated?
                bool isAuthenticated() const { return m_isAuthenticated; }

                //! Connect to network
                bool connectTo(const QString &username, const QString &password, const QUuid &networkVersion);

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

            private:
                template<typename TResponse>
                TResponse postNoRequest(const QString &resource)
                {
                    if (!m_isAuthenticated)
                    {
                        qDebug() << "Not authenticated";
                        return {};
                    }

                    checkExpiry();

                    QNetworkAccessManager *nam = sApp->getNetworkAccessManager();

                    m_watch->start();
                    QUrl url(m_address);
                    url.setPath(resource);
                    QEventLoop loop;
                    connect(nam, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

                    QNetworkRequest request(url);
                    request.setRawHeader("Authorization", "Bearer " + m_jwt);
                    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nam->post(request, QByteArray()));
                    while (! reply->isFinished()) { loop.exec(); }
                    qDebug() << "POST" << resource << "(" << m_watch->elapsed() << "ms)";

                    if (reply->error() != QNetworkReply::NoError)
                    {
                        qWarning() << "POST" << resource << "failed:" << reply->errorString();
                        return {};
                    }

                    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                    TResponse response = TResponse::fromJson(doc.object());
                    return response;
                }

                template<typename TResponse>
                QVector<TResponse> getAsVector(const QString &resource)
                {
                    if (! m_isAuthenticated)
                    {
                        qDebug() << "Not authenticated";
                        return {};
                    }

                    checkExpiry();

                    QNetworkAccessManager *nam = sApp->getNetworkAccessManager();

                    m_watch->start();
                    QUrl url(m_address);
                    url.setPath(resource);
                    QEventLoop loop;
                    connect(nam, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

                    QNetworkRequest request(url);
                    request.setRawHeader("Authorization", "Bearer " + m_jwt);
                    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply(nam->get(request));
                    while (! reply->isFinished()) { loop.exec(); }
                    qDebug() << "GET" << resource << "(" << m_watch->elapsed() << "ms)";

                    if (reply->error() != QNetworkReply::NoError)
                    {
                        qWarning() << "GET" << resource << "failed:" << reply->errorString();
                        return {};
                    }

                    const QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
                    QVector<TResponse> dtos;
                    if (jsonDoc.isArray())
                    {
                        QJsonArray rootArray = jsonDoc.array();
                        for (auto o : rootArray)
                        {
                            QJsonObject d = o.toObject();
                            TResponse dto = TResponse::fromJson(d);
                            dtos.push_back(dto);
                        }
                    }
                    return dtos;
                }

                void postNoResponse(const QString &resource, const QJsonDocument &json);
                void deleteResource(const QString &resource);
                void checkExpiry();
                static bool isShuttingDown();

                const QString m_address;
                QByteArray    m_jwt;
                QString       m_username;
                QString       m_password;
                QUuid         m_networkVersion;
                QDateTime     m_expiryLocalUtc;
                qint64        m_serverToUserOffsetMs;
                bool          m_isAuthenticated = false;
                QElapsedTimer *m_watch = nullptr;
            };
        } // ns
    } // ns
} // ns

#endif // guard
