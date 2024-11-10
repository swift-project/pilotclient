// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/data/networksetup.h"
#include "blackcore/application.h"
#include "config/buildconfig.h"

using namespace swift::config;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackCore::Data
{
    CNetworkSetup::CNetworkSetup()
    {}

    CServer CNetworkSetup::getLastVatsimServer() const
    {
        return m_lastVatsimServer.get();
    }

    CStatusMessage CNetworkSetup::setLastVatsimServer(const CServer &server)
    {
        return m_lastVatsimServer.set(server);
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
