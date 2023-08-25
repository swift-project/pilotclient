// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSOUND_TONEPAIR_H
#define BLACKSOUND_TONEPAIR_H

#include "blacksound/blacksoundexport.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/time.h"

#include <tuple>

namespace BlackSound
{
    //! Tone pair to be played
    class BLACKSOUND_EXPORT CTonePair
    {
    public:
        //! Play two tones with frequencies f for t milliseconds
        CTonePair(const BlackMisc::PhysicalQuantities::CFrequency &frequency,
                  const BlackMisc::PhysicalQuantities::CFrequency &secondaryFrequency,
                  const BlackMisc::PhysicalQuantities::CTime &duration);

        //! Get frequency of the first tone
        int getFirstFrequencyHz() const { return m_firstFrequencyHz; }

        //! Get frequency of the second tone
        int getSecondFrequencyHz() const { return m_secondFrequencyHz; }

        //! Get play duration
        qint64 getDurationMs() const { return m_durationMs; }

        //! Comparison operator
        friend bool operator<(const CTonePair &lhs, const CTonePair &rhs)
        {
            return std::tie(lhs.m_firstFrequencyHz, lhs.m_secondFrequencyHz, lhs.m_durationMs) < std::tie(rhs.m_firstFrequencyHz, rhs.m_secondFrequencyHz, rhs.m_durationMs);
        }

    private:
        int m_firstFrequencyHz; //!< first tone's frequency, use 0 for silence
        int m_secondFrequencyHz; //!< second tone's frequency, or 0
        qint64 m_durationMs; //!< How long to play (duration)
    };
}

#endif // guard
