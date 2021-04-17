/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_EVENTLOOP_H
#define BLACKMISC_EVENTLOOP_H

#include "blackmisc/blackmiscexport.h"

#include <QObject>
#include <QEventLoop>
#include <QTimer>

namespace BlackMisc
{
    /*!
     * Utility class which blocks until a signal is emitted or timeout reached
     */
    class CEventLoop
    {
    public:
        //! Deleted constructor
        CEventLoop() = delete;

        //! Wait for the given time, while processing events.
        static void processEventsFor(int timeoutMs)
        {
            QEventLoop eventLoop;
            QTimer::singleShot(timeoutMs, &eventLoop, &QEventLoop::quit);
            eventLoop.exec();
        }

        //! Block, but keep processing events, until sender emits the signal or the timeout expires.
        //! Return true if the signal was emitted, false if the timeout expired.
        template <typename T, typename F>
        static bool processEventsUntil(const T *sender, F signal, int timeoutMs)
        {
            return processEventsUntil(sender, signal, timeoutMs, [] {});
        }

        //! Overloaded version that executes an initial function after connecting the signal.
        //! If the function's return type is convertible to bool, and it evaluates to true,
        //! then the waiting will immediately time out and return true.
        template <typename T, typename F1, typename F2>
        static bool processEventsUntil(const T *sender, F1 signal, int timeoutMs, F2 init)
        {
            QEventLoop eventLoop;
            bool result = false;
            QObject::connect(sender, signal, &eventLoop, [ & ]
            {
                result = true;
                eventLoop.quit();
            });
            if constexpr (std::is_void_v<decltype(init())>) { init(); }
            else if (init()) { return true; }
            if (timeoutMs > 0)
            {
                QTimer::singleShot(timeoutMs, &eventLoop, &QEventLoop::quit);
            }
            eventLoop.exec();
            return result;
        }
    };
} // ns

#endif // guard
