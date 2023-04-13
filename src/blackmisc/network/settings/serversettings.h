/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
            static const CServer dv = CServer::swiftFsdTestServer();
            return dv;
        }
    };

    /*!
     * Auto logoff
     */
    struct TAutoLogoff : public TSettingTrait<bool>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "network/autologoff"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Auto logoff");
            return name;
        }
    };
} // ns

#endif
