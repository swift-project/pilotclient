/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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

    //! Simple logger class.
    //! Don't use it directly, but the _LOG macros instead
    class Logger
    {
    public:
        //! Message type
        enum MsgType { DebugMsg, WarningMsg, ErrorMsg, InfoMsg };

        Logger() = delete;

        //! Print message to X-Plane log
        static void print(const std::string &filePath, int line, MsgType type, const std::string &message);
    };
}

//! @{
//! Logger convenience macros
// *INDENT-OFF*
#define DEBUG_LOG(msg) (XSwiftBus::Logger::print(__FILE__, __LINE__, XSwiftBus::Logger::DebugMsg, (msg)))
#define DEBUG_LOG_C(msg, doLog) ((doLog) ? XSwiftBus::Logger::print(__FILE__, __LINE__, XSwiftBus::Logger::DebugMsg, (msg)) : static_cast<void>(0))
#define INFO_LOG(msg) (XSwiftBus::Logger::print(__FILE__, __LINE__, XSwiftBus::Logger::InfoMsg, (msg)))
#define WARNING_LOG(msg) (XSwiftBus::Logger::print(__FILE__, __LINE__, XSwiftBus::Logger::WarningMsg, (msg)))
#define ERROR_LOG(msg) (XSwiftBus::Logger::print(__FILE__, __LINE__, XSwiftBus::Logger::ErrorMsg, (msg)))
// *INDENT-ON*
//! @}

#endif // guard
