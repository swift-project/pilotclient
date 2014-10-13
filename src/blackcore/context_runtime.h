/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_RUNTIME_H
#define BLACKCORE_CONTEXT_RUNTIME_H

#include "blackcore/context_runtime_config.h"
#include "blackmisc/statusmessagelist.h"
#include <QDBusConnection>
#include <QObject>
#include <QMultiMap>
#include <QReadWriteLock>

using namespace BlackMisc;

namespace BlackCore
{
    // forward declaration, see review
    // https://dev.vatsim-germany.org/boards/22/topics/1350?r=1359#message-1359
    class CDBusServer;
    class CContextApplication;
    class CContextAudio;
    class CContextNetwork;
    class CContextOwnAircraft;
    class CContextSettings;
    class CContextSimulator;
    class IContextApplication;
    class IContextAudio;
    class IContextNetwork;
    class IContextOwnAircraft;
    class IContextSettings;
    class IContextSimulator;

    //! The Context runtime class
    class CRuntime : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CRuntime(const CRuntimeConfig &config, QObject *parent = nullptr);

        //! Destructor
        virtual ~CRuntime() { this->gracefulShutdown(); }

        //! DBus server (if applicable)
        const CDBusServer *getDBusServer() const { return this->m_dbusServer; }

        //! DBus connection (if applicable)
        const QDBusConnection &getDBusConnection() const { return this->m_dbusConnection; }

        //! Clean up (will be connected to signal QCoreApplication::aboutToQuit)
        void gracefulShutdown();

        // ------- Context as interface, normal way to access a context

        //! Context for network
        IContextNetwork *getIContextNetwork();

        //! Context for network
        const IContextNetwork *getIContextNetwork() const;

        //! Context for audio
        IContextAudio *getIContextAudio();

        //! Context for audio
        const IContextAudio *getIContextAudio() const;

        //! Context for application
        IContextApplication *getIContextApplication();

        //! Context for application
        const IContextApplication *getIContextApplication() const;

        //! Context for own aircraft
        IContextOwnAircraft *getIContextOwnAircraft();

        //! Context for own aircraft
        const IContextOwnAircraft *getIContextOwnAircraft() const;

        //! Context for settings
        IContextSettings *getIContextSettings();

        //! Context for settings
        const IContextSettings *getIContextSettings() const;

        //! Context for simulator
        IContextSimulator *getIContextSimulator();

        //! Context for simulator
        const IContextSimulator *getIContextSimulator() const;

        // ------- Context as implementing (local) class

        //! Context for application
        //! \remarks only applicable for local object
        CContextApplication *getCContextApplication();

        //! Context for application
        //! \remarks only applicable for local object
        const CContextApplication *getCContextApplication() const;

        //! Context for audio
        //! \remarks only applicable for local object
        CContextAudio *getCContextAudio();

        //! Context for audio
        //! \remarks only applicable for local object
        const CContextAudio *getCContextAudio() const;

        //! Context for own aircraft
        //! \remarks only applicable for local object
        CContextOwnAircraft *getCContextOwnAircraft();

        //! Context for own aircraft
        //! \remarks only applicable for local object
        const CContextOwnAircraft *getCContextOwnAircraft() const;

        //! Context for network
        //! \remarks only applicable for local object
        CContextNetwork *getCContextNetwork();

        //! Context for network
        //! \remarks only applicable for local object
        const CContextNetwork *getCContextNetwork() const;

        //! Context for simulator
        //! \remarks only applicable for local object
        CContextSimulator *getCContextSimulator();

        //! Context for simulator
        //! \remarks only applicable for local object
        const CContextSimulator *getCContextSimulator() const;

        //! Init
        void init(const CRuntimeConfig &config);

        //! Remote application context, indicates distributed environment
        bool hasRemoteApplicationContext() const;

        //! Is application context available?
        bool canPingApplicationContext() const;

    private:
        bool m_init = false; /*!< flag */

        // DBus
        CDBusServer *m_dbusServer = nullptr;
        QDBusConnection m_dbusConnection = QDBusConnection("default");
        bool m_initDBusConnection   = false;

        // logging on signals / slots
        bool m_signalLogApplication = false;
        bool m_signalLogAudio       = false;
        bool m_signalLogNetwork     = false;
        bool m_signalLogOwnAircraft = false;
        bool m_signalLogSettings    = false;
        bool m_signalLogSimulator   = false;
        bool m_slotLogApplication   = false;
        bool m_slotLogAudio         = false;
        bool m_slotLogNetwork       = false;
        bool m_slotLogOwnAircraft   = false;
        bool m_slotLogSettings      = false;
        bool m_slotLogSimulator     = false;
        QMultiMap<QString, QMetaObject::Connection> m_logSignalConnections;

        // thread safety for logging
        mutable QReadWriteLock m_lock;

        // contexts:
        // There is a reason why we do not use smart pointers here. When the context is deleted
        // we need to use deleteLater to gracefully shut the context
        IContextApplication *m_contextApplication = nullptr;
        IContextAudio       *m_contextAudio       = nullptr;
        IContextNetwork     *m_contextNetwork     = nullptr;
        IContextOwnAircraft *m_contextOwnAircraft = nullptr;
        IContextSettings    *m_contextSettings    = nullptr;
        IContextSimulator   *m_contextSimulator   = nullptr;

        //! initialization of DBus connection (where applicable)
        void initDBusConnection(const QString &address);

        //! initialization of DBus connection (where applicable)
        void initDBusServer(const QString &dBusAddress);

        //! post init tasks, mainly connecting context signal slots
        void initPostSetup();
    };
} // namespace
#endif // guard
