// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "tonepair.h"

namespace swift::sound
{
    CTonePair::CTonePair(const BlackMisc::PhysicalQuantities::CFrequency &frequency,
                         const BlackMisc::PhysicalQuantities::CFrequency &secondaryFrequency,
                         const BlackMisc::PhysicalQuantities::CTime &duration) : m_firstFrequencyHz(static_cast<int>(frequency.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::Hz()))),
                                                                                 m_secondFrequencyHz(static_cast<int>(secondaryFrequency.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::Hz()))),
                                                                                 m_durationMs(static_cast<qint64>(duration.valueRounded(BlackMisc::PhysicalQuantities::CTimeUnit::ms())))
    {}

}
