// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFTGUIMINI_MINISETTINGS_H
#define SWIFTGUIMINI_MINISETTINGS_H

#include <QString>

#include "misc/settingscache.h"

namespace miniguisettings
{
    //! Sortiermodus fuer die ATC-Liste in der Mini-GUI
    enum MiniAtcSortMode
    {
        SortByDistance = 0, //!< nach Entfernung
        SortByCategory = 1 //!< nach Kategorie (Buchstaben nach dem letzten Unterstrich im Rufzeichen)
    };

    //! Setting: ATC-Sortiermodus (Teil der "Miscellaneous" Einstellungen der Mini-GUI)
    struct TMiniAtcSortMode : public swift::misc::TSettingTrait<int>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "miniguimisc/atcsortmode"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Mini GUI: ATC Sortierung");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const int &defaultValue()
        {
            static const int def = static_cast<int>(SortByDistance);
            return def;
        }
    };
} // namespace miniguisettings

#endif // SWIFTGUIMINI_MINISETTINGS_H