/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_DIGESTSIGNAL_H
#define BLACKMISC_DIGESTSIGNAL_H

#include "blackmisc/blackmiscexport.h"

#include <QObject>
#include <QTimer>

namespace BlackMisc
{
    //! Receive 1..n signals, collect them over time, and resend afer n milliseconds
    class BLACKMISC_EXPORT CDigestSignal : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        template <class T, class F1, class F2>
        CDigestSignal(T *sender, F1 inputSignal, F2 digestSignal, int maxDelayMs = 500, int maxInputsPerDigest = 3)
            : m_maxInputsPerDigest(maxInputsPerDigest)
        {
            QObject::connect(sender, inputSignal, this, &CDigestSignal::inputSignal);
            QObject::connect(this, &CDigestSignal::digestSignal, sender, digestSignal);
            init(maxDelayMs);
        }

        //! Constructor without input signal, can be manually triggered
        template <class T, class F2>
        CDigestSignal(T *sender, F2 digestSignal, int maxDelayMs = 500, int maxInputsPerDigest = 3)
            : m_maxInputsPerDigest(maxInputsPerDigest)
        {
            QObject::connect(this, &CDigestSignal::digestSignal, sender, digestSignal);
            init(maxDelayMs);
        }

        //! Destructor
        virtual ~CDigestSignal() {}

    signals:
        //! Send digest signal
        void digestSignal();

    public slots:
        //! Received input signal, or manually trigger
        void inputSignal();

    private:
        //! Timer timed out
        void timerTimeout();

        //! Init in ctor
        void init(int maxDelayMs);

        QTimer m_timer;
        const int m_maxInputsPerDigest = 3;
        int m_inputsCount = 0;
    };
}

#endif
