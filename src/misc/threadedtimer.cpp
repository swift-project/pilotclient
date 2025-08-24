// SPDX-FileCopyrightText: Copyright (C) 2025 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/threadedtimer.h"

#include <QPointer>

#include "threadutils.h"

namespace swift::misc
{
    CThreadedTimer::CThreadedTimer(QObject *owner, const QString &name, bool singleShot) : QObject(owner)
    {
        m_updateTimer.setObjectName(name + ":timer");
        m_updateTimer.setSingleShot(singleShot);
        connect(&m_updateTimer, &QTimer::timeout, this, &CThreadedTimer::timeout);
    }

    void CThreadedTimer::startTimer(std::chrono::milliseconds ms)
    {
        if (!CThreadUtils::isInThisThread(this))
        {
            // shift in correct thread
            QPointer<CThreadedTimer> myself(this);
            QTimer::singleShot(0, this, [=, this] {
                if (!myself) { return; }
                this->startTimer(ms);
            });
            return;
        }

        connect(thread(), &QThread::finished, &m_updateTimer, &QTimer::stop, Qt::UniqueConnection);
        m_updateTimer.start(ms);
    }

    void CThreadedTimer::stopTimer()
    {
        if (!CThreadUtils::isInThisThread(this))
        {
            // shift in correct thread
            QPointer<CThreadedTimer> myself(this);
            QTimer::singleShot(0, this, [=, this] {
                if (!myself) { return; }
                this->stopTimer();
            });
            return;
        }

        if (!m_updateTimer.isActive()) { return; }
        m_updateTimer.stop();
    }
} // namespace swift::misc
