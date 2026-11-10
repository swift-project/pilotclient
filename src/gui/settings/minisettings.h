// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_SETTINGS_MINISETTINGS_H
#define SWIFT_GUI_SETTINGS_MINISETTINGS_H

#include <QString>

#include "gui/swiftguiexport.h"
#include "misc/aviation/flightplan.h"
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

    //! Setting: Ob eine Verbindung nur mit vorhandenem Flugplan zulaessig sein soll
    //! (Teil der "Miscellaneous" Einstellungen der Mini-GUI). Wenn aktiviert, bleibt der
    //! "Connect"-Button so lange deaktiviert, bis ein Flugplan geladen, ausgefuellt oder
    //! gesendet wurde (SimBrief, Datei, manuelle Eingabe oder Netzwerk - alle gleichwertig).
    struct TMiniRequireFlightPlanForConnect : public swift::misc::TSettingTrait<bool>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "miniguimisc/requireflightplanforconnect"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Mini GUI: Verbindung nur mit Flugplan");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const bool &defaultValue()
        {
            static const bool def = false;
            return def;
        }
    };

    //! Setting: Transparenzgrad des Fensters im "Minimize"-Modus der Mini-GUI (in Prozent, 10-100).
    //! 100 = voll deckend (keine Transparenz), niedrigere Werte = durchsichtiger.
    struct TMiniMinimizeOpacity : public swift::misc::TSettingTrait<int>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "miniguimisc/minimizeopacity"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Mini GUI: Transparenz im Minimize-Modus");
            return name;
        }

        //! \copydoc swift::misc::TSettingTrait::defaultValue
        static const int &defaultValue()
        {
            static const int def = 50;
            return def;
        }
    };

    //! Setting: Name des zuletzt auf dem "Connect" Reiter der Mini-GUI ausgewaehlten Servers
    //! (unabhaengig vom "Last used server" der swiftguistandard, damit beide Anwendungen sich
    //! nicht gegenseitig beeinflussen). Wird ueber die neue Server-Auswahlbox oberhalb von
    //! "Own Aircraft" gesetzt und beim naechsten Start der Mini-GUI wiederhergestellt.
    struct TMiniLastServer : public swift::misc::TSettingTrait<QString>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "miniguimisc/lastserver"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Mini GUI: Letzter Server");
            return name;
        }
    };

    //! Setting: Zuletzt im "Flight Plan Management" Dialog der Mini-GUI eingegebener Flugplan
    //! (unabhaengig vom Flugplan der swiftguistandard). Wird beim Schliessen des Dialogs
    //! gespeichert und beim naechsten Start der Mini-GUI als aktiver Flugplan wiederhergestellt.
    struct TMiniLastFlightPlan : public swift::misc::TSettingTrait<swift::misc::aviation::CFlightPlan>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "miniguimisc/lastflightplan"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Mini GUI: Letzter Flugplan");
            return name;
        }
    };
} // namespace swift::gui::settings

#endif // SWIFT_GUI_SETTINGS_MINISETTINGS_H