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
            //! \copydoc BlackCore::TSettingTrait::key
            static const char *key() { return "guiviewdirectory/%Application%"; }

            //! \copydoc BlackCore::TSettingTrait::humanReadable
            static const QString &humanReadable() { static const QString name("View directory"); return name; }

            //! \copydoc BlackCore::TSettingTrait::isValid
            static bool isValid(const QString &directory) { Q_UNUSED(directory); return true; }

            //! \copydoc BlackCore::TSettingTrait::defaultValue
            static const QString &defaultValue() { return BlackMisc::CDirectoryUtils::documentationDirectory(); }
        };
    } // ns
} // ns

#endif // guard
