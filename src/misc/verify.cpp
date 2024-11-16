// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/verify.h"

#include <QLoggingCategory>
#include <QString>
#include <QtGlobal>

#include "misc/logcategories.h"

#ifdef SWIFT_USE_CRASHPAD
#    include "crashpad/client/simulate_crash.h"
#endif

#if defined(Q_CC_MSVC)
#    include <Windows.h>
#    include <intrin.h>
#endif

#if defined(Q_CC_CLANG)
#    if __has_builtin(__builtin_debugtrap)
#        define SWIFT_BUILTIN_DEBUGTRAP __builtin_debugtrap
#    elif __has_builtin(__builtin_debugger)
#        define SWIFT_BUILTIN_DEBUGTRAP __builtin_debugger
#    endif
#endif

namespace swift::misc::private_ns
{
    // cppcheck-suppress unusedFunction
    void failedVerify(const char *condition, const char *filename, int line, const char *context, const char *message,
                      bool audit)
    {
        Q_UNUSED(condition)
        Q_UNUSED(filename)
        Q_UNUSED(line)
        Q_UNUSED(context)
        Q_UNUSED(message)
        Q_UNUSED(audit)

#if defined(QT_DEBUG)
#    if defined(Q_CC_MSVC)
        if (!audit || IsDebuggerPresent())
        {
            __debugbreak();
            return;
        }
#    elif defined(SWIFT_BUILTIN_DEBUGTRAP)
        SWIFT_BUILTIN_DEBUGTRAP();
#    elif defined(Q_PROCESSOR_X86)
        __asm__ volatile("int $0x03");
#    elif defined(Q_PROCESSOR_ARM)
        __asm__ volatile(".inst 0xe7f001f0");
#    elif defined(Q_OS_UNIX)
        raise(SIGTRAP);
#    else
        Q_ASSERT(false);
#    endif
#endif

#if defined(QT_NO_DEBUG) || defined(Q_CC_MSVC)
        QString log;
        if (context && message)
        {
            log = QStringLiteral("Failed to verify: %1 (%2 in %3) in %4 line %5")
                      .arg(condition, message, context, filename, QString::number(line));
        }
        else
        {
            log = QStringLiteral("Failed to verify: %1 in %2 line %3").arg(condition, filename, QString::number(line));
        }
        QMessageLogger().warning(QLoggingCategory(qPrintable(CLogCategories::verification()))) << log;
#    if defined(SWIFT_USE_CRASHPAD)
        CRASHPAD_SIMULATE_CRASH();
#    endif
#endif
    }
} // namespace swift::misc::private_ns
