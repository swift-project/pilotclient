// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_DATA_NETWORKSETUP_H
#define BLACKCORE_DATA_NETWORKSETUP_H

#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/data/vatsimsetup.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/network/data/lastserver.h"
#include "blackmisc/network/settings/serversettings.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/metaclass.h"

#include <QMetaType>
#include <QString>
#include <QObject>

namespace BlackCore::Data
{
    //! Remembering the last servers and ecosystem.
    class BLACKCORE_EXPORT CNetworkSetup : public QObject
    {
        Q_OBJECT

    public:
        //! Default constructor
        CNetworkSetup();

        //! Destructor.
        virtual ~CNetworkSetup() {}

        //! Last VATSIM server (VATSIM only)
        BlackMisc::Network::CServer getLastVatsimServer() const;

        //! Set value of last VATSIM server
        BlackMisc::CStatusMessage setLastVatsimServer(const BlackMisc::Network::CServer &server);

        //! Last server (all networks)
        BlackMisc::Network::CServer getLastServer() const;

        //! Set value of last server
        BlackMisc::CStatusMessage setLastServer(const BlackMisc::Network::CServer &server);

        //! Last used eco system
        BlackMisc::Network::CEcosystem getLastEcosystem() const;

        //! The other servers
        BlackMisc::Network::CServerList getOtherServers() const;

        //! The other servers plus test servers
        BlackMisc::Network::CServerList getOtherServersPlusPredefinedServers() const;

        //! Last used with VATSIM?
        bool wasLastUsedWithVatsim() const;

        //! Used with an other server (i.e. non VATSIM)
        bool wasLastUsedWithOtherServer() const;

    signals:
        //! Setup changed
        void setupChanged();

    private:
        //! Settings have been changed
        void onSettingsChanged();

        BlackMisc::CSettingReadOnly<BlackMisc::Network::Settings::TTrafficServers> m_otherTrafficNetworkServers { this, &CNetworkSetup::onSettingsChanged };
        BlackMisc::CData<BlackMisc::Network::Data::TLastServer> m_lastServer { this, &CNetworkSetup::onSettingsChanged }; //!< recently used server (VATSIM, other)
        BlackMisc::CData<BlackCore::Data::TVatsimLastServer> m_lastVatsimServer { this, &CNetworkSetup::onSettingsChanged }; //!< recently used VATSIM server
    };
} // ns

#endif // guard
