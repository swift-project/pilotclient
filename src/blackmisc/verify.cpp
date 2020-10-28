/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/verify.h"
#include "blackmisc/crashhandler.h"
#include "blackmisc/logmessage.h"
#include <QtGlobal>

#ifdef BLACK_USE_CRASHPAD
#include "crashpad/client/simulate_crash.h"
#endif

#if defined(Q_CC_MSVC)
#include <intrin.h>
#include <Windows.h>
#endif

#if defined(Q_CC_CLANG)
#if __has_builtin(__builtin_debugtrap)
#define BLACK_BUILTIN_DEBUGTRAP __builtin_debugtrap
#elif __has_builtin(__builtin_debugger)
#define BLACK_BUILTIN_DEBUGTRAP __builtin_debugger
#endif
#endif

namespace BlackMisc
{
    namespace Private
    {
        // cppcheck-suppress unusedFunction
        void failedVerify(const char *condition, const char *filename, int line, const char *context, const char *message, bool audit)
        {
            Q_UNUSED(condition)
            Q_UNUSED(filename)
            Q_UNUSED(line)
            Q_UNUSED(context)
            Q_UNUSED(message)
            Q_UNUSED(audit)

#if defined(QT_DEBUG)
#   if defined(Q_CC_MSVC)
            if (!audit || IsDebuggerPresent())
            {
                __debugbreak();
                return;
            }
#   elif defined(BLACK_BUILTIN_DEBUGTRAP)
            BLACK_BUILTIN_DEBUGTRAP();
#   elif defined(Q_PROCESSOR_X86)
            __asm__ volatile("int $0x03");
#   elif defined(Q_PROCESSOR_ARM)
            __asm__ volatile(".inst 0xe7f001f0");
#   elif defined(Q_OS_UNIX)
            raise(SIGTRAP);
#   else
            Q_ASSERT(false);
#   endif
#endif

#if defined(QT_NO_DEBUG) || defined(Q_CC_MSVC)
            if (context && message)
            {
                CLogMessage(CLogCategories::verification()).warning(u"Failed to verify: %1 (%2 in %3) in %4 line %5") << condition << message << context << filename << line;
            }
            else
            {
                CLogMessage(CLogCategories::verification()).warning(u"Failed to verify: %1 in %2 line %3") << condition << filename << line;
            }
#   if defined(BLACK_USE_CRASHPAD)
            CRASHPAD_SIMULATE_CRASH();
#   endif
#endif
        }
    }
}
