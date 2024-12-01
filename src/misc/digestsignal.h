// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_DIGESTSIGNAL_H
#define SWIFT_MISC_DIGESTSIGNAL_H

#include <QObject>
#include <QTimer>

#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    //! Receive 1..n signals, collect them over time, and resend afer n milliseconds
    class SWIFT_MISC_EXPORT CDigestSignal : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        template <class T, class F1, class F2>
        CDigestSignal(T *sender, F1 inputSignal, F2 digestSignal,
                      std::chrono::milliseconds maxDelay = std::chrono::milliseconds(500), int maxInputsPerDigest = 3)
            : m_maxInputsPerDigest(maxInputsPerDigest)
        {
            QObject::connect(sender, inputSignal, this, &CDigestSignal::inputSignal);
            QObject::connect(this, &CDigestSignal::digestSignal, sender, digestSignal);
            init(maxDelay);
        }

        //! Constructor without input signal, can be manually triggered
        template <class T, class F2>
        CDigestSignal(T *sender, F2 digestSignal, std::chrono::milliseconds maxDelay = std::chrono::milliseconds(500),
                      int maxInputsPerDigest = 3)
            : m_maxInputsPerDigest(maxInputsPerDigest)
        {
            QObject::connect(this, &CDigestSignal::digestSignal, sender, digestSignal);
            init(maxDelay);
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
        void init(std::chrono::milliseconds maxDelay);

        QTimer m_timer;
        const int m_maxInputsPerDigest = 3;
        int m_inputsCount = 0;
    };
} // namespace swift::misc

#endif // SWIFT_MISC_DIGESTSIGNAL_H
