// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/cputime.h"
#include <QtGlobal>

#if defined(Q_OS_WIN32)
#    include <windows.h>
#elif defined(Q_OS_UNIX)
#    include <time.h>
#endif

namespace BlackMisc
{

#if defined(Q_OS_WIN32)

    static int getCpuTimeMs(const FILETIME &kernelTime, const FILETIME &userTime)
    {
        const ULARGE_INTEGER kernel { { kernelTime.dwLowDateTime, kernelTime.dwHighDateTime } };
        const ULARGE_INTEGER user { { userTime.dwLowDateTime, userTime.dwHighDateTime } };
        const quint64 usecs = (kernel.QuadPart + user.QuadPart) / 10ull;
        return static_cast<int>(usecs / 1000ull);
    }
    int getProcessCpuTimeMs()
    {
        FILETIME creationTime, exitTime, kernelTime, userTime;
        GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
        return getCpuTimeMs(kernelTime, userTime);
    }
    int getThreadCpuTimeMs()
    {
        FILETIME creationTime, exitTime, kernelTime, userTime;
        GetThreadTimes(GetCurrentThread(), &creationTime, &exitTime, &kernelTime, &userTime);
        return getCpuTimeMs(kernelTime, userTime);
    }

#elif defined(Q_OS_UNIX)

    static int getCpuTimeMs(const timespec &ts)
    {
        const double secs = static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) / 1e9;
        return static_cast<int>(secs * 1000);
    }
    int getProcessCpuTimeMs()
    {
        timespec ts {};
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
        return getCpuTimeMs(ts);
    }
    int getThreadCpuTimeMs()
    {
        timespec ts {};
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
        return getCpuTimeMs(ts);
    }

#else // Q_OS_UNIX

    int getProcessCpuTimeMs()
    {
        return 0; // not implemented
    }
    int getThreadCpuTimeMs()
    {
        return 0; // not implemented
    }

#endif

}
