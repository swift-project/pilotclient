/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/stacktrace.h"
#include <QMutex>
#include <array>
#if defined(Q_OS_WIN32)
#   include <windows.h>
#   pragma warning(push)
#   pragma warning(disable:4091)
#   include <dbghelp.h>
#   pragma warning(pop)
#elif defined(Q_CC_GNU)
#   include <execinfo.h>
#endif

namespace BlackMisc
{

#if defined(QT_NO_DEBUG)
    QStringList getStackTrace()
    {
        return { "No stack trace with release build" };
    }
#elif defined(Q_OS_WIN32)
    QStringList getStackTrace()
    {
        static QMutex mutex;
        QMutexLocker lock(&mutex);

        auto process = GetCurrentProcess();
        SymInitialize(process, nullptr, true);

        std::array<void*, 100> stack;
        auto frames = CaptureStackBackTrace(1, static_cast<DWORD>(stack.size()), stack.data(), nullptr);

        struct
        {
            SYMBOL_INFO info;
            std::array<char, 256> name;
        } symbol;
        symbol.info.MaxNameLen = static_cast<ULONG>(symbol.name.size() - 1);
        symbol.info.SizeOfStruct = sizeof(symbol.info);

        QStringList result;
        for (int i = 0; i < frames; ++i)
        {
            DWORD displacement = 0;
            IMAGEHLP_LINE64 line;
            line.SizeOfStruct = sizeof(line);
            SymFromAddr(process, reinterpret_cast<quintptr>(stack[i]), nullptr, &symbol.info);
            SymGetLineFromAddr64(process, reinterpret_cast<quintptr>(stack[i]), &displacement, &line);

            result.push_back(QString(symbol.info.Name) + " line " + QString::number(line.LineNumber));
        }
        return result;
    }
#elif defined(Q_CC_GNU)
    QStringList getStackTrace()
    {
        std::array<void*, 100> stack;
        auto frames = backtrace(stack.data(), stack.size());
        auto *symbols = backtrace_symbols(stack.data(), frames);

        QStringList result;
        QString symbol;
        char *demangled = nullptr;
        size_t size = 0;
        for (int i = 0; i < frames; ++i)
        {
            symbol = symbols[i];
            int end = symbol.indexOf(' ');
            if (end > 0) { symbol.truncate(end); }

            demangled = abi::__cxa_demangle(qPrintable(symbol), demangled, &size, nullptr);
            result.push_back(demangled);
        }
        free(symbols);
        free(demangled);
        return result;
    }
#else
    QStringList getStackTrace()
    {
        return { "No stack trace on this platform" };
    }
#endif

}
