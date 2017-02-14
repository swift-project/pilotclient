/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/networkutils.h"
#include "blackmisc/network/server.h"
#include "blackconfig/buildconfig.h"
#include <QAbstractSocket>
#include <QDateTime>
#include <QDebug>
#include <QHostAddress>
#include <QJsonDocument>
#include <QList>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QNetworkReply>
#include <QObject>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslSocket>
#include <QTcpSocket>
#include <QTextStream>
#include <QUrl>
#include <QUrlQuery>
#include <QVariant>
#include <QtDebug>
#include <QRegularExpression>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Network
    {
        int CNetworkUtils::getTimeoutMs()
        {
            return 2000;
        }

        bool CNetworkUtils::hasConnectedInterface(bool withDebugOutput)
        {
            // http://stackoverflow.com/questions/2475266/verfiying-the-network-connection-using-qt-4-4
            const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
            bool result = false;

            for (int i = 0; i < interfaces.count(); i++)
            {
                QNetworkInterface interface = interfaces.at(i);

                // details of connection
                if (withDebugOutput) qDebug() << "name:" << interface.name() << endl << "ip addresses:" << endl << "mac:" << interface.hardwareAddress() << endl;
                if (interface.flags().testFlag(QNetworkInterface::IsUp) && !interface.flags().testFlag(QNetworkInterface::IsLoopBack))
                {
                    // this loop is important
                    for (int j = 0; j < interface.addressEntries().count(); j++)
                    {
                        if (withDebugOutput) qDebug() << interface.addressEntries().at(j).ip().toString() << " / " << interface.addressEntries().at(j).netmask().toString() << endl;

                        // we have an interface that is up, and has an ip address, therefore the link is present
                        // we will only enable this check on first positive, all later results are incorrect
                        if (!result)
                        {
                            result = true;
                            break;
                        }
                    }
                }
            }
            return result;
        }

        QStringList CNetworkUtils::getKnownLocalIpV4Addresses()
        {
            QStringList ips;
            if (!CNetworkUtils::hasConnectedInterface(false)) {return ips; }
            for (const QHostAddress &address : QNetworkInterface::allAddresses())
            {
                if (address.isLoopback() || address.isNull()) continue;
                if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
                {
                    QString a = address.toString();
                    if (CNetworkUtils::isValidIPv4Address(a))
                    {
                        ips.append(a);
                    }
                }
            }
            return ips;
        }

        bool CNetworkUtils::canConnect(const QString &hostAddress, int port, QString &message, int timeoutMs)
        {
            if (!CNetworkUtils::hasConnectedInterface(false))
            {
                message = QObject::tr("No connected network interface", "BlackMisc");
                return false;
            }

            // http://qt-project.org/forums/viewthread/9346
            QTcpSocket socket;
            bool connected = false;
            socket.connectToHost(hostAddress, static_cast<quint16>(port));
            if (!socket.waitForConnected(timeoutMs))
            {
                message = QObject::tr("Connection failed : %1", "BlackMisc").arg(socket.errorString());
                connected = false;
            }
            else
            {
                message = QObject::tr("OK, connected", "BlackMisc");
                connected = true;
            }
            socket.close();
            return connected;
        }

        bool CNetworkUtils::canConnect(const Network::CServer &server, QString &message, int timeoutMs)
        {
            return CNetworkUtils::canConnect(server.getAddress(), server.getPort(), message, timeoutMs);
        }

        bool CNetworkUtils::canConnect(const QString &url, QString &message, int timeoutMs)
        {
            if (url.isEmpty())
            {
                message = QObject::tr("Missing URL", "BlackMisc");
                return false;
            }
            return canConnect(QUrl(url), message, timeoutMs);
        }

        bool CNetworkUtils::canConnect(const QUrl &url, QString &message, int timeoutMs)
        {
            if (!url.isValid())
            {
                message = QObject::tr("Invalid URL: %1", "BlackMisc").arg(url.toString());
                return false;
            }

            if (url.isRelative())
            {
                message = QObject::tr("Relative URL cannot be tested: %1", "BlackMisc").arg(url.toString());
                return false;
            }

            const QString host(url.host());
            const QString scheme(url.scheme().toLower());
            int p = url.port();
            if (p < 0)
            {
                p = scheme.contains("https") ? 443 : 80;
            }
            return canConnect(host, p, message, timeoutMs);
        }

        bool CNetworkUtils::canConnect(const QUrl &url, int timeoutMs)
        {
            QString m;
            return canConnect(url, m, timeoutMs);
        }

        bool CNetworkUtils::canConnect(const CUrl &url, QString &message, int timeoutMs)
        {
            return canConnect(url.getHost(), url.getPort(), message, timeoutMs);
        }

        bool CNetworkUtils::canConnect(const CUrl &url, int timeoutMs)
        {
            QString m;
            return canConnect(url, m, timeoutMs);
        }

        bool CNetworkUtils::isValidIPv4Address(const QString &candidate)
        {
            QHostAddress address(candidate);
            return (QAbstractSocket::IPv4Protocol == address.protocol());
        }

        bool CNetworkUtils::isValidIPv6Address(const QString &candidate)
        {
            QHostAddress address(candidate);
            return (QAbstractSocket::IPv6Protocol == address.protocol());
        }

        bool CNetworkUtils::isValidPort(const QString &port)
        {
            bool success;
            int p = port.toInt(&success);
            if (!success) return false;
            return (p >= 1 && p <= 65535);
        }

        QString CNetworkUtils::buildUrl(const QString &protocol, const QString &server, const QString &baseUrl, const QString &serviceUrl)
        {
            Q_ASSERT_X(protocol.length() > 3, Q_FUNC_INFO, "worng protocol");
            Q_ASSERT_X(!server.isEmpty(), Q_FUNC_INFO, "missing server");
            Q_ASSERT_X(!serviceUrl.isEmpty(), Q_FUNC_INFO, "missing service URL");

            QString url(server);
            if (!baseUrl.isEmpty())
            {
                url.append("/").append(baseUrl);
            }
            url.append("/").append(serviceUrl);
            url.replace("//", "/");
            return protocol + "://" + url;
        }

        void CNetworkUtils::ignoreSslVerification(QNetworkRequest &request)
        {
            QSslConfiguration conf = request.sslConfiguration();
            conf.setPeerVerifyMode(QSslSocket::VerifyNone);
            request.setSslConfiguration(conf);
        }

        void CNetworkUtils::setSwiftUserAgent(QNetworkRequest &request)
        {
            static const QString userAgent("swift/" + CVersion::version());
            request.setRawHeader("User-Agent", userAgent.toLatin1());
        }

        //! \cond PRIVATE
        QSslKey loadPrivateSslKey()
        {
            QFile privateKeyFile(":/blackmisc/ssl/swiftclient.key");
            privateKeyFile.open(QIODevice::ReadOnly);
            QSslKey privateKey(&privateKeyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, QByteArrayLiteral("ead811e4474a29539a37ff766ca18330"));
            Q_ASSERT(!privateKey.isNull());
            return privateKey;
        }

        QSslCertificate loadSslCertifcate()
        {
            QFile certificateFile(":/blackmisc/ssl/swiftclient.crt");
            certificateFile.open(QIODevice::ReadOnly);
            QSslCertificate certificate(&certificateFile);
            Q_ASSERT(!certificate.isNull());
            return certificate;
        }
        //! \endcond

        void CNetworkUtils::setSwiftClientSslCertificate(QNetworkRequest &request)
        {
            static const QSslKey privateKey(loadPrivateSslKey());
            static const QSslCertificate certificate(loadSslCertifcate());
            QSslConfiguration sslConfiguration = request.sslConfiguration();
            sslConfiguration.setPrivateKey(privateKey);
            sslConfiguration.setLocalCertificate(certificate);
            request.setSslConfiguration(sslConfiguration);
        }

        void CNetworkUtils::setSwiftClientSslCertificate(QNetworkRequest &request, const CUrlList &swiftSharedUrls)
        {
            for (const CUrl &sharedUrl : swiftSharedUrls)
            {
                const QString urlString = request.url().toString();
                if (urlString.startsWith(sharedUrl.toQString()))
                {
                    CNetworkUtils::setSwiftClientSslCertificate(request);
                    break;
                }
            }
        }

        QHttpPart CNetworkUtils::getMultipartWithDebugFlag()
        {
            QHttpPart textPartDebug;
            textPartDebug.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"XDEBUG_SESSION_START\""));
            textPartDebug.setBody(QString("ECLIPSE_DBGP").toUtf8());
            return textPartDebug;
        }

        void CNetworkUtils::addDebugFlag(QUrlQuery &qurl)
        {
            qurl.addQueryItem("XDEBUG_SESSION_START", "ECLIPSE_DBGP");
        }

        QHttpPart CNetworkUtils::getJsonTextMultipart(const QJsonObject &json)
        {
            const QByteArray bytes(QJsonDocument(json).toJson(QJsonDocument::Compact));
            return getJsonTextMultipart(bytes);
        }

        QHttpPart CNetworkUtils::getJsonTextMultipart(const QJsonArray &json)
        {
            const QByteArray bytes(QJsonDocument(json).toJson(QJsonDocument::Compact));
            return getJsonTextMultipart(bytes);
        }

        QHttpPart CNetworkUtils::getJsonTextMultipart(const QByteArray &bytes)
        {
            QHttpPart textPart;
            QString name("form-data; name=\"swiftjson\"");
            textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(name));
            textPart.setBody(bytes);
            return textPart;
        }

        QNetworkRequest CNetworkUtils::getNetworkRequest(const CUrl &url, RequestType type)
        {
            QNetworkRequest request(url.toQUrl());
            switch (type)
            {
            case PostUrlEncoded:
                request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
                break;
            default:
                break;
            }
            CNetworkUtils::ignoreSslVerification(request);
            CNetworkUtils::setSwiftUserAgent(request);
            return request;
        }

        qint64 CNetworkUtils::lastModifiedMsSinceEpoch(QNetworkReply *nwReply)
        {
            Q_ASSERT(nwReply);
            const QDateTime lm = CNetworkUtils::lastModifiedDateTime(nwReply);
            return lm.isValid() ? lm.toMSecsSinceEpoch() : -1;
        }

        QDateTime CNetworkUtils::lastModifiedDateTime(QNetworkReply *nwReply)
        {
            Q_ASSERT(nwReply);
            const QVariant lastModifiedQv = nwReply->header(QNetworkRequest::LastModifiedHeader);
            if (lastModifiedQv.isValid() && lastModifiedQv.canConvert<QDateTime>())
            {
                return lastModifiedQv.value<QDateTime>();
            }
            return QDateTime();
        }

        qint64 CNetworkUtils::lastModifiedSinceNow(QNetworkReply *nwReply)
        {
            const qint64 sinceEpoch = CNetworkUtils::lastModifiedMsSinceEpoch(nwReply);
            return sinceEpoch > 0 ? std::max(0LL, QDateTime::currentMSecsSinceEpoch() - sinceEpoch) : QDateTime::currentMSecsSinceEpoch();
        }

        int CNetworkUtils::getHttpStatusCode(QNetworkReply *nwReply)
        {
            if (!nwReply) { return -1; }
            const QVariant statusCode = nwReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            if (!statusCode.isValid()) { return -1; }
            const int status = statusCode.toInt();
            return status;
        }

        bool CNetworkUtils::isHttpStatusRedirect(QNetworkReply *nwReply)
        {
            if (!nwReply) { return false; }
            const int code = CNetworkUtils::getHttpStatusCode(nwReply);
            return code == 301 || code == 302 || code == 303 || code == 307;
        }

        QUrl CNetworkUtils::getHttpRedirectUrl(QNetworkReply *nwReply)
        {
            if (!nwReply) { return QUrl(); }
            const QVariant possibleRedirectUrl = nwReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
            if (!possibleRedirectUrl.isValid()) { return QUrl(); }
            QUrl redirectUrl = possibleRedirectUrl.toUrl();
            if (redirectUrl.isRelative())
            {
                redirectUrl = nwReply->url().resolved(redirectUrl);
            }
            return redirectUrl;
        }

        QString CNetworkUtils::removeHtmlPartsFromPhpErrorMessage(const QString &errorMessage)
        {
            if (errorMessage.isEmpty()) { return errorMessage; }
            QString phpError(errorMessage);
            static const QRegularExpression regEx("<[^>]*>");
            return phpError.remove(regEx);
        }
    } // namespace
} // namespacee
