// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "tonepair.h"

namespace swift::sound
{
    CTonePair::CTonePair(const swift::misc::physical_quantities::CFrequency &frequency,
                         const swift::misc::physical_quantities::CFrequency &secondaryFrequency,
                         std::chrono::milliseconds duration)
        : m_firstFrequencyHz(
              static_cast<int>(frequency.valueRounded(swift::misc::physical_quantities::CFrequencyUnit::Hz()))),
          m_secondFrequencyHz(static_cast<int>(
              secondaryFrequency.valueRounded(swift::misc::physical_quantities::CFrequencyUnit::Hz()))),
          m_durationMs(duration)
    {}

} // namespace swift::sound
