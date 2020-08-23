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
#include <QThread>
#include <functional>

namespace BlackMisc
{
    /*!
     * Utility class for threaded operations
     */
    class BLACKMISC_EXPORT CThreadUtils
    {
    public:
        //! No constructor
        CThreadUtils() = delete;

        //! Is the current thread the QObject's thread?
        //! \remarks can be used as ASSERT check for threaded objects
        static bool isCurrentThreadObjectThread(const QObject *toBeTested);

        //! Is the application thread the QObject's thread?
        //! \remarks can be used as ASSERT check for threaded objects
        static bool isApplicationThreadObjectThread(const QObject *toBeTested);

        //! Is the application thread the QObject's thread?
        //! \remarks can be used as ASSERT check for threaded objects
        static bool isApplicationThread(const QThread *toBeTested);

        //! Is the current thread the Application thread?
        //! \remarks can be used as ASSERT check for threaded objects
        static bool isCurrentThreadApplicationThread();

        //! Priority to string
        static const QString &priorityToString(QThread::Priority priority);

        //! Thread to int string info
        static const QString threadToString(const void *t);

        //! Info about current thread
        static const QString threadInfo(const QThread *thread);

        //! Info about current thread
        static const QString currentThreadInfo();

        //! Call in object's thread IF not already in object's thread
        static bool callInObjectThread(QObject *object, std::function<void()> callFunct);
    };
} // ns

#endif // guard
