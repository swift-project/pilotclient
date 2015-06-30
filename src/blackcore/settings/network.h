/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SETTINGS_NETWORK_H
#define BLACKCORE_SETTINGS_NETWORK_H

#include "blackcore/settingscache.h"
#include "blackmisc/network/serverlist.h"

namespace BlackCore
{
    namespace Settings
    {
        namespace Network
        {
            //! Virtual air traffic servers
            struct TrafficServers : public CSettingTrait<BlackMisc::Network::CServerList>
            {
                //! \copydoc BlackCore::CSetting::key
                static const char *key() { return "network/trafficservers"; }
            };

            //! Currently selected virtual air traffic server
            struct CurrentTrafficServer : public CSettingTrait<BlackMisc::Network::CServer>
            {
                //! \copydoc BlackCore::CSetting::key
                static const char *key() { return "network/currenttrafficserver"; }

                //! \copydoc BlackCore::CSetting::defaultValue
                static const BlackMisc::Network::CServer &defaultValue()
                {
                    using namespace BlackMisc::Network;
                    static const CServer dv("Testserver", "Client project testserver", "vatsim-germany.org", 6809, CUser("guest", "Guest Client project", "", "guest"));
                    return dv;
                }
            };

            //! Booking service URL
            struct BookingService : public CSettingTrait<QString>
            {
                //! \copydoc BlackCore::CSetting::key
                static const char *key() { return "network/bookingservice"; }

                //! \copydoc BlackCore::CSetting::defaultValue
                static const QString &defaultValue() { static const QString dv("http://vatbook.euroutepro.com/xml2.php"); return dv; }
            };

            //! DBus server address
            struct DBusServerAddress : public CSettingTrait<QString>
            {
                //! \copydoc BlackCore::CSetting::key
                static const char *key() { return "network/dbusserver"; }

                //! \copydoc BlackCore::CSetting::defaultValue
                static const QString &defaultValue() { static const QString dv("session"); return dv; }
            };
        }
    }
}

#endif
