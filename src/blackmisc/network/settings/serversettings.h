// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_SETTINGS_SERVERSSETTINGS_H
#define BLACKMISC_NETWORK_SETTINGS_SERVERSSETTINGS_H

#include "blackmisc/network/serverlist.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/valueobject.h"

namespace BlackMisc::Network::Settings
{
    /*!
     * Virtual air traffic servers
     */
    struct TTrafficServers : public TSettingTrait<CServerList>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "network/trafficservers"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Traffic servers");
            return name;
        }
    };

    /*!
     * Currently selected virtual air traffic server
     */
    struct TCurrentTrafficServer : public TSettingTrait<CServer>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "network/currenttrafficserver"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Current traffic servers");
            return name;
        }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static const BlackMisc::Network::CServer &defaultValue()
        {
            static const CServer dv;
            return dv;
        }
    };
} // ns

#endif
