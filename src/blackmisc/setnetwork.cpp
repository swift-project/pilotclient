/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "setnetwork.h"
#include "blackcore/dbus_server.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/settingutilities.h"
#include "blackmisc/variant.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Settings
    {
        /*
         * Constructor
         */
        CSettingsNetwork::CSettingsNetwork() :
            m_bookingServiceUrl("http://vatbook.euroutepro.com/xml2.php"),
            m_dbusServerAddress(BlackCore::CDBusServer::sessionDBusServer())
        {
            // settings
        }

        bool CSettingsNetwork::setCurrentNetworkServer(const CServer &currentServer)
        {
            if (this->m_trafficNetworkServerCurrent == currentServer) return false;
            m_trafficNetworkServerCurrent = currentServer;
            return true;
        }

        /*
         * Convert to string
         */
        QString CSettingsNetwork::convertToQString(bool i18n) const
        {
            QString s("Traffic servers:");
            s.append(" ").append(this->m_trafficNetworkServers.toQString(i18n));
            return s;
        }

        /*
         * Default values
         */
        void CSettingsNetwork::initDefaultValues()
        {
            this->m_trafficNetworkServers.clear();
            this->addTrafficNetworkServer(CServer("Testserver", "Client project testserver", "vatsim-germany.org", 6809, CUser("guest", "Guest Client project", "", "guest")));
        }

        /*
         * Value
         */
        BlackMisc::CStatusMessage CSettingsNetwork::value(const QString &path, const QString &command, const CVariant &value, bool &changedFlag)
        {
            // TODO: This needs to be refactored to a smarter way to delegate commands
            changedFlag = false;
            if (path == CSettingsNetwork::ValueTrafficServers())
            {
                const CServer server = value.value<CServer>();
                if (command == CSettingsNetwork::CmdSetCurrentServer())
                {
                    changedFlag = this->setCurrentNetworkServer(server);
                    if (changedFlag)
                    {
                        // make sure the server is correct int the list too
                        this->m_trafficNetworkServers.replaceOrAdd(&CServer::getName, server.getName(), server);
                    }
                }
                else if (command == CSettingUtilities::CmdUpdate())
                {
                    this->m_trafficNetworkServers.replaceOrAdd(&CServer::getName, server.getName(), server);
                    changedFlag = true;
                }
                else if (command == CSettingUtilities::CmdRemove())
                {
                    changedFlag = this->m_trafficNetworkServers.contains(&CServer::getName, server.getName());
                    this->m_trafficNetworkServers.removeIf(&CServer::getName, server.getName());
                }
                return CLogMessage(CLogCategory::settingsUpdate()).info("Set current server");
            }
            else if (path == CSettingsNetwork::ValueBookingServiceUrl())
            {
                if (command == CSettingUtilities::CmdUpdate())
                {
                    QString v = value.toQString();
                    changedFlag = (v != this->m_bookingServiceUrl);
                    this->m_bookingServiceUrl = v;
                    return CLogMessage(CLogCategory::settingsUpdate()).info("booking URL%1 changed") << (changedFlag ? "" : " not");
                }
            }
            return CLogMessage(CLogCategory::validation()).error("wrong path: %1") << path;
        }
    } // namespace
} // namespace
