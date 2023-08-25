// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_EVENTLOOP_H
#define BLACKMISC_EVENTLOOP_H

#include "blackmisc/blackmiscexport.h"

#include <QObject>
#include <QEventLoop>
#include <QPointer>
#include <QTimer>

namespace BlackMisc
{
    /*!
     * Utility class which blocks until a signal is emitted or timeout reached
     */
    class CEventLoop
    {
    public:
        //! Constructor.
        CEventLoop() : m_guard(&m_eventLoop) {}

        //! Constructor. Guard object must exist, and will be checked again when the loop quits.
        CEventLoop(QObject *guard) : m_guard(guard)
        {
            Q_ASSERT(guard);
            QObject::connect(guard, &QObject::destroyed, &m_eventLoop, [this] { m_eventLoop.exit(TimedOut); });
        }

        //! Event loop will stop if the given signal is received.
        template <typename T, typename F>
        void stopWhen(const T *sender, F signal)
        {
            QObject::connect(sender, signal, &m_eventLoop, [this] { m_eventLoop.exit(GotSignal); });
        }

        //! Event loop will stop if the given signal is received and condition returns true.
        template <typename T, typename F1, typename F2>
        void stopWhen(const T *sender, F1 signal, F2 &&condition)
        {
            QObject::connect(sender, signal, &m_eventLoop, [this, condition = std::forward<F2>(condition)](auto &&...args) {
                if (condition(std::forward<decltype(args)>(args)...)) { m_eventLoop.exit(GotSignal); }
            });
        }

        //! Begin processing events until the timeout or stop condition occurs.
        //! \return True if the signal was received, false if it timed out or the guard object died.
        bool exec(int timeoutMs)
        {
            if (timeoutMs >= 0)
            {
                QTimer::singleShot(timeoutMs, &m_eventLoop, [this] { m_eventLoop.exit(TimedOut); });
            }
            return m_eventLoop.exec() == GotSignal && isGuardAlive();
        }

        //! True if the guard object still exists.
        bool isGuardAlive() const
        {
            return m_guard;
        }

    private:
        enum Result
        {
            GotSignal = 0,
            TimedOut,
        };
        QEventLoop m_eventLoop;
        QPointer<QObject> m_guard;
    };
} // ns

#endif // guard
