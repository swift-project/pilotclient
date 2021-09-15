/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "callsigndelaycache.h"

namespace BlackCore::Afv::Audio
{
    void CallsignDelayCache::initialise(const QString &callsign)
    {
        if (!m_delayCache.contains(callsign)) { m_delayCache[callsign] = delayDefault; }
        if (!successfulTransmissionsCache.contains(callsign)) { successfulTransmissionsCache[callsign] = 0; }
    }

    int CallsignDelayCache::get(const QString &callsign)
    {
        return m_delayCache[callsign];
    }

    void CallsignDelayCache::underflow(const QString &callsign)
    {
        if (!successfulTransmissionsCache.contains(callsign)) return;

        successfulTransmissionsCache[callsign] = 0;
        increaseDelayMs(callsign);
    }

    void CallsignDelayCache::success(const QString &callsign)
    {
        if (!successfulTransmissionsCache.contains(callsign)) return;

        successfulTransmissionsCache[callsign]++;
        if (successfulTransmissionsCache[callsign] > 5)
        {
            decreaseDelayMs(callsign);
            successfulTransmissionsCache[callsign] = 0;
        }
    }

    void CallsignDelayCache::increaseDelayMs(const QString &callsign)
    {
        if (!m_delayCache.contains(callsign))
            return;

        m_delayCache[callsign] += delayIncrement;
        if (m_delayCache[callsign] > delayMax)
        {
            m_delayCache[callsign] = delayMax;
        }
    }

    void CallsignDelayCache::decreaseDelayMs(const QString &callsign)
    {
        if (!m_delayCache.contains(callsign))
            return;

        m_delayCache[callsign] -= delayIncrement;
        if (m_delayCache[callsign] < delayMin)
        {
            m_delayCache[callsign] = delayMin;
        }
    }

    CallsignDelayCache &CallsignDelayCache::instance()
    {
        static CallsignDelayCache cache;
        return cache;
    }

} // ns
