// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup sampleblackmiscdbus

#ifndef BLACKSAMPLE_SERVICETOOL_H
#define BLACKSAMPLE_SERVICETOOL_H

#include <QCoreApplication>
#include <QDBusArgument>
#include <QString>
#include <QStringList>
#include <QtGlobal>

class QDBusConnection;
class QObject;
class QProcess;

namespace BlackMisc
{
    class CDBusServer;
}
namespace BlackSample
{
    class Testservice; // forward declaration

    /*!
     * Supporting / helper functions for running the tests
     */
    class ServiceTool
    {
    public:
        //! Client side of data transfer test
        static void dataTransferTestClient(const QString &address);

        //! Server side of data transfer test
        static void dataTransferTestServer(BlackMisc::CDBusServer *dBusServer, bool verbose);

        //! Loop to send data to test service (slots on server)
        static void sendDataToTestservice(const QDBusConnection &connection);

    private:
        //! No constructor
        ServiceTool() = delete;
    };
} // namespace

#endif // guard
