// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_CPUTIME_H
#define SWIFT_MISC_CPUTIME_H

#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    /*!
     * Get the time in milliseconds that the CPU has spent executing the current process.
     * This is the sum of getThreadCpuTimeMs() for all threads.
     */
    SWIFT_MISC_EXPORT int getProcessCpuTimeMs();

    /*!
     * Get the time in milliseconds that the CPU has spent executing the current thread.
     */
    SWIFT_MISC_EXPORT int getThreadCpuTimeMs();
} // namespace swift::misc

#endif // SWIFT_MISC_CPUTIME_H
