/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "utils.h"
#include <XPMPMultiplayerCSL.h>
#include <XPLM/XPLMUtilities.h>
#include <string>
#include <sstream>

namespace XSwiftBus
{
    std::string g_xplanePath;
    std::string g_sep;

    //! Init global xplane path
    void initXPlanePath()
    {
        if (!g_xplanePath.empty() && !g_sep.empty()) {}

        char xplanePath[512];
        XPLMGetSystemPath(xplanePath);
#ifdef Q_OS_MAC
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
        (void) line;
        (void) type;
        (void) filePath;

        assert(!filePath.empty());
        std::ostringstream ss;

        ss << "xswiftbus";
        ss << ' ';

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
        case FatalMsg:
            ss << "Fatal";
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
#endif

        ss << message;
        ss << "\n";

        std::string buffer = ss.str();
        XPLMDebugString(buffer.c_str());
    }

}

//! \endcond
