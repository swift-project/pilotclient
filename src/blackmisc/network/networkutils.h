/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORKUTILS_H
#define BLACKMISC_NETWORKUTILS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/network/url.h"

#include <QByteArray>
#include <QHttpPart>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QString>
#include <QStringList>
#include <QtGlobal>

class QHttpPart;
class QNetworkReply;
class QUrl;
class QUrlQuery;

namespace BlackMisc
{
    namespace Network
    {
        class CServer;

        //! Utilities, e.g. checking whether a network connection can be established
        class BLACKMISC_EXPORT CNetworkUtils
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

            //! Is a connected interface available?
            //! \param withDebugOutput enables some debugging output
            //! \return
            static bool hasConnectedInterface(bool withDebugOutput = false);

            //! Can connect?
            //! \param hostAddress   130.4.20.3, or myserver.com
            //! \param port          80, 1234
            //! \param timeoutMs
            //! \param message       human readable message
            //! \return
            static bool canConnect(const QString &hostAddress, int port, QString &message, int timeoutMs = getTimeoutMs());

            //! Can connect to server?
            //! \param server
            //! \param message       human readable message
            //! \param timeoutMs
            //! \return
            static bool canConnect(const BlackMisc::Network::CServer &server, QString &message, int timeoutMs = getTimeoutMs());

            //! Can connect to URL?
            static bool canConnect(const QString &url, QString &message, int timeoutMs = getTimeoutMs());

            //! Can connect to URL?
            static bool canConnect(const QUrl &url, QString &message, int timeoutMs = getTimeoutMs());

            //! Can connect to URL?
            static bool canConnect(const QUrl &url, int timeoutMs = getTimeoutMs());

            //! Can connect to URL?
            static bool canConnect(const BlackMisc::Network::CUrl &url, QString &message, int timeoutMs = getTimeoutMs());

            //! Can connect to URL?
            static bool canConnect(const BlackMisc::Network::CUrl &url, int timeoutMs = getTimeoutMs());

            //! Find out my IPv4 address, empty if not possible
            static QStringList getKnownIpAddresses();

            //! Valid IPv4 address
            static bool isValidIPv4Address(const QString &candidate);

            //! Valid IPv6 address
            static bool isValidIPv6Address(const QString &candidate);

            //! Valid port
            static bool isValidPort(const QString &port);

            //! Build / concatenate an URL
            static QString buildUrl(const QString &protocol, const QString &server, const QString &baseUrl, const QString &serviceUrl);

            //! Ignore SSL verification such as self signed certificates
            static void ignoreSslVerification(QNetworkRequest &request);

            //! Set user agent for request
            static void setSwiftUserAgent(QNetworkRequest &request);

            //! Multipart with DEBUG FLAG for server
            static QHttpPart getMultipartWithDebugFlag();

            //! Add debug flag
            static void addDebugFlag(QUrlQuery &qurl);

            //! Multipart for JSON
            static QHttpPart getJsonTextMultipart(const QJsonObject &json);

            //! Multipart for JSON
            static QHttpPart getJsonTextMultipart(const QJsonArray &json);

            //! Multipart for JSON
            static QHttpPart getJsonTextMultipart(const QByteArray &bytes);

            //! Our tweaked network request
            static QNetworkRequest getNetworkRequest(const CUrl &url, RequestType type = Get);

            //! Last modified from reply
            static qint64 lastModifiedMsSinceEpoch(QNetworkReply *nwReply);

            //! Remove the HTML formatting from a PHP error message
            static QString removeHtmlPartsFromPhpErrorMessage(const QString &errorMessage);

        private:
            //! Hidden constructor
            CNetworkUtils() {}
        };
    } // namespace
} // namespace

#endif // guard
