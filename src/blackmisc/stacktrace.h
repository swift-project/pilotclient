/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_STACKTRACE_H
#define BLACKMISC_STACKTRACE_H

#include "blackmisc/blackmiscexport.h"
#include <QStringList>

namespace BlackMisc
{
    /*!
     * Returns a stack trace of the current thread of execution as a list of function names.
     *
     * Returns a dummy list in release build.
     */
    BLACKMISC_EXPORT QStringList getStackTrace();

    /*!
     * Returns a stack trace of the current thread of execution as a list of function names.
     */
    BLACKMISC_EXPORT QStringList getStackTraceAlways();
}

#endif
