// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/stacktrace.h"

#include <array>
#include <cstdlib>
#include <mutex>

#include <QByteArray>
#include <QLatin1String>
#include <QMutexLocker>
#include <QStringBuilder>

#if defined(Q_CC_MSVC)
#    include <Windows.h>

#    pragma warning(push)
#    pragma warning(disable : 4091)
#    include <DbgHelp.h>

#    pragma warning(pop)
#elif defined(Q_OS_WIN) && defined(Q_CC_GNU)
#    include <dbghelp.h>
#    include <windows.h>
#elif defined(Q_CC_GNU)
#    include <cxxabi.h>
#    include <execinfo.h>

#    include <cstring>
#endif

namespace swift::misc
{

#if defined(QT_DEBUG)
    QStringList getStackTrace() { return getStackTraceAlways(); }
#else
    QStringList getStackTrace() { return { "No stack trace with release build" }; }
#endif

#if defined(Q_OS_WIN32)
    QStringList getStackTraceAlways()
    {
        static QMutex mutex;
        QMutexLocker lock(&mutex);

        static std::once_flag flag;
        std::call_once(flag, [] {
            SymInitialize(GetCurrentProcess(), nullptr, true);
            std::atexit([] { SymCleanup(GetCurrentProcess()); });
        });

        auto process = GetCurrentProcess();
        SymRefreshModuleList(process);

        using stackarray = std::array<void *, 100>;
        stackarray stack;
        auto frames = CaptureStackBackTrace(1, static_cast<DWORD>(stack.size()), stack.data(), nullptr);

        struct
        {
            SYMBOL_INFO info;
            std::array<char, 256> name;
        } symbol;
        symbol.info.MaxNameLen = static_cast<ULONG>(symbol.name.size() - 1);
        symbol.info.SizeOfStruct = sizeof(symbol.info);

        QStringList result;
        for (stackarray::size_type i = 0; i < frames; ++i)
        {
            DWORD displacement = 0;
            IMAGEHLP_LINE64 line;
            line.SizeOfStruct = sizeof(line);
            SymFromAddr(process, reinterpret_cast<quintptr>(stack[i]), nullptr, &symbol.info);
            SymGetLineFromAddr64(process, reinterpret_cast<quintptr>(stack[i]), &displacement, &line);

            result.push_back(QLatin1String(symbol.info.Name) % u" line " % QString::number(line.LineNumber));
        }
        return result;
    }
#elif defined(Q_CC_GNU)
    QStringList getStackTraceAlways()
    {
        std::array<void *, 100> stack;
        auto frames = backtrace(stack.data(), stack.size());
        auto *symbols = backtrace_symbols(stack.data(), frames);

        QStringList result;
        char *demangled = nullptr;
        size_t size = 0;
        for (int i = 0; i < frames; ++i)
        {
            // Using C string functions to avoid unnecessary dynamic memory allocation
            auto *basename = std::strrchr(symbols[i], '/');
            if (!basename) { continue; }
            basename++;
            auto *symbol = std::strchr(basename, '(');
            if (!symbol) { continue; }
            auto *basenameEnd = symbol++;
            auto *end = std::strrchr(symbol, ')');
            if (!end) { continue; }
            auto *offset = std::strrchr(symbol, '+');
            auto *symbolEnd = offset ? offset++ : end;

            auto *temp = demangled; // avoid leaking memory if __cxa_demangle returns nullptr

            demangled =
                abi::__cxa_demangle(QByteArray(symbol, symbolEnd - symbol).constData(), demangled, &size, nullptr);

            if (demangled)
            {
                result.push_back(QLatin1String(demangled) % u' ' % u'(' %
                                 QLatin1String(basename, basenameEnd - basename) % u' ' %
                                 QLatin1String(symbol, end - symbol) % u')');
            }
            else
            {
                result.push_back(u'(' % QLatin1String(basename, basenameEnd - basename) % u' ' %
                                 QLatin1String(symbol, end - symbol) % u')');
                demangled = temp;
            }
        }
        free(symbols);
        free(demangled);
        return result;
    }
#else
    // cppcheck-suppress unusedFunction
    QStringList getStackTraceAlways() { return { "No stack trace on this platform" }; }
#endif

} // namespace swift::misc
