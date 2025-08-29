// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIM_XSWIFTBUS_UTILS_H
#define SWIFT_SIM_XSWIFTBUS_UTILS_H

#include <string>
#include <vector>

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
        enum MsgType
        {
            DebugMsg,
            WarningMsg,
            ErrorMsg,
            InfoMsg
        };

        Logger() = delete;

        //! Print message to X-Plane log
        static void print(const std::string &filePath, int line, MsgType type, const std::string &message);
    };
} // namespace XSwiftBus

//! @{
//! Logger convenience macros
#define DEBUG_LOG(msg) (XSwiftBus::Logger::print(__FILE__, __LINE__, XSwiftBus::Logger::DebugMsg, (msg)))
#define DEBUG_LOG_C(msg, doLog)                                                                                        \
    ((doLog) ? XSwiftBus::Logger::print(__FILE__, __LINE__, XSwiftBus::Logger::DebugMsg, (msg)) : static_cast<void>(0))
#define INFO_LOG(msg) (XSwiftBus::Logger::print(__FILE__, __LINE__, XSwiftBus::Logger::InfoMsg, (msg)))
#define WARNING_LOG(msg) (XSwiftBus::Logger::print(__FILE__, __LINE__, XSwiftBus::Logger::WarningMsg, (msg)))
#define ERROR_LOG(msg) (XSwiftBus::Logger::print(__FILE__, __LINE__, XSwiftBus::Logger::ErrorMsg, (msg)))
//! @}

#endif // SWIFT_SIM_XSWIFTBUS_UTILS_H
