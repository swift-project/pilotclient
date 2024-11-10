// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_COREFACADE_H
#define BLACKCORE_COREFACADE_H

#include "blackcore/corefacadeconfig.h"
#include "blackcore/data/launchersetup.h"
#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/blackcoreexport.h"
#include "misc/identifier.h"
#include "misc/settingscache.h"

#include <QDBusConnection>
#include <QMap>
#include <QObject>
#include <QString>

namespace swift::misc
{
    class CDBusServer;
    class CLogHistory;
    class CLogHistorySource;

    namespace shared_state
    {
        class CDataLinkDBus;
    }
}
namespace BlackCore
{
    namespace Context
    {
        class CContextApplication;
        class CContextAudio;
        class CContextAudioBase;
        class CContextNetwork;
        class CContextOwnAircraft;
        class CContextSimulator;
        class IContextApplication;
        class IContextAudio;
        class IContextNetwork;
        class IContextOwnAircraft;
        class IContextSimulator;
    }

    /*!
     * The class providing facades (the contexts) for all DBus relevant operations.
     * - It initializes all contexts in correct order
     * - Allows an ordered and correct shutdown
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
        virtual ~CCoreFacade() override { this->gracefulShutdown(); }

        //! Transport mechanism for sharing state between applications
        swift::misc::shared_state::CDataLinkDBus *getDataLinkDBus() { return this->m_dataLinkDBus; }

        //! In case connection between DBus parties is lost, try to reconnect
        swift::misc::CStatusMessage tryToReconnectWithDBus();

        //! Clean up (will be connected to signal QCoreApplication::aboutToQuit)
        void gracefulShutdown();

        //! Facade and context shutting down
        bool isShuttingDown() const { return m_shuttingDown; }

        //! Parse command line in all contexts
        bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator);

        // ------- Context as interface, normal way to access a context

        //! Context for network
        Context::IContextNetwork *getIContextNetwork();

        //! Context for network
        const Context::IContextNetwork *getIContextNetwork() const;

        //! Context for audio
        Context::IContextAudio *getIContextAudio();

        //! Context for audio
        const Context::IContextAudio *getIContextAudio() const;

        //! Context for audio
        Context::CContextAudioBase *getCContextAudioBase();

        //! Context for audio
        const Context::CContextAudioBase *getCContextAudioBase() const;

        //! Context for application
        Context::IContextApplication *getIContextApplication();

        //! Context for application
        const Context::IContextApplication *getIContextApplication() const;

        //! Context for own aircraft
        Context::IContextOwnAircraft *getIContextOwnAircraft();

        //! Context for own aircraft
        const Context::IContextOwnAircraft *getIContextOwnAircraft() const;

        //! Context for simulator
        Context::IContextSimulator *getIContextSimulator();

        //! Context for simulator
        const Context::IContextSimulator *getIContextSimulator() const;

        // ------- Context as implementing (local) class

        //! Context for application
        //! \remarks only applicable for local object
        Context::CContextApplication *getCContextApplication();

        //! Context for application
        //! \remarks only applicable for local object
        const Context::CContextApplication *getCContextApplication() const;

        //! Context for audio
        //! \remarks only applicable for local object
        Context::CContextAudio *getCContextAudio();

        //! Context for audio
        //! \remarks only applicable for local object
        const Context::CContextAudio *getCContextAudio() const;

        //! Context for own aircraft
        //! \remarks only applicable for local object
        Context::CContextOwnAircraft *getCContextOwnAircraft();

        //! Context for own aircraft
        //! \remarks only applicable for local object
        const Context::CContextOwnAircraft *getCContextOwnAircraft() const;

        //! Context for network
        //! \remarks only applicable for local object
        Context::CContextNetwork *getCContextNetwork();

        //! Context for network
        //! \remarks only applicable for local object
        const Context::CContextNetwork *getCContextNetwork() const;

        //! Context for simulator
        //! \remarks only applicable for local object
        Context::CContextSimulator *getCContextSimulator();

        //! Context for simulator
        //! \remarks only applicable for local object
        const Context::CContextSimulator *getCContextSimulator() const;

        //! DBus address if any
        QString getDBusAddress() const;

        //! Register metadata
        static void registerMetadata();

    private:
        bool m_initalized = false; //!< flag if already initialized
        bool m_shuttingDown = false; //!< flag if shutting down
        const CCoreFacadeConfig m_config; //!< used config
        swift::misc::CData<Data::TLauncherSetup> m_launcherSetup { this }; //!< updating DBus

        // DBus
        swift::misc::CDBusServer *m_dbusServer = nullptr;
        bool m_initDBusConnection = false;
        QDBusConnection m_dbusConnection { "default" };

        // shared state infrastructure
        swift::misc::shared_state::CDataLinkDBus *m_dataLinkDBus = nullptr;
        swift::misc::CLogHistory *m_logHistory = nullptr;
        swift::misc::CLogHistorySource *m_logHistorySource = nullptr;

        // contexts:
        // There is a reason why we do not use smart pointers here. When the context is deleted
        // we need to use deleteLater to gracefully shut the context
        Context::IContextApplication *m_contextApplication = nullptr;
        Context::CContextAudioBase *m_contextAudio = nullptr;
        Context::IContextNetwork *m_contextNetwork = nullptr;
        Context::IContextOwnAircraft *m_contextOwnAircraft = nullptr;
        Context::IContextSimulator *m_contextSimulator = nullptr;

        //! Init
        void init();

        //! initialization of DBus connection (where applicable)
        void initDBusConnection(const QString &address);

        //! initialization of DBus connection (where applicable)
        void initDBusServer(const QString &dBusAddress);

        //! post init tasks, load simulator and connecting context signal slots
        void initPostSetup(QMap<QString, qint64> &times);
    };
} // namespace
#endif // guard
