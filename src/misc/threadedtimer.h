// SPDX-FileCopyrightText: Copyright (C) 2025 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_THREADED_TIMER_H
#define SWIFT_MISC_THREADED_TIMER_H

#include <QObject>
#include <QTimer>

#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    //! Thread-safe timer class
    class SWIFT_MISC_EXPORT CThreadedTimer : public QObject
    {
        Q_OBJECT
    public:
        CThreadedTimer(QObject *owner, const QString& name, bool singleShot = false);

        //! Destructor
        ~CThreadedTimer() override = default;

        //! Start updating (start timer)
        //! \threadsafe
        void startTimer(std::chrono::milliseconds ms);

        //! Safely stop update time
        //! \threadsafe
        void stopTimer();

    signals:
        void timeout();

    private:
        QTimer m_updateTimer { this }; //!< timer which can be used by implementing classes
    };
} // namespace swift::misc

#endif // SWIFT_MISC_THREADED_TIMER_H
