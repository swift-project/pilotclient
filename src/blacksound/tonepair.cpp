/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "tonepair.h"

namespace BlackSound
{
    CTonePair::CTonePair(const BlackMisc::PhysicalQuantities::CFrequency &frequency,
                         const BlackMisc::PhysicalQuantities::CFrequency &secondaryFrequency,
                         const BlackMisc::PhysicalQuantities::CTime &duration) : m_firstFrequencyHz(static_cast<int>(frequency.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::Hz()))),
                                                                                 m_secondFrequencyHz(static_cast<int>(secondaryFrequency.valueRounded(BlackMisc::PhysicalQuantities::CFrequencyUnit::Hz()))),
                                                                                 m_durationMs(static_cast<qint64>(duration.valueRounded(BlackMisc::PhysicalQuantities::CTimeUnit::ms())))
    {}

}
