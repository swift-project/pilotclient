// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_TONEPAIR_H
#define SWIFT_SOUND_TONEPAIR_H

#include <chrono>
#include <tuple>

#include "misc/pq/frequency.h"
#include "misc/pq/time.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound
{
    //! Tone pair to be played
    class SWIFT_SOUND_EXPORT CTonePair
    {
    public:
        //! Play two tones with frequencies f for t milliseconds
        CTonePair(const swift::misc::physical_quantities::CFrequency &frequency,
                  const swift::misc::physical_quantities::CFrequency &secondaryFrequency,
                  std::chrono::milliseconds duration);

        //! Get frequency of the first tone
        int getFirstFrequencyHz() const { return m_firstFrequencyHz; }

        //! Get frequency of the second tone
        int getSecondFrequencyHz() const { return m_secondFrequencyHz; }

        //! Get play duration
        std::chrono::milliseconds getDurationMs() const { return m_durationMs; }

        //! Comparison operator
        friend bool operator<(const CTonePair &lhs, const CTonePair &rhs)
        {
            return std::tie(lhs.m_firstFrequencyHz, lhs.m_secondFrequencyHz, lhs.m_durationMs) <
                   std::tie(rhs.m_firstFrequencyHz, rhs.m_secondFrequencyHz, rhs.m_durationMs);
        }

    private:
        int m_firstFrequencyHz; //!< first tone's frequency, use 0 for silence
        int m_secondFrequencyHz; //!< second tone's frequency, or 0
        std::chrono::milliseconds m_durationMs; //!< How long to play (duration)
    };
} // namespace swift::sound

#endif // SWIFT_SOUND_TONEPAIR_H
