/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_XSWIFTBUS_UTILS_H
#define BLACKSIM_XSWIFTBUS_UTILS_H

#include <vector>
#include <string>

namespace XSwiftBus
{
    //! Absolute xplane path
    extern std::string g_xplanePath;

    //! Platform specific dir separator
    extern std::string g_sep;

    //! Init global xplane path
    void initXPlanePath();

    //! Returns the directory name of a given file path
    std::string getDirName(const std::string &filePath);

    //! Returns the filename (including extension) of a given file path
    std::string getFileName(const std::string &filePath);

    //! Returns the filename without extension of a given file path
    std::string getBaseName(const std::string &filePath);

    //! Splits the given string maximal maxSplitCount times and returns the tokens
    //! The size of the returned vector is up to maxSplitCount + 1 or less
    std::vector<std::string> split(const std::string &str, size_t maxSplitCount = 0);

    //! Simple logger class.
    //! Don't use it directly, but the _LOG macros instead
    class Logger
    {
    public:
        //! Message type
        enum MsgType { DebugMsg, WarningMsg, FatalMsg, InfoMsg };

        Logger() = delete;

        //! Print message to X-Plane log
        static void print(const std::string &filePath, int line, MsgType type, const std::string &message);
    };

    //! Logger convenience macros
    //! @{
    #define DEBUG_LOG(msg) Logger::print(__FILE__, __LINE__, Logger::DebugMsg, msg)
    #define INFO_LOG(msg) Logger::print(__FILE__, __LINE__, Logger::InfoMsg, msg)
    #define WARNING_LOG(msg) Logger::print(__FILE__, __LINE__, Logger::WarningMsg, msg)
    //! @}
}

#endif // guard
