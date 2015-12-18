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

#include "blackmisc/settingscache.h"
#include "blackmisc/network/serverlist.h"
#include <QTextCodec>

namespace BlackCore
{
    namespace Settings
    {
        namespace Network
        {
            //! Name of text codec to use with text in FSD protocol
            struct WireTextCodec : public BlackMisc::CSettingTrait<QString>
            {
                //! \copydoc BlackMisc::CSettingTrait::key
                static const char *key() { return "network/wiretextcodec"; }

                //! \copydoc BlackMisc::CSettingTrait::defaultValue
                static const QString &defaultValue() { static const QString dv("latin1"); return dv; }

                //! \copydoc BlackMisc::CSettingTrait::isValid
                static bool isValid(const QString &value) { return QTextCodec::codecForName(qPrintable(value)); }
            };

            //! Virtual air traffic servers
            struct TrafficServers : public BlackMisc::CSettingTrait<BlackMisc::Network::CServerList>
            {
                //! \copydoc BlackMisc::CSettingTrait::key
                static const char *key() { return "network/trafficservers"; }
            };

            //! Currently selected virtual air traffic server
            struct CurrentTrafficServer : public BlackMisc::CSettingTrait<BlackMisc::Network::CServer>
            {
                //! \copydoc BlackMisc::CSettingTrait::key
                static const char *key() { return "network/currenttrafficserver"; }

                //! \copydoc BlackMisc::CSettingTrait::defaultValue
                static const BlackMisc::Network::CServer &defaultValue()
                {
                    using namespace BlackMisc::Network;
                    static const CServer dv("Testserver", "Client project testserver", "vatsim-germany.org", 6809, CUser("guest", "Guest Client project", "", "guest"));
                    return dv;
                }
            };

            //! DBus server address
            struct DBusServerAddress : public BlackMisc::CSettingTrait<QString>
            {
                //! \copydoc BlackMisc::CSettingTrait::key
                static const char *key() { return "network/dbusserver"; }

                //! \copydoc BlackMisc::CSettingTrait::defaultValue
                static const QString &defaultValue() { static const QString dv("session"); return dv; }
            };
        } // ns
    } // ns
} // ns

#endif
