// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FS9_H
#define BLACKSIMPLUGIN_SIMULATOR_FS9_H

#include "fs9host.h"
#include "fs9client.h"
#include "lobbyclient.h"
#include "../fscommon/simulatorfscommon.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/interpolation/interpolator.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/pixmap.h"
#include "plugins/simulator/fscommon/fsuipc.h"
#include <QObject>
#include <QtPlugin>
#include <QList>
#include <QThread>
#include <QHash>

namespace BlackSimPlugin::Fs9
{
    //! FS9 Simulator Implementation
    class CSimulatorFs9 : public FsCommon::CSimulatorFsCommon
    {
        Q_OBJECT

    public:
        //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
        CSimulatorFs9(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                      const QSharedPointer<CFs9Host> &fs9Host,
                      const QSharedPointer<CLobbyClient> &lobbyClient,
                      BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                      BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                      BlackMisc::Network::IClientProvider *clientProvider,
                      QObject *parent = nullptr);

        //! Destructor
        virtual ~CSimulatorFs9() override = default;

        //! \name Interface implementations
        //! @{
        virtual bool connectTo() override;
        virtual bool disconnectFrom() override;
        virtual bool physicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &newRemoteAircraft) override;
        virtual bool physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual int physicallyRemoveAllRemoteAircraft() override;
        virtual BlackMisc::Aviation::CCallsignSet physicallyRenderedAircraft() const override;
        virtual bool isPhysicallyRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual bool updateOwnSimulatorCockpit(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft, const BlackMisc::CIdentifier &originator) override;
        virtual bool updateOwnSimulatorSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;
        virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;
        virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;
        virtual BlackMisc::CStatusMessageList getInterpolationMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual bool testSendSituationAndParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CAircraftParts &parts) override;
        //! @}

    protected:
        //! \name Interface implementations
        //! @{
        virtual bool isConnected() const override;
        virtual bool isSimulating() const override { return isConnected(); }
        //! @}

        //! Timer event dispatching
        //! \sa m_timerId
        //! \sa CSimulatorFsxCommon::dispatch
        virtual void timerEvent(QTimerEvent *event) override;

        // remark: in FS9 there is no central updateRemoteAircraft() function, each FS9 client updates itself
        // updateRemoteAircraft()

    private:
        //! Dispatch FSUIPC reading
        //! \remark very frequently called
        void dispatch();

        //! Process incoming FS9 message
        void processFs9Message(const QByteArray &message);

        //! Called when data about our own aircraft are received
        void updateOwnAircraftFromSimulator(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

        //! Render status
        void updateRenderStatus(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, BlackSimPlugin::Fs9::CFs9Client::ClientStatus);

        //! Disconnect all clients
        void disconnectAllClients();

        //! Sync time with user's computer
        void synchronizeTime();

        BlackSimPlugin::FsCommon::CFsuipc *m_fsuipc = nullptr; //!< FSUIPC
        QHash<BlackMisc::Aviation::CCallsign, QPointer<CFs9Client>> m_hashFs9Clients;
        QMetaObject::Connection m_connectionHostMessages;
        bool m_simConnected = false; //!< Is simulator connected?
        QSharedPointer<CFs9Host> m_fs9Host;
        QSharedPointer<CLobbyClient> m_lobbyClient;

        static constexpr int SkipUpdateCyclesForCockpit = 10;
    };

    //! Listener for FS9
    //! Listener starts the FS9 multiplayer host and tries to make the running instance
    //! of simulator to connect to it. When emitting the simulatorStarted() signal,
    //! FS9 is already connected.
    class CSimulatorFs9Listener : public BlackCore::ISimulatorListener
    {
        Q_OBJECT

    public:
        //! Constructor
        CSimulatorFs9Listener(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                              const QSharedPointer<CFs9Host> &fs9Host,
                              const QSharedPointer<CLobbyClient> &lobbyClient);

    protected:
        //! \copydoc BlackCore::ISimulatorListener::startImpl
        virtual void startImpl() override;

        //! \copydoc BlackCore::ISimulatorListener::stopImpl
        virtual void stopImpl() override;

        //! \copydoc BlackCore::ISimulatorListener::checkImpl
        virtual void checkImpl() override;

    private:
        //! Check connection to FS9
        bool checkConnection(bool canLobbyConnect);

        QTimer *m_timer = nullptr;
        bool m_isConnecting = false;
        bool m_isStarted = false;
        QSharedPointer<CFs9Host> m_fs9Host;
        QSharedPointer<CLobbyClient> m_lobbyClient;
        FsCommon::CFsuipc *m_fsuipc = nullptr;
    };

    //! Factory implementation to create CSimulatorFs9 instances
    class CSimulatorFs9Factory : public QObject, public BlackCore::ISimulatorFactory
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blackcore.simulatorinterface" FILE "simulatorfs9.json")
        Q_INTERFACES(BlackCore::ISimulatorFactory)

    public:
        //! Constructor
        CSimulatorFs9Factory(QObject *parent = nullptr);

        //! Destructor
        virtual ~CSimulatorFs9Factory() override;

        //! \copydoc BlackCore::ISimulatorFactory::create
        virtual BlackCore::ISimulator *create(
            const BlackMisc::Simulation::CSimulatorPluginInfo &info,
            BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
            BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
            BlackMisc::Network::IClientProvider *clientProvider) override;

        //! \copydoc BlackCore::ISimulatorFactory::createListener
        virtual BlackCore::ISimulatorListener *createListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info) override;

    private:
        QSharedPointer<CFs9Host> m_fs9Host;
        QSharedPointer<CLobbyClient> m_lobbyClient;
    };
} // ns

#endif // guard
