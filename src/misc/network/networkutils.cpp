// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/networkutils.h"

#include <QAbstractSocket>
#include <QDateTime>
#include <QHostAddress>
#include <QList>
#include <QMetaEnum>
#include <QNetworkInterface>
#include <QNetworkReply>
#include <QObject>
#include <QRegularExpression>
#include <QSignalMapper>
#include <QStringBuilder>
#include <QTcpSocket>
#include <QUrl>
#include <QUrlQuery>
#include <QVariant>

#include "config/buildconfig.h"
#include "misc/eventloop.h"
#include "misc/network/server.h"
#include "misc/stringutils.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::network;

namespace swift::misc::network
{
    int CNetworkUtils::getTimeoutMs() { return 5000; }

    int CNetworkUtils::getLongTimeoutMs() { return 3 * getTimeoutMs(); }

    QStringList CNetworkUtils::getKnownLocalIpV4Addresses()
    {
        QStringList ips;
        const QList<QHostAddress> allAddresses = QNetworkInterface::allAddresses();
        for (const QHostAddress &address : allAddresses)
        {
            if (address.isNull()) { continue; }
            if (address.protocol() == QAbstractSocket::IPv4Protocol)
            {
                const QString a = address.toString();
                ips.append(a);
            }
        }
        ips.sort();
        return ips;
    }

    bool CNetworkUtils::canConnect(const QString &hostAddress, int port, QString &message, int timeoutMs)
    {
        if (timeoutMs < 0) { timeoutMs = getTimeoutMs(); } // external functions might call with -1
        QTcpSocket socket;
        QSignalMapper mapper;
        QObject::connect(&socket, &QTcpSocket::connected, &mapper, qOverload<>(&QSignalMapper::map));
        QObject::connect(&socket, &QAbstractSocket::errorOccurred, &mapper, qOverload<>(&QSignalMapper::map));
        mapper.setMapping(&socket, 0);

        CEventLoop eventLoop;
        eventLoop.stopWhen(&mapper, &QSignalMapper::mappedInt);
        socket.connectToHost(hostAddress, static_cast<quint16>(port));
        const bool timedOut = !eventLoop.exec(timeoutMs);

        if (socket.state() != QTcpSocket::ConnectedState)
        {
            static const QString e("Connection failed: '%1'");
            message = timedOut ? e.arg("Timed out") : e.arg(socket.errorString());
            return false;
        }
        else
        {
            static const QString ok("OK, connected");
            message = ok;
            return true;
        }
    }

    bool CNetworkUtils::canConnect(const network::CServer &server, QString &message, int timeoutMs)
    {
        return CNetworkUtils::canConnect(server.getAddress(), server.getPort(), message, timeoutMs);
    }

    bool CNetworkUtils::canConnect(const QString &url, QString &message, int timeoutMs)
    {
        if (url.isEmpty())
        {
            message = QObject::tr("Missing URL", "Misc");
            return false;
        }
        return canConnect(QUrl(url), message, timeoutMs);
    }

    bool CNetworkUtils::canConnect(const QUrl &url, QString &message, int timeoutMs)
    {
        if (!url.isValid())
        {
            message = QObject::tr("Invalid URL: %1", "Misc").arg(url.toString());
            return false;
        }

        if (url.isRelative())
        {
            message = QObject::tr("Relative URL cannot be tested: %1", "Misc").arg(url.toString());
            return false;
        }

        const QString host(url.host());
        const QString scheme(url.scheme().toLower());
        int p = url.port();
        if (p < 0) { p = scheme.contains("https") ? 443 : 80; }
        return canConnect(host, p, message, timeoutMs);
    }

    bool CNetworkUtils::canConnect(const QUrl &url, int timeoutMs)
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

    QString CNetworkUtils::buildUrl(const QString &protocol, const QString &server, const QString &baseUrl,
                                    const QString &serviceUrl)
    {
        Q_ASSERT_X(protocol.length() > 3, Q_FUNC_INFO, "worng protocol");
        Q_ASSERT_X(!server.isEmpty(), Q_FUNC_INFO, "missing server");
        Q_ASSERT_X(!serviceUrl.isEmpty(), Q_FUNC_INFO, "missing service URL");

        QString url(server);
        if (!baseUrl.isEmpty()) { url.append("/").append(baseUrl); }
        url.append("/").append(serviceUrl);
        url.replace("//", "/");
        return protocol + "://" + url;
    }

