// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_APPLICATION_UPDATESETTINGS_H
#define SWIFT_CORE_APPLICATION_UPDATESETTINGS_H

#include <QStringList>

#include "config/buildconfig.h"
#include "core/application.h"
#include "misc/platform.h"
#include "misc/settingscache.h"

namespace swift::core::application
{
    //! Update info settings, QStringList with 2 values: channel/platform
    struct TUpdatePreferences : public swift::misc::TSettingTrait<QStringList>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "updatepreferences"; }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const QStringList &defaultValue()
        {
            // guessing / preset-ing some default values
            static const QStringList d =
                (sApp && !sApp->getUpdateInfo().isEmpty()) ?
                    sApp->getUpdateInfo().anticipateMyDefaultChannelAndPlatform() : // from cached or loaded update info
                    QStringList({ "STABLE", swift::misc::CPlatform::currentPlatform().getPlatformName() });
            return d;
        }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Updates channel/platform");
            return name;
        }
    };
} // namespace swift::core::application

#endif // guard
