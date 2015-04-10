/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DIGESTSIGNAL_H
#define BLACKMISC_DIGESTSIGNAL_H

#include "blackmiscexport.h"
#include <QTimer>
#include <QObject>

namespace BlackMisc
{

    //! Receive 1..n signal, collect them over time, and resend afer n milliseconds
    class BLACKMISC_EXPORT CDigestSignal : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        template <class T, class F1, class F2>
        CDigestSignal(T *sender, F1 inputSignal, F2 digestSignal, int maxDelayMs = 500, int maxInputsPerDigest = 3)
            : m_maxInputsPerDigest(maxInputsPerDigest)
        {
            QObject::connect(sender, inputSignal, this, &CDigestSignal::ps_inputSignal);
            QObject::connect(this, &CDigestSignal::digestSignal, sender, digestSignal);

            QObject::connect(&m_timer, &QTimer::timeout, this, &CDigestSignal::ps_timeout);
            m_timer.setSingleShot(true);
            m_timer.setInterval(maxDelayMs);
        }

        // Destructor
        virtual ~CDigestSignal() {}

    signals:
        //! Send digest signal
        void digestSignal();

    private slots:
        //! Received input signal
        void ps_inputSignal();

        //! Timer timed out
        void ps_timeout();

    private:
        QTimer m_timer;
        const int m_maxInputsPerDigest = 3;
        int m_inputsCount = 0;
    };
}

#endif
