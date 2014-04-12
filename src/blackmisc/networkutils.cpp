/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "networkutils.h"
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QTcpSocket>
#include <QCoreApplication>
#include <QHostAddress>
#include <QAbstractSocket>

namespace BlackMisc
{
    /*
     * Connected interface?
     */
    bool CNetworkUtils::hasConnectedInterface(bool withDebugOutput)
    {
        // http://stackoverflow.com/questions/2475266/verfiying-the-network-connection-using-qt-4-4
        QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
        bool result = false;

        for (int i = 0; i < interfaces.count(); i++)
        {
            QNetworkInterface iface = interfaces.at(i);

            // details of connection
            if (withDebugOutput) qDebug() << "name:" << iface.name() << endl << "ip addresses:" << endl << "mac:" << iface.hardwareAddress() << endl;
            if (iface.flags().testFlag(QNetworkInterface::IsUp) && !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
            {
                // this loop is important
                for (int j = 0; j < iface.addressEntries().count(); j++)
                {
                    if (withDebugOutput) qDebug() << iface.addressEntries().at(j).ip().toString() << " / " << iface.addressEntries().at(j).netmask().toString() << endl;

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

    /*
     * my IP
     */
    QStringList CNetworkUtils::getKnownIpAddresses()
    {
        QStringList ips;
        if (!CNetworkUtils::hasConnectedInterface(false)) return ips;
        foreach(const QHostAddress & address, QNetworkInterface::allAddresses())
        {
            if (address.isLoopback() || address.isNull()) continue;
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            {
                QString a = address.toString();
                if (CNetworkUtils::isValidIPv4Address(a)) ips.append(a);
            }
        }
        return ips;
    }

    /*
     * Can connect to IP/port?
     */
    bool CNetworkUtils::canConnect(const QString &hostAddress, quint16 port, QString &message, int timeoutMs)
    {
        if (!CNetworkUtils::hasConnectedInterface(false))
        {
            message = QObject::tr("No connected network interface", "BlackMisc");
            return false;
        }

        // http://qt-project.org/forums/viewthread/9346
        QTcpSocket socket;
        bool connected = false;
        socket.connectToHost(hostAddress, port);
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

    /*
     * Can connect server?
     */
    bool CNetworkUtils::canConnect(const Network::CServer &server, QString &message, int timeoutMs)
    {
        return CNetworkUtils::canConnect(server.getAddress(), server.getPort(), message, timeoutMs);
    }

    /*
     * Valid IPv4 address
     */
    bool CNetworkUtils::isValidIPv4Address(const QString &candidate)
    {
        QHostAddress address(candidate);
        return (QAbstractSocket::IPv4Protocol == address.protocol());
    }

    /*
     * Valid IPv6 address
     */
    bool CNetworkUtils::isValidIPv6Address(const QString &candidate)
    {
        QHostAddress address(candidate);
        return (QAbstractSocket::IPv6Protocol == address.protocol());
    }

    /*
     * Valid port?
     */
    bool CNetworkUtils::isValidPort(const QString &port)
    {
        bool success;
        int p = port.toInt(&success);
        if (!success) return false;
        return (p >= 1 && p <= 65535);
    }
} // namespace
