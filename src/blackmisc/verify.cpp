/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/verify.h"
#include "blackmisc/logmessage.h"

#include <QtGlobal>

#if defined(Q_CC_MSVC)
#include <intrin.h>
#elif defined(Q_OS_UNIX)
#endif

#if defined(Q_CC_CLANG)
#if __has_builtin(__builtin_debugger)
#define BLACK_HAS_BUILTIN_DEBUGGER
#endif
#endif

namespace BlackMisc
{
    namespace Private
    {
        // cppcheck-suppress unusedFunction
        void failedVerify(const char *condition, const char *filename, int line, const char *context, const char *message)
        {
            Q_UNUSED(condition);
            Q_UNUSED(filename);
            Q_UNUSED(line);
            Q_UNUSED(context);
            Q_UNUSED(message);
#if defined(QT_NO_DEBUG)
            if (context && message)
            {
                CLogMessage(CLogCategory::verification()).warning("Failed to verify: %1 (%2 in %3) in %4 line %5") << condition << message << context << filename << line;
            }
            else
            {
                CLogMessage(CLogCategory::verification()).warning("Failed to verify: %1 in %2 line %3") << condition << filename << line;
            }
#elif defined(Q_CC_MSVC)
            __debugbreak();
#elif defined(BLACK_HAS_BUILTIN_DEBUGGER)
            __builtin_debugger();
#elif defined(Q_PROCESSOR_X86)
            __asm__ volatile("int $0x03");
#elif defined(Q_PROCESSOR_ARM)
            __asm__ volatile(".inst 0xe7f001f0");
#elif defined(Q_OS_UNIX)
            raise(SIGTRAP);
#else
            Q_ASSERT(false);
#endif
        }
    }
}
