/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "config.h"
#include "utils.h"
#include "blackmisc/simulation/xplane/qtfreeutils.h"

#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace BlackMisc::Simulation::XPlane::QtFreeUtils;

namespace XSwiftBus
{
    CConfig::CConfig()
    {}

    CConfig::~CConfig() {}

    void CConfig::parse()
    {
        std::ifstream configFile(m_filePath);
        if (!configFile.is_open()) { return; }

        std::string line;
        for (int lineNo = 1; std::getline(configFile, line); ++lineNo)
        {
            line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
            if (line.empty() || line[0] == '#') { continue; }

            auto delimiterPos = line.find("=");
            if (delimiterPos == std::string::npos)
            {
                WARNING_LOG("xswiftbus.conf line " + std::to_string(lineNo) + ": Skipping invalid line!");
                continue;
            }

            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            if (key.empty() || value.empty())
            {
                WARNING_LOG("xswiftbus.conf line " + std::to_string(lineNo) + ": Skipping invalid line!");
                continue;
            }

            bool valid = true;
            if (stringCompareCaseInsensitive(key, "dbusMode")) { valid = parseDBusMode(value); }
            else if (stringCompareCaseInsensitive(key, "dbusAddress")) { valid = parseDBusAddress(value); }
            else if (stringCompareCaseInsensitive(key, "dbusPort")) { valid = parseDBusPort(value); }
            else if (stringCompareCaseInsensitive(key, "debug")) { valid = parseDebug(value); }
            else if (stringCompareCaseInsensitive(key, "tcas")) { valid = parseTcas(value); }
            else
            {
                WARNING_LOG("xswiftbus.conf line " + std::to_string(lineNo) + ": Unknown variable " + key + "!");
                continue;
            }

            if (!valid)
            {
                WARNING_LOG("xswiftbus.conf line " + std::to_string(lineNo) + ": Skipping invalid line!");
                continue;
            }
        }
    }

    void CConfig::print()
    {
        DEBUG_LOG("XSwiftBus configuration:");
        DEBUG_LOG("DBus mode: " + dbusModeToString(m_dbusMode));
        DEBUG_LOG("DBus server address: " + m_dbusAddress);
        DEBUG_LOG("DBus server port: " + std::to_string(m_dbusPort));
    }

    bool CConfig::writeConfig(bool tcas, bool debug)
    {
        setTcasEnabled(tcas);
        setDebugMode(debug);
        return writeConfigFile();
    }

    bool CConfig::writeConfigFile() const
    {
        std::ofstream configFile(m_filePath, std::ofstream::out | std::ofstream::trunc);
        if (!configFile.is_open()) { return false; }

        // this code should be similar to CXSwiftBusConfigWriter
        configFile << "# DBus Mode - Options: p2p, session" << std::endl;
        configFile << "dbusMode = " << toLower(dbusModeToString(m_dbusMode)) << std::endl;
        configFile << std::endl;
        configFile << "# DBus server address - relevant for P2P mode only" << std::endl;
        configFile << "dbusAddress = " << m_dbusAddress << std::endl;
        configFile << std::endl;
        configFile << "# DBus server port - relevant for P2P mode only" << std::endl;
        configFile << "dbusPort = " << m_dbusPort << std::endl;
        configFile << std::endl;
        configFile << "# Render phase debugging - to help diagnose crashes" << std::endl;
        configFile << "debug = " << boolToOnOff(m_debug) << std::endl;
        configFile << std::endl;
        configFile << "# TCAS traffic - to disable in case of crashes" << std::endl;
        configFile << "tcas = " << boolToOnOff(m_tcas) << std::endl;

        // for info
        const auto clockNow = std::chrono::system_clock::now();
        const time_t now = std::chrono::system_clock::to_time_t(clockNow);
        struct tm tms;
#if defined(IBM)
        localtime_s(&tms, &now);
#else
        localtime_r(&now, &tms);
#endif
        configFile << std::endl;
        configFile << "# Updated by XSwiftBus plugin " << std::put_time(&tms, "%T");
        configFile << std::endl;
        configFile.close();
        return true;
    }

    bool CConfig::parseDBusMode(const std::string &value)
    {
        if (stringCompareCaseInsensitive(value, "session"))
        {
            m_dbusMode = CConfig::DBusSession;
            return true;
        }
        else if (stringCompareCaseInsensitive(value, "P2P"))
        {
            m_dbusMode = CConfig::DBusP2P;
            return true;
        }
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

    bool CConfig::parseDebug(const std::string &value)
    {
        if (stringCompareCaseInsensitive(value, "on"))
        {
            m_debug = true;
        }
        else
        {
            m_debug = false;
        }
        return true;
    }

    bool CConfig::parseTcas(const std::string &value)
    {
        m_tcas = stringCompareCaseInsensitive(value, "on");
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

    std::string CConfig::boolToOnOff(bool on)
    {
        if (on) { return "on"; }
        return "off";
    }
} // ns
