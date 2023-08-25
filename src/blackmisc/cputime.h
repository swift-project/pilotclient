// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_CPUTIME_H
#define BLACKMISC_CPUTIME_H

#include "blackmisc/blackmiscexport.h"

namespace BlackMisc
{
    /*!
     * Get the time in milliseconds that the CPU has spent executing the current process.
     * This is the sum of getThreadCpuTimeMs() for all threads.
     */
    BLACKMISC_EXPORT int getProcessCpuTimeMs();

    /*!
     * Get the time in milliseconds that the CPU has spent executing the current thread.
     */
    BLACKMISC_EXPORT int getThreadCpuTimeMs();
}

#endif
