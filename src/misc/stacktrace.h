// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_STACKTRACE_H
#define SWIFT_MISC_STACKTRACE_H

#include "misc/swiftmiscexport.h"
#include <QStringList>

namespace swift::misc
{
    /*!
     * Returns a stack trace of the current thread of execution as a list of function names.
     *
     * Returns a dummy list in release build.
     */
    SWIFT_MISC_EXPORT QStringList getStackTrace();

    /*!
     * Returns a stack trace of the current thread of execution as a list of function names.
     */
    SWIFT_MISC_EXPORT QStringList getStackTraceAlways();
}

#endif
