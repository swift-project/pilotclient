/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "digestsignal.h"
#include "threadutils.h"

namespace BlackMisc
{
    void CDigestSignal::inputSignal()
    {
        if (!CThreadUtils::isCurrentThreadObjectThread(this))
        {
            // call in correct thread
            QTimer::singleShot(0, this, &CDigestSignal::inputSignal);
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
        emit digestSignal();
    }

    void CDigestSignal::init(int maxDelayMs)
    {
        QObject::connect(&m_timer, &QTimer::timeout, this, &CDigestSignal::timerTimeout);
        m_timer.setSingleShot(true);
        m_timer.setInterval(maxDelayMs);
    }

} // namespace