    void CNetworkUtils::setSwiftUserAgent(QNetworkRequest &request, const QString &userAgentDetails)
    {
        static const QString defaultUserAgent("swift/" + CBuildConfig::getVersionString());

        // User-Agent is known header, we use high level setHeader not setRawHeader
        const QVariant agent = QVariant::fromValue(
            userAgentDetails.isEmpty() ? defaultUserAgent : defaultUserAgent + "/" + userAgentDetails);
        request.setHeader(QNetworkRequest::UserAgentHeader, agent);
    }

    void CNetworkUtils::addDebugFlag(QUrlQuery &qurl) { qurl.addQueryItem("XDEBUG_SESSION_START", "ECLIPSE_DBGP"); }

    QNetworkRequest CNetworkUtils::getSwiftNetworkRequest(const QUrl &url, RequestType type,
                                                          const QString &userAgentDetails)
    {
        QNetworkRequest request(url);
        switch (type)
        {
        case PostUrlEncoded:
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            break;
        default: break;
        }
        CNetworkUtils::setSwiftUserAgent(request, userAgentDetails);
        return request;
    }

    QNetworkRequest CNetworkUtils::getSwiftNetworkRequest(const QNetworkRequest &request,
                                                          const QString &userAgentDetails)
    {
        QNetworkRequest req(request); // copy
        CNetworkUtils::setSwiftUserAgent(req, userAgentDetails);
        return req;
    }

    qint64 CNetworkUtils::lastModifiedMsSinceEpoch(const QNetworkReply *nwReply)
    {
        Q_ASSERT(nwReply);
        const QDateTime lm = CNetworkUtils::lastModifiedDateTime(nwReply);
        return lm.isValid() ? lm.toMSecsSinceEpoch() : -1;
    }

    QDateTime CNetworkUtils::lastModifiedDateTime(const QNetworkReply *nwReply)
    {
        Q_ASSERT(nwReply);
        const QVariant lastModifiedQv = nwReply->header(QNetworkRequest::LastModifiedHeader);
        if (lastModifiedQv.isValid() && lastModifiedQv.canConvert<QDateTime>())
        {
            return lastModifiedQv.value<QDateTime>();
        }
        return QDateTime();
    }

    qint64 CNetworkUtils::lastModifiedSinceNow(const QNetworkReply *nwReply)
    {
        const qint64 sinceEpoch = CNetworkUtils::lastModifiedMsSinceEpoch(nwReply);
        return sinceEpoch > 0 ? std::max(0LL, QDateTime::currentMSecsSinceEpoch() - sinceEpoch) :
                                QDateTime::currentMSecsSinceEpoch();
    }

    qint64 CNetworkUtils::requestDuration(const QNetworkReply *nwReply)
    {
        if (!nwReply) { return -1; }
        const QVariant started = nwReply->property("started");
        if (started.isValid() && started.canConvert<qint64>())
        {
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            const auto start = started.value<qint64>();
            return (now - start);
        }
        return -1;
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
        if (redirectUrl.isRelative()) { redirectUrl = nwReply->url().resolved(redirectUrl); }
        return redirectUrl;
    }

    QString CNetworkUtils::removeHtmlPartsFromPhpErrorMessage(const QString &errorMessage)
    {
        if (errorMessage.isEmpty()) { return errorMessage; }
        QString phpError(errorMessage);
        thread_local const QRegularExpression regEx("<[^>]*>");
        return phpError.remove(regEx);
    }

    bool CNetworkUtils::looksLikePhpErrorMessage(const QString &errorMessage)
    {
        if (errorMessage.length() < 50) { return false; }
        if (errorMessage.contains("xdebug", Qt::CaseInsensitive)) { return true; }
        return false;
    }

    const QString &CNetworkUtils::networkOperationToString(QNetworkAccessManager::Operation operation)
    {
        static const QString h("HEAD");
        static const QString g("GET");
        static const QString put("PUT");
        static const QString d("DELETE");
        static const QString post("POST");
        static const QString c("custom");
        static const QString u("unknown");

        switch (operation)
        {
        case QNetworkAccessManager::HeadOperation: return h;
        case QNetworkAccessManager::GetOperation: return g;
        case QNetworkAccessManager::PutOperation: return put;
        case QNetworkAccessManager::PostOperation: return post;
        case QNetworkAccessManager::DeleteOperation: return d;
        case QNetworkAccessManager::CustomOperation: return c;
        case QNetworkAccessManager::UnknownOperation:
        default: break;
        }
        return u;
    }
} // namespace swift::misc::network
