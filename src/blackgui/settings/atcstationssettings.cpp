// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "atcstationssettings.h"

using namespace BlackMisc;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackGui::Settings, CAtcStationsSettings)

namespace BlackGui::Settings
{
    CAtcStationsSettings::CAtcStationsSettings()
    {}

    QString CAtcStationsSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        static const QString s("In range only: %1 valid freq: %2");
        return s.arg(boolToOnOff(this->showOnlyInRange()), boolToOnOff(this->showOnlyWithValidFrequency()));
    }

    QVariant CAtcStationsSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexInRangeOnly: return QVariant::fromValue(m_showOnlyInRange);
        case IndexValidFrequencyOnly: return QVariant::fromValue(m_onlyWithValidFrequency);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CAtcStationsSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAtcStationsSettings>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexInRangeOnly: this->setShowOnlyInRange(variant.toBool()); break;
        case IndexValidFrequencyOnly: this->setShowOnlyWithValidFrequency(variant.toBool()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }
} // ns
