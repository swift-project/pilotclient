/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGS_VIEWDIRECTORYSETTINGS_H
#define BLACKGUI_SETTINGS_VIEWDIRECTORYSETTINGS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/directoryutils.h"
#include <QString>

namespace BlackGui
{
    namespace Settings
    {
        //! Trait for directory settings
        struct TViewDirectorySettings : public BlackMisc::TSettingTrait<QString>
        {
            //! Key in data cache
            static const char *key() { return "guiviewdirectory/%Application%"; }

            //! Validator function.
            static bool isValid(const QString &directory) { Q_UNUSED(directory); return true; }

            //! Default, not consolidating
            static const QString &defaultValue() { return BlackMisc::CDirectoryUtils::documentationDirectory(); }
        };
    } // ns
} // ns

#endif // guard
