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

        //! Is the current thread the object's thread?
        static bool isCurrentThreadObjectThread(const QObject *toBeTested);

        //! Is the application thread the object's thread?
        static bool isApplicationThreadObjectThread(const QObject *toBeTested);

        //! Is the application thread the object's thread?
        static bool isApplicationThread(const QThread *toBeTested);

        //! Is the current thread the application thread?
        static bool isCurrentThreadApplicationThread();

        //! Info about current thread, for debug messages
        static QString currentThreadInfo();

        //! Call in object's thread if not already in object's thread
        static bool callInObjectThread(QObject *object, std::function<void()> callFunct);
    };
} // ns

#endif // guard
