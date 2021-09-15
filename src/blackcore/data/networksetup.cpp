/* Copyright (C) 2018
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/data/networksetup.h"
#include "blackcore/application.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore::Data
{
    CNetworkSetup::CNetworkSetup()
    { }

    CServer CNetworkSetup::getLastVatsimServer() const
    {
        return m_lastVatsimServer.get();
    }

    CStatusMessage CNetworkSetup::setLastVatsimServer(const CServer &server)
    {
        return m_lastVatsimServer.set(server);
    }

    CStatusMessage CNetworkSetup::setAutoLogoff(bool autoLogoff)
    {
        return m_autoLogoff.setAndSave(autoLogoff);
    }

    CServer CNetworkSetup::getLastServer() const
    {
        return m_lastServer.get();
    }

    CStatusMessage CNetworkSetup::setLastServer(const CServer &server)
    {
        return m_lastServer.set(server);
    }

    CServerList CNetworkSetup::getOtherServers() const
    {
        return m_otherTrafficNetworkServers.get();
    }

    CServerList CNetworkSetup::getOtherServersPlusPredefinedServers() const
    {
        // add a testserver when no servers can be loaded
        CServerList otherServers(this->getOtherServers());
        if (sApp)
        {
            if (otherServers.isEmpty() && CBuildConfig::isLocalDeveloperDebugBuild())
            {
                otherServers.addIfAddressNotExists(sApp->getGlobalSetup().getPredefinedServersPlusHardcodedServers());
            }
            else
            {
                otherServers.addIfAddressNotExists(sApp->getGlobalSetup().getPredefinedServers());
            }
        }
        return otherServers;
    }

    CEcosystem CNetworkSetup::getLastEcosystem() const
    {
        return this->getLastServer().getEcosystem();
    }

    bool CNetworkSetup::wasLastUsedWithVatsim() const
    {
        return (this->getLastEcosystem() == CEcosystem::vatsim());
    }

    bool CNetworkSetup::wasLastUsedWithOtherServer() const
    {
        const CServer server(this->getLastServer());
        if (server.isNull()) { return false; }
        return server.getEcosystem() == CEcosystem::privateFsd() ||
                server.getEcosystem() == CEcosystem::swiftTest() ||
                server.getEcosystem() == CEcosystem::swift();
    }

    void CNetworkSetup::onSettingsChanged()
    {
        emit this->setupChanged();
    }
} // ns
