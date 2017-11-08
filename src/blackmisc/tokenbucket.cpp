/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/tokenbucket.h"
#include "blackmisc/pq/units.h"

#include <QtGlobal>

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    CTokenBucket::CTokenBucket(int capacity, const CTime &interval, int numTokensToRefill)
        : m_capacity(capacity), m_intervalSecs(interval.value(CTimeUnit::s())), m_numTokensToRefill(numTokensToRefill) {}

    bool CTokenBucket::tryConsume(int numTokens)
    {
        Q_ASSERT(numTokens > 0 && numTokens < m_capacity);

        // Replenish maximal up to capacity
        int replenishedTokens = qMin(m_capacity, getTokens());

        // Take care of overflows
        m_availableTokens = qMin(m_availableTokens + replenishedTokens, m_capacity);

        if (numTokens <= m_availableTokens)
        {
            m_availableTokens -= numTokens;
            return true;
        }

        return false;
    }

    void CTokenBucket::setNumberOfTokensToRefill(int noTokens)
    {
        m_numTokensToRefill = noTokens;
    }

    void CTokenBucket::setCapacity(int capacity)
    {
        m_capacity = capacity;
    }

    int CTokenBucket::getTokens()
    {
        const auto now = QDateTime::currentDateTime();
        const auto deltaSeconds = m_lastReplenishmentTime.secsTo(now);
        const int numberOfTokens = static_cast<int>(m_numTokensToRefill * deltaSeconds / m_intervalSecs);

        // Update the time only when replenishment actually took place. We will end up in a infinite loop otherwise.
        if (numberOfTokens > 0) { m_lastReplenishmentTime = now; }
        return numberOfTokens;
    }
} // namespace
