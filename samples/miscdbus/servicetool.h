// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscdbus

#ifndef SWIFT_SAMPLE_SERVICETOOL_H
#define SWIFT_SAMPLE_SERVICETOOL_H

#include <QCoreApplication>
#include <QDBusArgument>
#include <QString>
#include <QStringList>
#include <QtGlobal>

class QDBusConnection;
class QObject;
class QProcess;

namespace swift::misc
{
    class CDBusServer;
}
namespace swift::sample
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
        static void dataTransferTestServer(swift::misc::CDBusServer *dBusServer, bool verbose);

        //! Loop to send data to test service (slots on server)
        static void sendDataToTestservice(const QDBusConnection &connection);

    private:
        //! No constructor
        ServiceTool() = delete;
    };
} // namespace swift::sample

#endif // guard
