/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
