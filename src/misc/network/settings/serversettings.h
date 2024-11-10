// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_SETTINGS_SERVERSSETTINGS_H
#define SWIFT_MISC_NETWORK_SETTINGS_SERVERSSETTINGS_H

#include "misc/network/serverlist.h"
#include "misc/settingscache.h"
#include "misc/valueobject.h"

namespace swift::misc::network::settings
{
    /*!
     * Virtual air traffic servers
     */
    struct TTrafficServers : public TSettingTrait<CServerList>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "network/trafficservers"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
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
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "network/currenttrafficserver"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Current traffic servers");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const swift::misc::network::CServer &defaultValue()
        {
            static const CServer dv;
            return dv;
        }
    };
} // ns

#endif
