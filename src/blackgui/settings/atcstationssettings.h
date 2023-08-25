// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_SETTINGS_ATCSTATIONSSETTINGS_H
#define BLACKGUI_SETTINGS_ATCSTATIONSSETTINGS_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/settingscache.h"
#include <QString>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackGui::Settings, CAtcStationsSettings)

namespace BlackGui::Settings
{
    //! How to display ATC stations
    class BLACKGUI_EXPORT CAtcStationsSettings : public BlackMisc::CValueObject<CAtcStationsSettings>
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexInRangeOnly = BlackMisc::CPropertyIndexRef::GlobalIndexCAtcStationsSettings,
            IndexValidFrequencyOnly
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

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

    private:
        bool m_showOnlyInRange = true;
        bool m_onlyWithValidFrequency = true;

        BLACK_METACLASS(
            CAtcStationsSettings,
            BLACK_METAMEMBER(showOnlyInRange),
            BLACK_METAMEMBER(onlyWithValidFrequency)
        );
    };

    //! ATC stations settings
    struct TAtcStationsSettings : public BlackMisc::TSettingTrait<CAtcStationsSettings>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "atcstations"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("ATC stations");
            return name;
        }
    };
} // ns

Q_DECLARE_METATYPE(BlackGui::Settings::CAtcStationsSettings)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackGui::Settings::CAtcStationsSettings>)

#endif // guard
