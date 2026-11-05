// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SETTINGS_MINISETTINGS_H
#define SWIFT_GUI_SETTINGS_MINISETTINGS_H

#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/settingscache.h"

namespace swift::gui::settings
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

    //! Setting: Sprache der Mini-GUI (Teil der "Miscellaneous" Einstellungen).
    //! Wert ist entweder "system" (Systemsprache verwenden) oder ein Locale-Name wie "de_DE", "en_US".
    struct TMiniLanguage : public swift::misc::TSettingTrait<QString>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "miniguimisc/language"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Mini GUI: Sprache");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const QString &defaultValue()
        {
            static const QString def("system");
            return def;
        }
    };
} // namespace swift::gui::settings

#endif // SWIFT_GUI_SETTINGS_MINISETTINGS_H