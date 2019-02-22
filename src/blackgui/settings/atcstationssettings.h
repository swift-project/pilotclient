/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
                IndexValidFrequencyOnly,
                IndexValidVoiceRoomOnly
            };

            //! Default constructor
            CAtcStationsSettings();

            //! Show in range ATC stations only?
            bool showOnlyInRange() const { return m_showOnlyInRange; }

            //! Show in range ATC stations only
            void setShowOnlyInRange(bool onlyInRange) { m_showOnlyInRange = onlyInRange; }

            //! Show only with valid frequency?
            bool showOnlyWithValidFrequency() const { return m_onlyWithValidFrequency; }

            //! Show only with valid frequency
            void setShowOnlyWithValidFrequency(bool onlyValidFrequency) { m_onlyWithValidFrequency = onlyValidFrequency; }

            //! Show only with valid voice room?
            bool showOnlyWithValidVoiceRoom() const { return m_onlyWithValidVoiceRoom; }

            //! Show only with valid voice room
            void setShowOnlyWithValidVoiceRoom(bool onlyValidVoiceRoom) { m_onlyWithValidVoiceRoom = onlyValidVoiceRoom; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            BlackMisc::CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const BlackMisc::CVariant &variant);

        private:
            bool m_showOnlyInRange = true;
            bool m_onlyWithValidFrequency = true;
            bool m_onlyWithValidVoiceRoom = false;

            BLACK_METACLASS(
                CAtcStationsSettings,
                BLACK_METAMEMBER(showOnlyInRange),
                BLACK_METAMEMBER(onlyWithValidFrequency),
                BLACK_METAMEMBER(onlyWithValidVoiceRoom)
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
