/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SETTINGS_ATCSTATIONSSETTINGS_H
#define BLACKGUI_SETTINGS_ATCSTATIONSSETTINGS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"
#include <QString>

namespace BlackGui
{
    namespace Settings
    {
        //! How to display ATC stations
        class BLACKGUI_EXPORT CAtcStationsSettings : public BlackMisc::CValueObject<CAtcStationsSettings>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexInRangeOnly = BlackMisc::CPropertyIndex::GlobalIndexCAtcStationsSettings,
            };

            //! Default constructor
            CAtcStationsSettings();

            //! Show in range ATC stations only
            bool showOnlyInRange() const { return m_showOnlyInRange; }

            //! Show in range ATC stations only?
            void setShowOnlyInRange(bool onlyInRange) { m_showOnlyInRange = onlyInRange; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

        private:
            bool m_showOnlyInRange = true;

            BLACK_METACLASS(
                CAtcStationsSettings,
                BLACK_METAMEMBER(showOnlyInRange)
            );
        };

        //! ATC stations settings
        struct TAtcStationsSettings : public BlackMisc::TSettingTrait<CAtcStationsSettings>
        {
            //! \copydoc BlackCore::TSettingTrait::key
            static const char *key() { return "atcstations"; }

            //! \copydoc BlackCore::TSettingTrait::humanReadable
            static const QString &humanReadable() { static const QString name("ATC stations"); return name; }
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackGui::Settings::CAtcStationsSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackGui::Settings::CAtcStationsSettings>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackGui::Settings::CAtcStationsSettings>)

#endif // guard
