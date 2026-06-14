// SPDX-FileCopyrightText: Copyright (C) 2025 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/threadedreaderperiodic.h"

namespace swift::core
{
    CThreadedReaderPeriodic::CThreadedReaderPeriodic(QObject *owner, const QString &name)
        : CThreadedReader(owner, name), m_updateTimer(this, name, true)
    {
        connect(&m_updateTimer, &misc::CThreadedTimer::timeout, this, &CThreadedReaderPeriodic::doWork);
    }

    void CThreadedReaderPeriodic::startReader()
    {
        Q_ASSERT_X(hasStarted(), Q_FUNC_INFO, "Thread was not started yet!");
        QTimer::singleShot(m_initialTime.load(), this, [=, this] { this->doWork(); });
    }

    void CThreadedReaderPeriodic::setInitialAndPeriodicTime(std::chrono::milliseconds initialTime,
                                                            std::chrono::milliseconds periodicTime)
    {
        m_initialTime = initialTime;
        m_periodicTime = periodicTime;
    }

    void CThreadedReaderPeriodic::doWork()
    {
        if (!doWorkCheck()) { return; }
        this->doWorkImpl();
        using namespace std::chrono_literals;
        Q_ASSERT(m_periodicTime.load() > 0ms);

        m_updateTimer.startTimer(m_periodicTime); // restart
    }

} // namespace swift::core
