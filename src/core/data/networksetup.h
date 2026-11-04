// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_DATA_NETWORKSETUP_H
#define SWIFT_CORE_DATA_NETWORKSETUP_H

#include <QObject>
#include <QString>

#include "core/data/vatsimsetup.h"
#include "core/swiftcoreexport.h"
#include "core/vatsim/vatsimsettings.h"
#include "misc/network/data/lastserver.h"
#include "misc/network/serverlist.h"
#include "misc/network/settings/serversettings.h"
#include "misc/settingscache.h"
#include "misc/statusmessage.h"

namespace swift::core::data
{
    //! Trait for the last selected network tab (VATSIM or other servers), persisted independently
    //! from an actual successful login so the UI can restore the tab the user looked at last.
    struct TLastServerTabOther : public swift::misc::TDataTrait<bool>
    {
        //! \copydoc swift::misc::TDataTrait::key
        static const char *key() { return "lastservertabother"; }

        //! \copydoc swift::misc::TDataTrait::isPinned
        static constexpr bool isPinned() { return true; }

        //! \copydoc swift::misc::TDataTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Last selected network tab was other servers");
            return name;
        }
    };

    //! Remembering the last servers and ecosystem.
    class SWIFT_CORE_EXPORT CNetworkSetup : public QObject
    {
        Q_OBJECT

    public:
        //! Default constructor
        CNetworkSetup() = default;

        //! Destructor.
        ~CNetworkSetup() override = default;

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

        //! Was the "Other Servers" tab the last one selected in the UI (independent of login)?
        bool wasLastSelectedTabOtherServer() const;

        //! Persist which network tab was last selected in the UI (VATSIM or other servers)
        swift::misc::CStatusMessage setLastSelectedTabOtherServer(bool otherServerTabSelected);

    signals:
        //! Setup changed
        void setupChanged();

    private:
        //! Settings have been changed
        void onSettingsChanged();

        swift::misc::CSettingReadOnly<swift::misc::network::settings::TTrafficServers> m_otherTrafficNetworkServers {
            this, &CNetworkSetup::onSettingsChanged
        };
        swift::misc::CData<swift::misc::network::data::TLastServer> m_lastServer {
            this, &CNetworkSetup::onSettingsChanged
        }; //!< recently used server (VATSIM, other)
        swift::misc::CData<swift::core::data::TVatsimLastServer> m_lastVatsimServer {
            this, &CNetworkSetup::onSettingsChanged
        }; //!< recently used VATSIM server
        swift::misc::CData<TLastServerTabOther> m_lastServerTabOther {
            this, &CNetworkSetup::onSettingsChanged
        }; //!< last selected network tab (VATSIM/other), independent of login
    };
} // namespace swift::core::data

#endif // SWIFT_CORE_DATA_NETWORKSETUP_H
