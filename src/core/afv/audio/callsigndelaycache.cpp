// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/afv/audio/callsigndelaycache.h"

#include <algorithm>

namespace swift::core::afv::audio
{
    void CallsignDelayCache::initialise(const QString &callsign)
    {
        if (!m_delayCache.contains(callsign)) { m_delayCache[callsign] = delayDefault; }
        if (!m_successfulTransmissionsCache.contains(callsign)) { m_successfulTransmissionsCache[callsign] = 0; }
    }

    int CallsignDelayCache::get(const QString &callsign) { return m_delayCache[callsign]; }

    void CallsignDelayCache::underflow(const QString &callsign)
    {
        if (!m_successfulTransmissionsCache.contains(callsign)) return;

        m_successfulTransmissionsCache[callsign] = 0;
        increaseDelayMs(callsign);
    }

    void CallsignDelayCache::success(const QString &callsign)
    {
        if (!m_successfulTransmissionsCache.contains(callsign)) return;

        m_successfulTransmissionsCache[callsign]++;
        if (m_successfulTransmissionsCache[callsign] > 5)
        {
            decreaseDelayMs(callsign);
            m_successfulTransmissionsCache[callsign] = 0;
        }
    }

    void CallsignDelayCache::increaseDelayMs(const QString &callsign)
    {
        if (!m_delayCache.contains(callsign)) return;

        m_delayCache[callsign] += delayIncrement;
        m_delayCache[callsign] = std::min(m_delayCache[callsign], delayMax);
    }

    void CallsignDelayCache::decreaseDelayMs(const QString &callsign)
    {
        if (!m_delayCache.contains(callsign)) return;

        m_delayCache[callsign] -= delayIncrement;
        m_delayCache[callsign] = std::max(m_delayCache[callsign], delayMin);
    }

    CallsignDelayCache &CallsignDelayCache::instance()
    {
        static CallsignDelayCache cache;
        return cache;
    }

} // namespace swift::core::afv::audio
