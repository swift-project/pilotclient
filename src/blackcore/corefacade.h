/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_COREFACADE_H
#define BLACKCORE_COREFACADE_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/settings/network.h"
#include "blackmisc/identifier.h"
#include "blackmisc/settingscache.h"

#include <QDBusConnection>
#include <QMap>
#include <QObject>
#include <QString>

namespace BlackMisc
{
    class CDBusServer;
}

namespace BlackCore
{
    // forward declaration, see review
    // https://dev.vatsim-germany.org/boards/22/topics/1350?r=1359#message-1359
    class CContextApplication;
    class CContextAudio;
    class CContextNetwork;
    class CContextOwnAircraft;
    class CContextSimulator;
    class CCoreFacadeConfig;
    class IContextApplication;
    class IContextAudio;
    class IContextNetwork;
    class IContextOwnAircraft;
    class IContextSimulator;

    /*!
     * The runtime class providing facades (the contexts) for all DBus relevant operations.
     * - Initializes all contexts in correct order
     * - Allows a ordered and correct shutdown
     * - Connects all signal/slots between contexts
     *   (such cross context dependencies are not desired but sometimes required)
     * - Loads the application settings
     */
    class BLACKCORE_EXPORT CCoreFacade : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CCoreFacade(const CCoreFacadeConfig &config, QObject *parent = nullptr);

        //! Destructor
        virtual ~CCoreFacade() { this->gracefulShutdown(); }

        //! DBus server (if applicable)
        const BlackMisc::CDBusServer *getDBusServer() const { return this->m_dbusServer; }

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
        void init(const CCoreFacadeConfig &config);

        //! Remote application context, indicates distributed environment
        bool hasRemoteApplicationContext() const;

        //! Register metadata
        static void registerMetadata();

    public slots:
        //! Parse command line in all contexts
        bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator);

    private:
        bool m_init = false; //!< flag
        BlackMisc::CSetting<Settings::Network::DBusServerAddress> m_dbusServerAddress { this };

        // DBus
        BlackMisc::CDBusServer *m_dbusServer = nullptr;
        QDBusConnection m_dbusConnection     = QDBusConnection("default");
        bool m_initDBusConnection            = false;

        // contexts:
        // There is a reason why we do not use smart pointers here. When the context is deleted
        // we need to use deleteLater to gracefully shut the context
        IContextApplication *m_contextApplication = nullptr;
        IContextAudio       *m_contextAudio       = nullptr;
        IContextNetwork     *m_contextNetwork     = nullptr;
        IContextOwnAircraft *m_contextOwnAircraft = nullptr;
        IContextSimulator   *m_contextSimulator   = nullptr;

        //! initialization of DBus connection (where applicable)
        void initDBusConnection(const QString &address);

        //! initialization of DBus connection (where applicable)
        void initDBusServer(const QString &dBusAddress);

        //! post init tasks, load simulator and connecting context signal slots
        void initPostSetup(QMap<QString, int> &times);
    };
} // namespace
#endif // guard
