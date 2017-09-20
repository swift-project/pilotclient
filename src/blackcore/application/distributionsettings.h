/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_APPLICATION_DISTRIBUTIONSETTINGS_H
#define BLACKCORE_APPLICATION_DISTRIBUTIONSETTINGS_H

#include "blackcore/application.h"
#include "blackconfig/buildconfig.h"
#include "blackmisc/settingscache.h"
#include <QStringList>

namespace BlackCore
{
    namespace Application
    {
        //! Distribution settings, QStringList with 2 values: channel/platform
        struct TDistribution : public BlackMisc::TSettingTrait<QStringList>
        {
            //! \copydoc BlackMisc::TSettingTrait::key
            static const char *key() { return "distribution"; }

            //! \copydoc BlackMisc::TSettingTrait::defaultValue
            static const QStringList &defaultValue()
            {
                // guessing / preseting some default values
                static const QStringList d = (sApp && !sApp->getDistributionInfo().isEmpty()) ?
                                             sApp->getDistributionInfo().guessMyDefaultChannelAndPlatform() : // from cached or loaded distribution data
                                             QStringList({"ALPHA", BlackConfig::CBuildConfig::guessMyPlatformString()}); // guessing
                return d;
            }

            //! \copydoc BlackMisc::TSettingTrait::humanReadable
            static const QString &humanReadable()
            {
                static const QString name("Download channel/platform");
                return name;
            }
        };
    } // ns
} // ns

#endif // guard
