/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/simulation/xplane/xswiftbusconfigwriter.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/stringutils.h"

#include <QFile>
#include <QTextStream>

namespace BlackMisc::Simulation::XPlane
{
    CXSwiftBusConfigWriter::CXSwiftBusConfigWriter(QObject *parent) : QObject(parent)
    { }

    void CXSwiftBusConfigWriter::setDBusAddress(const QString &dBusAddress)
    {
        if (CDBusServer::isSessionOrSystemAddress(dBusAddress)) { m_dbusMode = "session"; }
        else { m_dbusMode = "p2p"; }

        if (m_dbusMode == "p2p")
        {
            CDBusServer::dBusAddressToHostAndPort(dBusAddress, m_dbusAddress, m_dbusPort);
        }

    }

    void CXSwiftBusConfigWriter::setDebugMode(bool on)
    {
        m_debug = on;
    }

    void CXSwiftBusConfigWriter::setTcasEnabled(bool on)
    {
        m_tcas = on;
    }

    void CXSwiftBusConfigWriter::updateInAllXPlaneVersions()
    {
        updateInXPlane9();
        updateInXPlane10();
        updateInXPlane11();
    }

    void CXSwiftBusConfigWriter::updateInXPlane9()
    {
        const QString path = CXPlaneUtil::xswiftbusPluginDir(CXPlaneUtil::xplane9Dir());
        if (!path.isEmpty()) { writeTo(path); }
    }

    void CXSwiftBusConfigWriter::updateInXPlane10()
    {
        const QString path = CXPlaneUtil::xswiftbusPluginDir(CXPlaneUtil::xplane10Dir());
        if (!path.isEmpty()) { writeTo(path); }
    }

    void CXSwiftBusConfigWriter::updateInXPlane11()
    {
        const QString path = CXPlaneUtil::xswiftbusPluginDir(CXPlaneUtil::xplane11Dir());
        if (!path.isEmpty()) { writeTo(path); }
    }

    void CXSwiftBusConfigWriter::writeTo(const QString &filePath)
    {
        // writing the file here (UI side) allows to overwrite the values
        // before XPlane is started
        // TCAS/log values MUST be changed before startup of the plugin
        QString configFilePath = filePath + "/xswiftbus.conf";
        QFile configFile(configFilePath);
        configFile.remove();
        if (configFile.open(QIODevice::WriteOnly))
        {
            // this code should be similar to XSwiftBus config.cpp
            QTextStream ts(&configFile);
            ts << "# DBus Mode - Options: p2p, session" << Qt::endl;
            ts << "dbusMode = " << m_dbusMode << Qt::endl;
            ts << Qt::endl;
            ts << "# DBus server address - relevant for P2P mode only" << Qt::endl;
            ts << "dbusAddress = " << m_dbusAddress << Qt::endl;
            ts << Qt::endl;
            ts << "# DBus server port - relevant for P2P mode only" << Qt::endl;
            ts << "dbusPort = " << m_dbusPort << Qt::endl;
            ts << Qt::endl;
            ts << "# Render phase debugging - to help diagnose crashes" << Qt::endl;
            ts << "debug = " << boolToOnOff(m_debug) << Qt::endl;
            ts << Qt::endl;
            ts << "# TCAS traffic - to disable in case of crashes" << Qt::endl;
            ts << "tcas = " << boolToOnOff(m_tcas) << Qt::endl;

            // add comment as information
            ts << Qt::endl;
            ts << "# Updated by CXSwiftBusConfigWriter " << QDateTime::currentDateTimeUtc().toString("yyyyMMddHHmmss") << " ";
            ts << Qt::endl;
        }
    }
} // ns
