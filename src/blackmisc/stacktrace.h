// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
