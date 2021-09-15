/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SETTINGS_FGSWIFTBUSSETTINGS_H
#define BLACKMISC_SIMULATION_SETTINGS_FGSWIFTBUSSETTINGS_H

#include <QString>
#include "blackmisc/settingscache.h"
#include "blackmisc/dbusserver.h"

namespace BlackMisc::Simulation::Settings
{
    /*!
     * Setting for FGSwiftBus.
     */
    struct TFGSwiftBusServer : public BlackMisc::TSettingTrait<QString>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "fgswiftbus/server"; }

        //! \copydoc BlackCore::TSettingTrait::humanReadable
        static const QString &humanReadable() { static const QString name("FGSwiftBus"); return name; }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static QString defaultValue() { return "tcp:host=127.0.0.1,port=45003"; }

        //! \copydoc BlackMisc::TSettingTrait::isValid
        static bool isValid(const QString &dBusAddress, QString &) { return BlackMisc::CDBusServer::isSessionOrSystemAddress(dBusAddress) || BlackMisc::CDBusServer::isQtDBusAddress(dBusAddress); }
    };
} // ns

#endif // guard
