// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#ifndef NOMINMAX
#    define NOMINMAX
#endif

#include "utils.h"

#include <XPLM/XPLMPlugin.h>
#include <XPLM/XPLMUtilities.h>
#include <XPMPMultiplayer.h>

#include <cassert>
#include <sstream>
#include <string>

#ifdef APL
#    include <Carbon/Carbon.h>
#endif

namespace XSwiftBus
{
    std::string g_xplanePath;
    std::string g_sep;

#ifdef APL
    int HFS2PosixPath(const char *path, char *result, int resultLen);
#endif

    //! Init global xplane path
    void initXPlanePath()
    {
        if (!g_xplanePath.empty() && !g_sep.empty()) { return; }

        char xplanePath[512];
        XPLMGetSystemPath(xplanePath);
#ifdef APL
        if (XPLMIsFeatureEnabled("XPLM_USE_NATIVE_PATHS") == 0)
        {
            HFS2PosixPath(xplanePath, xplanePath, sizeof(xplanePath));
        }
        g_sep = "/";
#else
        g_sep = XPLMGetDirectorySeparator();
#endif
        g_xplanePath = xplanePath;
    }

    void Logger::print(const std::string &filePath, int line, MsgType type, const std::string &message)
    {
        (void)line;
        (void)type;
        (void)filePath;

        assert(!filePath.empty());
        std::ostringstream ss;
        ss << "xswiftbus: ";

#if defined(XSWIFTBUS_ENABLE_TRACE_LOG)
        switch (type)
        {
        case DebugMsg:
            ss << "Debug";
            break;
        case InfoMsg:
            ss << "Info";
            break;
        case WarningMsg:
            ss << "Warning";
            break;
        case ErrorMsg:
            ss << "Error";
            break;
        }
        ss << ' ';

        std::string seperator = "/\\";
        std::size_t sepPos = filePath.find_last_of(seperator);
        if (sepPos != std::string::npos)
        {
            ss << filePath.substr(sepPos + 1, filePath.size() - 1);
        }
        else
        {
            ss << filePath;
        }
        ss << ' ';

        ss << line;
        ss << " : ";
#endif // XSWIFTBUS_ENABLE_TRACE_LOG

        ss << message;
        ss << "\n";

        const std::string buffer = ss.str();
        XPLMDebugString(buffer.c_str());
    }

#ifdef APL
    template <typename T>
    struct CFSmartPtr
    {
        CFSmartPtr(T p) : p_(p) {}
        ~CFSmartPtr()
        {
            if (p_) CFRelease(p_);
        }
        operator T() { return p_; }
        T p_;
    };

#    ifdef __clang__
#        pragma clang diagnostic push
#        pragma clang diagnostic ignored "-Wdeprecated-declarations"
#    endif

    int HFS2PosixPath(const char *path, char *result, int resultLen)
    {
        bool is_dir = (path[strlen(path) - 1] == ':');

        CFSmartPtr<CFStringRef> inStr(CFStringCreateWithCString(kCFAllocatorDefault, path, kCFStringEncodingMacRoman));
        if (inStr == nullptr) return -1;

        CFSmartPtr<CFURLRef> url(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLHFSPathStyle, 0));
        if (url == nullptr) return -1;

        CFSmartPtr<CFStringRef> outStr(CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle));
        if (outStr == nullptr) return -1;

        if (!CFStringGetCString(outStr, result, resultLen, kCFStringEncodingMacRoman))
            return -1;

        if (is_dir) strcat(result, "/");

        return 0;
    }

#    ifdef __clang__
#        pragma clang diagnostic pop
#    endif
#endif // APL
} // namespace XSwiftBus

//! \endcond
