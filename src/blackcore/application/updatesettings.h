/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_APPLICATION_UPDATESETTINGS_H
#define BLACKCORE_APPLICATION_UPDATESETTINGS_H

#include "blackcore/application.h"
#include "blackconfig/buildconfig.h"
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
