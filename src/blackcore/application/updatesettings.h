// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_APPLICATION_UPDATESETTINGS_H
#define BLACKCORE_APPLICATION_UPDATESETTINGS_H

#include "blackcore/application.h"
#include "config/buildconfig.h"
#include "blackmisc/platform.h"
#include "blackmisc/settingscache.h"
#include <QStringList>

namespace BlackCore::Application
{
    //! Update info settings, QStringList with 2 values: channel/platform
    struct TUpdatePreferences : public BlackMisc::TSettingTrait<QStringList>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "updatepreferences"; }

        //! \copydoc BlackMisc::TSettingTrait::defaultValue
        static const QStringList &defaultValue()
        {
            // guessing / preset-ing some default values
            static const QStringList d = (sApp && !sApp->getUpdateInfo().isEmpty()) ?
                                             sApp->getUpdateInfo().anticipateMyDefaultChannelAndPlatform() : // from cached or loaded update info
                                             QStringList({ "STABLE", BlackMisc::CPlatform::currentPlatform().getPlatformName() });
            return d;
        }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Updates channel/platform");
            return name;
        }
    };
} // ns

#endif // guard
