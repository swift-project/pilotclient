// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/digestsignal.h"

#include <QPointer>

#include "misc/threadutils.h"

namespace swift::misc
{
    void CDigestSignal::inputSignal()
    {
        if (!CThreadUtils::isInThisThread(this))
        {
            // call in correct thread
            const QPointer<CDigestSignal> myself(this);
            QTimer::singleShot(0, this, [=] {
                if (!myself) { return; }
                this->inputSignal();
            });
            return;
        }

        m_timer.start(); // start or restart
        m_inputsCount++;
        if (m_inputsCount >= m_maxInputsPerDigest)
        {
            timerTimeout();
        }
    }

    void CDigestSignal::timerTimeout()
    {
        m_timer.stop();
        m_inputsCount = 0;
        emit this->digestSignal();
    }

    void CDigestSignal::init(int maxDelayMs)
    {
        QObject::connect(&m_timer, &QTimer::timeout, this, &CDigestSignal::timerTimeout);
        m_timer.setSingleShot(true);
        m_timer.setInterval(maxDelayMs);
    }
} // namespace swift::misc
