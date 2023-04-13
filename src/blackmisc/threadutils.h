/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_THREADUTILS_H
#define BLACKMISC_THREADUTILS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/stacktrace.h"
#include "blackmisc/promise.h"
#include <QTimer>
#include <QThread>
#include <QObject>
#include <QMetaObject>
#include <QSharedPointer>
#include <functional>

namespace BlackMisc
{
    /*!
     * Starts a single-shot timer which will call a task in the thread of the given object when it times out.
     *
     * Differs from QTimer::singleShot in that this implementation interacts better with QObject::moveToThread,
     * and returns a QFuture which can be used to detect when the task has finished or obtain its return value.
     */
    template <typename F>
    auto singleShot(int msec, QObject *target, F &&task)
    {
        CPromise<decltype(task())> promise;
        QSharedPointer<QTimer> timer(new QTimer, [](QObject *o) { QMetaObject::invokeMethod(o, &QObject::deleteLater); });
        timer->setSingleShot(true);
        timer->moveToThread(target->thread());
        QObject::connect(timer.data(), &QTimer::timeout, target, [trace = getStackTrace(), task = std::forward<F>(task), timer, promise]() mutable {
            static_cast<void>(trace);
            timer.clear();
            promise.setResultFrom(task);
        });
        QMetaObject::invokeMethod(timer.data(), [t = timer.data(), msec] { t->start(msec); });
        return promise.future();
    }

    /*!
     * Utility class for threaded operations
     */
    class BLACKMISC_EXPORT CThreadUtils
    {
    public:
        //! No constructor
        CThreadUtils() = delete;

        //! Is the current thread the object's thread?
        //! \deprecated
        //! \todo Refactor to inline method
        static bool isInThisThread(const QObject *toBeTested);

        //! Is the current thread the application thread?
        //! \deprecated
        //! \todo Refactor to inline method
        static bool thisIsMainThread();

        //! Info about current thread, for debug messages
        static QString currentThreadInfo();
    };
} // ns

#endif // guard
