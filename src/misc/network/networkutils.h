// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORKUTILS_H
#define SWIFT_MISC_NETWORKUTILS_H

#include "misc/swiftmiscexport.h"
#include "misc/statusmessagelist.h"
#include "misc/network/ping.h"

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QString>
#include <QStringList>
#include <QtGlobal>

class QHttpPart;
class QNetworkReply;
class QUrlQuery;

namespace swift::misc::network
{
    class CServer;

    //! Utilities, e.g. checking whether a network connection can be established
    class SWIFT_MISC_EXPORT CNetworkUtils
    {
    public:
        //! Request type
        enum RequestType
        {
            Get,
            PostUrlEncoded,
            Multipart
        };

        //! Default for timeout
        static int getTimeoutMs();

        //! Default for timeout (long)
        static int getLongTimeoutMs();

        //! Can connect?
        //! \param hostAddress   130.4.20.3, or myserver.com
        //! \param port          80, 1234
        //! \param timeoutMs
        //! \param message       human readable message
        static bool canConnect(const QString &hostAddress, int port, QString &message, int timeoutMs = getTimeoutMs());

        //! Can connect to server?
        //! \param server
        //! \param message       human readable message
        //! \param timeoutMs
        static bool canConnect(const swift::misc::network::CServer &server, QString &message, int timeoutMs = getTimeoutMs());

        //! Can connect to URL?
        static bool canConnect(const QString &url, QString &message, int timeoutMs = getTimeoutMs());

        //! Can connect to URL?
        static bool canConnect(const QUrl &url, QString &message, int timeoutMs = getTimeoutMs());

        //! Can connect to URL?
        static bool canConnect(const QUrl &url, int timeoutMs = getTimeoutMs());

        //! Find out my IPv4 addresses including loopback, empty if not possible
        static QStringList getKnownLocalIpV4Addresses();

        //! Valid IPv4 address
        static bool isValidIPv4Address(const QString &candidate);

        //! Valid IPv6 address
        static bool isValidIPv6Address(const QString &candidate);

        //! Valid port
        static bool isValidPort(const QString &port);

        //! Build / concatenate an URL
        static QString buildUrl(const QString &protocol, const QString &server, const QString &baseUrl, const QString &serviceUrl);

        //! Set user agent for request
        static void setSwiftUserAgent(QNetworkRequest &request, const QString &userAgentDetails = {});

        //! Add debug flag
        static void addDebugFlag(QUrlQuery &qurl);

        //! Our tweaked network request with swift header
        static QNetworkRequest getSwiftNetworkRequest(const QUrl &url, RequestType type = Get, const QString &userAgentDetails = {});

        //! Get a copied network request with swift header
        static QNetworkRequest getSwiftNetworkRequest(const QNetworkRequest &request, const QString &userAgentDetails = {});

        //! Last modified from reply
        static qint64 lastModifiedMsSinceEpoch(const QNetworkReply *nwReply);

        //! Last modified from reply
        static QDateTime lastModifiedDateTime(const QNetworkReply *nwReply);

        //! Last modified from reply compared with now (in ms)
        static qint64 lastModifiedSinceNow(const QNetworkReply *nwReply);

        //! Request duration (only works if requested by swift functions)
        static qint64 requestDuration(const QNetworkReply *nwReply);

        //! Get the http status code
        static int getHttpStatusCode(QNetworkReply *nwReply);

        //! Is the reply an HTTP redirect?
        //! \details Status codes:
        //! - 301: Permanent redirect. Clients making subsequent requests for this resource should use the new URI. Clients should not follow the redirect automatically for POST/PUT/DELETE requests.
        //! - 302: Redirect for undefined reason. Clients making subsequent requests for this resource should not use the new URI. Clients should not follow the redirect automatically for POST/PUT/DELETE requests.
        //! - 303: Redirect for undefined reason. Typically, 'Operation has completed, continue elsewhere.' Clients making subsequent requests for this resource should not use the new URI. Clients should follow the redirect for POST/PUT/DELETE requests.
        //! - 307: Temporary redirect. Resource may return to this location at a later point. Clients making subsequent requests for this resource should use the old URI. Clients should not follow the redirect automatically for POST/PUT/DELETE requests.
        static bool isHttpStatusRedirect(QNetworkReply *nwReply);

        //! Get the redirect URL if any
        static QUrl getHttpRedirectUrl(QNetworkReply *nwReply);

        //! Remove the HTML formatting from a PHP error message
        static QString removeHtmlPartsFromPhpErrorMessage(const QString &errorMessage);

        //! Looks like PHP error messages
        static bool looksLikePhpErrorMessage(const QString &errorMessage);

        //! Operation to string
        static const QString &networkOperationToString(QNetworkAccessManager::Operation operation);

    private:
        //! Hidden constructor
        CNetworkUtils() {}
    };
} // namespace

#endif // guard
