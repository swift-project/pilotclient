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
