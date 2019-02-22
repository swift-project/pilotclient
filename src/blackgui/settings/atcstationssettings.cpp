/* Copyright (C) 2016
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "atcstationssettings.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Settings
    {
        CAtcStationsSettings::CAtcStationsSettings()
        { }

        QString CAtcStationsSettings::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            static const QString s("In range only: %1 valid freq: %2 valid voice room: %3");
            return s.arg(boolToOnOff(this->showOnlyInRange()), boolToOnOff(this->showOnlyWithValidFrequency()), boolToOnOff(this->showOnlyWithValidVoiceRoom()));
        }

        CVariant CAtcStationsSettings::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexInRangeOnly: return CVariant::fromValue(m_showOnlyInRange);
            case IndexValidFrequencyOnly: return CVariant::fromValue(m_onlyWithValidFrequency);
            case IndexValidVoiceRoomOnly: return CVariant::fromValue(m_onlyWithValidVoiceRoom);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CAtcStationsSettings::setPropertyByIndex(const CPropertyIndex &index, const BlackMisc::CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAtcStationsSettings>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexInRangeOnly: this->setShowOnlyInRange(variant.toBool()); break;
            case IndexValidFrequencyOnly: this->setShowOnlyWithValidFrequency(variant.toBool()); break;
            case IndexValidVoiceRoomOnly: this->setShowOnlyWithValidVoiceRoom(variant.toBool()); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }
    } // ns
} // ns
