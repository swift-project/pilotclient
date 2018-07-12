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

    std::string getDirName(const std::string &filePath)
    {
        std::string seperator = "/\\";
        std::size_t sepPos = filePath.find_last_of(seperator);
        if (sepPos != std::string::npos)
        {
            std::string dirPath = filePath.substr(0, sepPos);
            return getFileName(dirPath);
        }
        else
        {
            return {};
        }
    }

    std::string getFileName(const std::string &filePath)
    {
        std::string seperator = "/\\";
        std::size_t sepPos = filePath.find_last_of(seperator);
        if (sepPos != std::string::npos)
        {
            return filePath.substr(sepPos + 1, filePath.size() - 1);
        }
        else
        {
            return filePath;
        }
    }

    std::string getBaseName(const std::string &filePath)
    {
        std::string seperator = ".";
        std::string fileName = getFileName(filePath);
        std::size_t sepPos = fileName.find(seperator);
        if (sepPos != std::string::npos)
        {
            return fileName.substr(0, sepPos);
        }
        else
        {
            return fileName;
        }
    }

    std::vector<std::string> split(const std::string &str, size_t maxSplitCount)
    {
        std::string s(str);
        std::string delimiter = " ";
        size_t pos = 0;
        std::vector<std::string> tokens;
        while ((pos = s.find(delimiter)) != std::string::npos)
        {
            tokens.push_back(s.substr(0, pos));
            s.erase(0, pos + delimiter.length());
            if (tokens.size() == maxSplitCount) { break; }
        }
        tokens.push_back(s);
        return tokens;
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
