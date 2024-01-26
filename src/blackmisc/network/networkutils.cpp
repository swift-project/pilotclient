// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/network/networkutils.h"
#include "blackmisc/network/server.h"
#include "blackmisc/eventloop.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"
#include <QAbstractSocket>
#include <QDateTime>
#include <QMetaEnum>
#include <QHostAddress>
#include <QList>
#include <QProcess>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QNetworkReply>
#include <QNetworkConfiguration>
#include <QObject>
#include <QSignalMapper>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslKey>
#include <QSslSocket>
#include <QTcpSocket>
#include <QTextStream>
#include <QUrl>
#include <QUrlQuery>
#include <QStringBuilder>
#include <QVariant>
#include <QtDebug>
#include <QRegularExpression>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackMisc::Network
{
    int CNetworkUtils::getTimeoutMs()
    {
        return 5000;
    }

    int CNetworkUtils::getLongTimeoutMs()
    {
        return 3 * getTimeoutMs();
    }

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
        QObject::connect(&socket, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error), &mapper, qOverload<>(&QSignalMapper::map));
        mapper.setMapping(&socket, 0);

        CEventLoop eventLoop;
        eventLoop.stopWhen(&mapper, qOverload<int>(&QSignalMapper::mapped));
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

    void CNetworkUtils::setSwiftUserAgent(QNetworkRequest &request, const QString &userAgentDetails)
    {
        static const QString defaultUserAgent("swift/" + CBuildConfig::getVersionString());

        // User-Agent is known header, we use high level setHeader not setRawHeader
        const QVariant agent = QVariant::fromValue(userAgentDetails.isEmpty() ? defaultUserAgent : defaultUserAgent + "/" + userAgentDetails);
        request.setHeader(QNetworkRequest::UserAgentHeader, agent);
    }

    void CNetworkUtils::addDebugFlag(QUrlQuery &qurl)
    {
        qurl.addQueryItem("XDEBUG_SESSION_START", "ECLIPSE_DBGP");
    }

    QNetworkRequest CNetworkUtils::getSwiftNetworkRequest(const QUrl &url, RequestType type, const QString &userAgentDetails)
    {
        QNetworkRequest request(url);
        switch (type)
        {
        case PostUrlEncoded:
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
            break;
        default:
            break;
        }
        CNetworkUtils::ignoreSslVerification(request);
        CNetworkUtils::setSwiftUserAgent(request, userAgentDetails);
        return request;
    }

    QNetworkRequest CNetworkUtils::getSwiftNetworkRequest(const QNetworkRequest &request, const QString &userAgentDetails)
    {
        QNetworkRequest req(request); // copy
        CNetworkUtils::ignoreSslVerification(req);
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
        return sinceEpoch > 0 ? std::max(0LL, QDateTime::currentMSecsSinceEpoch() - sinceEpoch) : QDateTime::currentMSecsSinceEpoch();
    }

    qint64 CNetworkUtils::requestDuration(const QNetworkReply *nwReply)
    {
        if (!nwReply) { return -1; }
        const QVariant started = nwReply->property("started");
        if (started.isValid() && started.canConvert<qint64>())
        {
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            const qint64 start = started.value<qint64>();
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
        thread_local const QRegularExpression regEx("<[^>]*>");
        return phpError.remove(regEx);
    }

    bool CNetworkUtils::looksLikePhpErrorMessage(const QString &errorMessage)
    {
        if (errorMessage.length() < 50) { return false; }
        if (errorMessage.contains("xdebug", Qt::CaseInsensitive)) { return true; }
        return false;
    }

    CStatusMessageList CNetworkUtils::createNetworkReport(const QNetworkAccessManager *am)
    {
        return CNetworkUtils::createNetworkReport(QUrl(), am);
    }

    CStatusMessageList CNetworkUtils::createNetworkReport(const QUrl &url, const QNetworkAccessManager *qam)
    {
        static const CLogCategoryList cats({ CLogCategories::network() });
        CStatusMessageList msgs;

        if (!url.isEmpty())
        {
            const QString host = url.host();
            const bool canPing = Network::canPing(host);
            const CStatusMessage ping(cats, canPing ? CStatusMessage::SeverityInfo : CStatusMessage::SeverityError, "Host: " + host + " ping: " + boolToYesNo(canPing));
            msgs.push_back(ping);

            QString msg;
            const bool canConnect = CNetworkUtils::canConnect(url, msg, getTimeoutMs() * 2);
            if (canConnect)
            {
                msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityInfo, u"Can connect to " % url.toString()));
            }
            else
            {
                msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"Cannot connect to " % url.toString() % u" msg: " % msg));
            }
        }

        if (qam)
        {
            const QString msg = CNetworkUtils::createNetworkAccessManagerReport(qam);
            const bool accessible = qam->networkAccessible() == QNetworkAccessManager::Accessible;
            msgs.push_back(CStatusMessage(cats, accessible ? CStatusMessage::SeverityInfo : CStatusMessage::SeverityError, msg));
        }

        return msgs;
    }

    QString CNetworkUtils::createNetworkConfigurationReport(const QNetworkConfigurationManager *qcm, const QNetworkAccessManager *qam, const QString &separator)
    {
        if (!qcm) { return QStringLiteral("No configuration manager"); }

        static const QString empty;
        QString report;
        int c = 0;

        int active = 0;
        int inActive = 0;
        int valid = 0;
        for (const QNetworkConfiguration &config : qcm->allConfigurations())
        {
            if (config.state() == QNetworkConfiguration::Active) { active++; }
            else { inActive++; }
            if (config.isValid()) { valid++; }

            report +=
                (report.isEmpty() ? empty : separator) %
                QString::number(++c) % u": " %
                CNetworkUtils::networkConfigurationToString(config);
        }

        if (c < 1)
        {
            report = QStringLiteral("No network configurations!");
        }
        else
        {
            static const QString count("Network configurations: active %1 / inactive %2 / valid %3");
            report +=
                (report.isEmpty() ? empty : separator) %
                count.arg(active).arg(inActive).arg(valid);
        }

        if (qam)
        {
            report +=
                (report.isEmpty() ? empty : separator) %
                u"QAM: " %
                CNetworkUtils::createNetworkAccessManagerReport(qam);
        }

        return report;
    }

    QString CNetworkUtils::createNetworkAccessManagerReport(const QNetworkAccessManager *qam)
    {
        static const QMetaEnum enumAccessible = QMetaEnum::fromType<QNetworkAccessManager::NetworkAccessibility>();
        static const QString info("Accessible: '%1' (%2) config: %3");

        const bool accessible = qam->networkAccessible() == QNetworkAccessManager::Accessible;
        return info.arg(boolToYesNo(accessible), enumAccessible.valueToKey(qam->networkAccessible()), CNetworkUtils::networkConfigurationToString(qam->configuration()));
    }

    QString CNetworkUtils::networkConfigurationToString(const QNetworkConfiguration &configuration)
    {
        static const QString s("'%1' '%2' valid: '%3' '%4' '%5'");
        const QString stateFlagsStr = networkStatesToString(configuration.state());
        return s.arg(configuration.name(), configuration.identifier(), boolToYesNo(configuration.isValid()), stateFlagsStr, networkTypeToString(configuration.type()));
    }

    const QString &CNetworkUtils::networkTypeToString(QNetworkConfiguration::Type type)
    {
        static const QString iap("InternetAccessPoint");
        static const QString sn("ServiceNetwork");
        static const QString i("Invalid");
        static const QString uc("UserChoice");

        switch (type)
        {
        case QNetworkConfiguration::InternetAccessPoint: return iap;
        case QNetworkConfiguration::ServiceNetwork: return sn;
        case QNetworkConfiguration::UserChoice: return uc;
        default:
        case QNetworkConfiguration::Invalid: break;
        }

        return i;
    }

    const QString &CNetworkUtils::networkStateToString(QNetworkConfiguration::StateFlag state)
    {
        static const QString disco("Discovered");
        static const QString a("Active");
        static const QString u("Undefined");
        static const QString d("Defined");

        switch (state)
        {
        case QNetworkConfiguration::Defined: return d;
        case QNetworkConfiguration::Active: return a;
        case QNetworkConfiguration::Discovered: return disco;
        default:
        case QNetworkConfiguration::Undefined: break;
        }

        return u;
    }

    QString CNetworkUtils::networkStatesToString(QNetworkConfiguration::StateFlags states)
    {
        QStringList statesSl;
        if (states.testFlag(QNetworkConfiguration::Active)) { statesSl << networkStateToString(QNetworkConfiguration::Active); }
        if (states.testFlag(QNetworkConfiguration::Discovered)) { statesSl << networkStateToString(QNetworkConfiguration::Discovered); }
        if (states.testFlag(QNetworkConfiguration::Defined)) { statesSl << networkStateToString(QNetworkConfiguration::Defined); }
        if (states.testFlag(QNetworkConfiguration::Undefined)) { statesSl << networkStateToString(QNetworkConfiguration::Undefined); }
        return statesSl.join(", ");
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
        default:
            break;
        }
        return u;
    }
} // namespace
