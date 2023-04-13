/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_CONFIG_H
#define BLACKSIM_XSWIFTBUS_CONFIG_H

#include <string>

namespace XSwiftBus
{
    /*!
     * XSwiftBus configuration class
     */
    class CConfig
    {
    public:
        //! DBus Server Mode
        enum DBusMode
        {
            DBusSession,
            DBusP2P
        };

        //! Constructor.
        CConfig();

        //! Destructor;
        ~CConfig();

        //! Set config file path
        void setFilePath(const std::string &filePath) { m_filePath = filePath; }

        //! Load and parse config file
        void parse();

        //! Print the current configuration to the X-Plane log
        void print();

        //! Get current DBus mode
        DBusMode getDBusMode() const { return m_dbusMode; }

        //! Get current DBus server address
        std::string getDBusAddress() const { return m_dbusAddress; }

        //! Get current DBus server port
        int getDBusPort() const { return m_dbusPort; }

        //! Get debug on/off
        bool getDebugMode() const { return m_debug; }

        //! Set debug mode
        void setDebugMode(bool on) { m_debug = on; }

        //! Get TCAS traffic on/off
        bool getTcasEnabled() const { return m_tcas; }

        //! Set TCAS traffic on/off
        void setTcasEnabled(bool on) { m_tcas = on; }

        //! Update and write config file
        bool writeConfig(bool tcas, bool debug);

    private:
        bool parseDBusMode(const std::string &value);
        bool parseDBusAddress(const std::string &value);
        bool parseDBusPort(const std::string &value);
        bool parseDebug(const std::string &value);
        bool parseTcas(const std::string &value);
        bool writeConfigFile() const;

        static std::string dbusModeToString(DBusMode mode);
        static std::string boolToOnOff(bool on);

        std::string m_filePath;
        DBusMode m_dbusMode = DBusP2P;
        std::string m_dbusAddress = "127.0.0.1";
        int m_dbusPort = 45001;
        bool m_debug = false;
        bool m_tcas = true;
    };
} // ns

#endif
