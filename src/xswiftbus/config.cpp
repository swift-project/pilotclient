/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "config.h"
#include "utils.h"
#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>

namespace XSwiftBus
{
    //! Case insensitive string compare
    bool stringCompare(const std::string & str1, const std::string &str2)
    {
        if (str1.size() != str2.size()) { return false; }
        return std::equal(str1.begin(), str1.end(), str2.begin(), [](const char & c1, const char & c2)
        {
            return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
        });
    }

    CConfig::CConfig()
    {}

    CConfig::~CConfig() {}

    void CConfig::parse()
    {
        std::ifstream configFile(m_filePath);
        if (! configFile.is_open()) { return; }

        std::string line;
        int lineNo = 1;

        while (std::getline(configFile, line))
        {
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            if (line.empty() || line[0] == '#') { continue; }

            auto delimiterPos = line.find("=");
            if (delimiterPos == std::string::npos)
            {
                WARNING_LOG("xswiftbus.conf line " + std::to_string(lineNo) + ": Skipping invalid line!");
                lineNo++;
                continue;
            }

            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            if (key.empty() || value.empty())
            {
                WARNING_LOG("xswiftbus.conf line " + std::to_string(lineNo) + ": Skipping invalid line!");
                lineNo++;
                continue;
            }

            bool valid = true;
            if (stringCompare(key, "dbusMode")) { valid = parseDBusMode(value); }
            else if (stringCompare(key, "dbusAddress")) { valid = parseDBusAddress(value); }
            else if (stringCompare(key, "dbusPort")) { valid = parseDBusPort(value); }
            else
            {
                WARNING_LOG("xswiftbus.conf line " + std::to_string(lineNo) + ": Unknown variable " + value + "!");
                lineNo++;
                continue;
            }

            if (! valid)
            {
                WARNING_LOG("xswiftbus.conf line " + std::to_string(lineNo) + ": Skipping invalid line!");
                lineNo++;
                continue;
            }

            lineNo++;
        }
    }

    void CConfig::print()
    {
        DEBUG_LOG("XSwiftBus configuration:");
        DEBUG_LOG("DBus mode: " + dbusModeToString(m_dbusMode));
        DEBUG_LOG("DBus server address: " + m_dbusAddress);
        DEBUG_LOG("DBus server port: " + std::to_string(m_dbusPort));
    }

    bool CConfig::parseDBusMode(const std::string &value)
    {
        if (stringCompare(value, "session")) { m_dbusMode = CConfig::DBusSession; return true; }
        else if (stringCompare(value, "P2P")) { m_dbusMode = CConfig::DBusP2P; return true; }
        else { return false; }
    }

    bool CConfig::parseDBusAddress(const std::string &value)
    {
        m_dbusAddress = value;
        return true;
    }

    bool CConfig::parseDBusPort(const std::string &value)
    {
        int port = 0;
        try
        {
            port = std::stoi(value);
        }
        catch (...)
        {
            return false;
        }

        if (port < 0 || port > 65535) { return false; }
        m_dbusPort = port;
        return true;
    }

    std::string CConfig::dbusModeToString(DBusMode mode)
    {
        switch (mode)
        {
        case DBusSession: return "Session";
        case DBusP2P: return "P2P";
        }

        return {};
    }
} // ns
