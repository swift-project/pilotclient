/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_COREFACADECONFIG_H
#define BLACKCORE_COREFACADECONFIG_H

#include "blackcore/blackcoreexport.h"

#include <QString>

namespace BlackCore
{
    //! Helper to correctly run a context
    class BLACKCORE_EXPORT CCoreFacadeConfig
    {
    public:
        //! How to handle a given context
        enum ContextMode
        {
            NotUsed,           //!< during shutdown or not used at all
            Local,             //!< in same process
            LocalInDBusServer, //!< in same process, also registered in DBus, will be accessed by proxy objects too
            Remote             //!< proxy object
        };

    private:
        ContextMode m_application;
        ContextMode m_audio;
        ContextMode m_network;
        ContextMode m_ownAircraft;
        ContextMode m_settings;
        ContextMode m_simulator;
        QString     m_dbusAddress; //!< for boot strapping

    public:
        //! Constructor
        CCoreFacadeConfig(ContextMode allTheSame = NotUsed, const QString &dbusBootstrapAddress = ""):
            m_application(allTheSame), m_audio(allTheSame), m_network(allTheSame), m_ownAircraft(allTheSame), m_settings(allTheSame), m_simulator(allTheSame),
            m_dbusAddress(dbusBootstrapAddress)
        {}

        //! Constructor
        CCoreFacadeConfig(ContextMode application, ContextMode audio, ContextMode network, ContextMode ownAircraft, ContextMode settings, ContextMode simulator, const QString &dbusBootstrapAddress = ""):
            m_application(application), m_audio(audio), m_network(network), m_ownAircraft(ownAircraft) , m_settings(settings), m_simulator(simulator),
            m_dbusAddress(dbusBootstrapAddress)
        {}

        //! Application mode
        ContextMode getModeApplication() const { return m_application; }

        //! Audio mode
        ContextMode getModeAudio() const { return m_audio; }

        //! Network mode
        ContextMode getModeNetwork() const { return m_network; }

        //! Own aircraft
        ContextMode getModeOwnAircraft() const { return m_ownAircraft; }

        //! Settings mode
        ContextMode getModeSettings() const { return m_settings; }

        //! Simulator mode
        ContextMode getModeSimulator() const { return m_simulator; }

        //! Local settings?
        bool hasLocalSettings() const { return m_settings == Local || m_settings == LocalInDBusServer; }

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
}
#endif // guard
