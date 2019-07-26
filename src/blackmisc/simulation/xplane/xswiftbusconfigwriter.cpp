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

#include <QFile>
#include <QTextStream>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace XPlane
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

            void CXSwiftBusConfigWriter::updateInAllXPlaneVersions()
            {
                updateInXPlane9();
                updateInXPlane10();
                updateInXPlane11();
            }

            void CXSwiftBusConfigWriter::updateInXPlane9()
            {
                QString path = CXPlaneUtil::xswiftbusPluginDir(CXPlaneUtil::xplane9Dir());
                if (!path.isEmpty()) { writeTo(path); }
            }

            void CXSwiftBusConfigWriter::updateInXPlane10()
            {
                QString path = CXPlaneUtil::xswiftbusPluginDir(CXPlaneUtil::xplane10Dir());
                if (!path.isEmpty()) { writeTo(path); }
            }

            void CXSwiftBusConfigWriter::updateInXPlane11()
            {
                QString path = CXPlaneUtil::xswiftbusPluginDir(CXPlaneUtil::xplane11Dir());
                if (!path.isEmpty()) { writeTo(path); }
            }

            void CXSwiftBusConfigWriter::writeTo(const QString &filePath)
            {
                QString configFilePath = filePath + "/xswiftbus.conf";
                QFile configFile(configFilePath);
                configFile.remove();
                if (configFile.open(QIODevice::WriteOnly))
                {
                    QTextStream ts(&configFile);
                    ts << "# DBus Mode - Options: p2p, session" << endl;
                    ts << "dbusMode = " << m_dbusMode << endl;
                    ts << endl;
                    ts << "# DBus server address - relevant for P2P mode only" << endl;
                    ts << "dbusAddress = " << m_dbusAddress << endl;
                    ts << endl;
                    ts << "# DBus server port - relevant for P2P mode only" << endl;
                    ts << "dbusPort = " << m_dbusPort << endl;
                }
            }
        } // ns
    } // ns
} // ns
