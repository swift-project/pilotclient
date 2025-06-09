// SPDX-FileCopyrightText: Copyright (C) 2025 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/threadedreaderperiodic.h"

namespace swift::core
{
    CThreadedReaderPeriodic::CThreadedReaderPeriodic(QObject *owner, const QString &name) : CThreadedReader(owner, name)
    {
        connect(&m_updateTimer, &QTimer::timeout, this, &CThreadedReaderPeriodic::doWork);
        m_updateTimer.setSingleShot(true);
    }

    void CThreadedReaderPeriodic::startReader()
    {
        Q_ASSERT(m_initialTime > 0);
        QTimer::singleShot(m_initialTime, this, [=] { this->doWork(); });
    }

    void CThreadedReaderPeriodic::setInitialAndPeriodicTime(int initialTime, int periodicTime)
    {
        m_initialTime = initialTime;
        m_periodicTime = periodicTime;

        // if timer is active start with delta time
        // remark: will be reset in doWork
        if (m_updateTimer.isActive())
        {
            const int oldPeriodicTime = m_updateTimer.interval();
            const int delta = m_periodicTime - oldPeriodicTime + m_updateTimer.remainingTime();
            m_updateTimer.start(qMax(delta, 0));
        }
    }

    void CThreadedReaderPeriodic::doWork()
    {
        if (!doWorkCheck()) { return; }
        this->doWorkImpl();
        Q_ASSERT(m_periodicTime > 0);
        m_updateTimer.start(m_periodicTime); // restart
    }

} // namespace swift::core
