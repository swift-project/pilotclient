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

#include "network/server.h"
#include <QStringList>

namespace BlackMisc
{

    //! Utilities, e.g. checking whether a network connection can be established
    class CNetworkUtils
    {

    public:
        /*!
         * Is a connected interface available?
         * \param withDebugOutput enables some debugging output
         * \return
         */
        static bool hasConnectedInterface(bool withDebugOutput = false);

        /*!
         * Can connect?
         * \param hostAddress   130.4.20.3, or myserver.com
         * \param port          80, 1234
         * \param timeoutMs
         * \param message       human readable message
         * \return
         */
        static bool canConnect(const QString &hostAddress, int port, QString &message, int timeoutMs = 1500);

        /*!
         * Can connect to server?
         * \param server
         * \param message       human readable message
         * \param timeoutMs
         * \return
         */
        static bool canConnect(const BlackMisc::Network::CServer &server, QString &message, int timeoutMs = 1500);

        //! Find out my IPv4 address, empty if not possible
        static QStringList getKnownIpAddresses();

        //! Valid IPv4 address
        static bool isValidIPv4Address(const QString &candidate);

        //! Valid IPv6 address
        static bool isValidIPv6Address(const QString &candidate);

        //! Valid port
        static bool isValidPort(const QString &port);

    private:
        //! Deleted constructor
        CNetworkUtils() {}

    };
} // namespace

#endif // guard

