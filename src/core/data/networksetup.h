// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DATA_NETWORKSETUP_H
#define SWIFT_CORE_DATA_NETWORKSETUP_H

#include "core/vatsim/vatsimsettings.h"
#include "core/data/vatsimsetup.h"
#include "core/swiftcoreexport.h"
#include "misc/network/data/lastserver.h"
#include "misc/network/settings/serversettings.h"
#include "misc/network/serverlist.h"
#include "misc/statusmessage.h"
#include "misc/settingscache.h"
#include "misc/metaclass.h"

#include <QMetaType>
#include <QString>
#include <QObject>

namespace swift::core::data
{
    //! Remembering the last servers and ecosystem.
    class SWIFT_CORE_EXPORT CNetworkSetup : public QObject
    {
        Q_OBJECT

    public:
        //! Default constructor
        CNetworkSetup();

        //! Destructor.
        virtual ~CNetworkSetup() {}

        //! Last VATSIM server (VATSIM only)
        swift::misc::network::CServer getLastVatsimServer() const;

        //! Set value of last VATSIM server
        swift::misc::CStatusMessage setLastVatsimServer(const swift::misc::network::CServer &server);

        //! Last server (all networks)
        swift::misc::network::CServer getLastServer() const;

        //! Set value of last server
        swift::misc::CStatusMessage setLastServer(const swift::misc::network::CServer &server);

        //! Last used eco system
        swift::misc::network::CEcosystem getLastEcosystem() const;

        //! The other servers
        swift::misc::network::CServerList getOtherServers() const;

        //! The other servers plus test servers
        swift::misc::network::CServerList getOtherServersPlusPredefinedServers() const;

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

        swift::misc::CSettingReadOnly<swift::misc::network::settings::TTrafficServers> m_otherTrafficNetworkServers { this, &CNetworkSetup::onSettingsChanged };
        swift::misc::CData<swift::misc::network::data::TLastServer> m_lastServer { this, &CNetworkSetup::onSettingsChanged }; //!< recently used server (VATSIM, other)
        swift::misc::CData<swift::core::data::TVatsimLastServer> m_lastVatsimServer { this, &CNetworkSetup::onSettingsChanged }; //!< recently used VATSIM server
    };
} // ns

#endif // guard
