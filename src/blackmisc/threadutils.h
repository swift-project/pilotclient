/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CTHREADUTILS_H
#define BLACKMISC_CTHREADUTILS_H

#include "blackmisc/blackmiscexport.h"

class QObject;

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
        static bool isCurrentThreadObjectThread(QObject *toBeTested);

        //! Is the application thread the QObject's thread?
        //! \remarks can be used as ASSERT check for threaded objects
        static bool isApplicationThreadObjectThread(QObject *toBeTested);

        //! Is the current thread the Application thread?
        //! \remarks can be used as ASSERT check for threaded objects
        static bool isCurrentThreadApplicationThread();
    };
} // ns

#endif // guard
