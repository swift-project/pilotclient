/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airspace_watchdog.h"
#include "blackmisc/logmessage.h"

namespace BlackCore
{

    using namespace BlackMisc;
    using namespace BlackMisc::Aviation;
    using namespace BlackMisc::PhysicalQuantities;

    CAirspaceWatchdog::CAirspaceWatchdog(QObject *parent)
        : QObject(parent)
    {
        startTimer(5000);
    }

    CAirspaceWatchdog::CAirspaceWatchdog(const CTime &initialTimeOut, QObject *parent)
        : QObject(parent), m_timeout(initialTimeOut)
    {
    }

    void CAirspaceWatchdog::addCallsign(const CCallsign &callsign)
    {
        if (m_callsignTimestamps.contains(callsign)) return;
        m_callsignTimestamps.insert(callsign, QDateTime::currentDateTimeUtc());
    }

    void CAirspaceWatchdog::resetCallsign(const CCallsign &callsign)
    {
        Q_ASSERT(m_callsignTimestamps.contains(callsign));
        m_callsignTimestamps[callsign] = QDateTime::currentDateTimeUtc();
    }

    void CAirspaceWatchdog::removeCallsign(const CCallsign &callsign)
    {
        m_callsignTimestamps.remove(callsign);
    }

    void CAirspaceWatchdog::removeAll()
    {
        m_callsignTimestamps.clear();
    }

    void CAirspaceWatchdog::timerEvent(QTimerEvent *event)
    {
        Q_UNUSED(event)
        checkTimeouts();
    }

    void CAirspaceWatchdog::checkTimeouts()
    {
        for (CCallsign callsign : m_callsignTimestamps.keys())
        {
            QDateTime timestamp = m_callsignTimestamps.value(callsign);
            if (timestamp.secsTo(QDateTime::currentDateTimeUtc()) > m_timeout.value(CTimeUnit::s()))
            {
                CLogMessage(this).debug() << "Aircraft " << callsign.toQString() << "timed out!";
                m_callsignTimestamps.remove(callsign);
                emit timeout(callsign);
            }
        }
    }
}
