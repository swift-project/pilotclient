// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_COREFACADECONFIG_H
#define SWIFT_CORE_COREFACADECONFIG_H

#include <QString>

#include "core/swiftcoreexport.h"

namespace swift::core
{
    //! Helper to correctly run a context
    class SWIFT_CORE_EXPORT CCoreFacadeConfig
    {
    public:
        //! How to handle a given context
        enum ContextMode
        {
            NotUsed, //!< during shutdown or not used at all
            Local, //!< in same process
            LocalInDBusServer, //!< in same process, also registered in DBus, will be accessed by proxy objects too
            Remote //!< proxy object
        };

    private:
        ContextMode m_application;
        ContextMode m_audio;
        ContextMode m_network;
        ContextMode m_ownAircraft;
        ContextMode m_simulator;
        QString m_dbusAddress; //!< for boot strapping

    public:
        //! Constructor
        CCoreFacadeConfig(ContextMode allTheSame = NotUsed, const QString &dbusBootstrapAddress = "")
            : m_application(allTheSame), m_audio(allTheSame), m_network(allTheSame), m_ownAircraft(allTheSame),
              m_simulator(allTheSame), m_dbusAddress(dbusBootstrapAddress)
        {}

        //! Constructor
        CCoreFacadeConfig(ContextMode application, ContextMode audio, ContextMode network, ContextMode ownAircraft,
                          ContextMode simulator, const QString &dbusBootstrapAddress = "")
            : m_application(application), m_audio(audio), m_network(network), m_ownAircraft(ownAircraft),
              m_simulator(simulator), m_dbusAddress(dbusBootstrapAddress)
        {}

        //! Application mode
        ContextMode getModeApplication() const { return m_application; }

        //! Audio mode
        ContextMode getModeAudio() const { return m_audio; }

        //! Network mode
        ContextMode getModeNetwork() const { return m_network; }

        //! Own aircraft
        ContextMode getModeOwnAircraft() const { return m_ownAircraft; }

        //! Simulator mode
        ContextMode getModeSimulator() const { return m_simulator; }

        //! Local core?
        bool hasLocalCore() const { return m_application == Local || m_application == LocalInDBusServer; }

        //! Requires server (at least one in server)?
        bool requiresDBusSever() const;

        //! Requires DBus connection (at least one remote)?
        bool requiresDBusConnection() const;

        //! DBus address
        QString getDBusAddress() const { return m_dbusAddress; }

        //! DBus address?
        bool hasDBusAddress() const { return !m_dbusAddress.isEmpty(); }

        //! Any context in given mode
        bool any(ContextMode mode) const;

        //! All contexts in given mode
        bool all(ContextMode mode) const;

        //! Any remote context?
        bool anyRemote() const;

        //! Any local in DBus context?
        bool anyLocalInDBusServer() const;

        //! Predefined for Core
        static CCoreFacadeConfig forCoreAllLocalInDBus(const QString &dbusBootstrapAddress = "");

        //! Predefined, completely local (e.g. for unit tests)
        static CCoreFacadeConfig local(const QString &dbusBootstrapAddress = "");

        //! Predefined, completely remote
        static CCoreFacadeConfig remote(const QString &dbusBootstrapAddress = "");

        //! Predefined, all empty configs (normally used when the real config can only be determined later)
        static CCoreFacadeConfig allEmpty();
    };
} // namespace swift::core
#endif // SWIFT_CORE_COREFACADECONFIG_H
