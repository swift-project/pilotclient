// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FS9_H
#define BLACKSIMPLUGIN_SIMULATOR_FS9_H

#include "fs9host.h"
#include "fs9client.h"
#include "lobbyclient.h"
#include "../fscommon/simulatorfscommon.h"
#include "core/simulator.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/interpolation/interpolator.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/pixmap.h"
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
        //! Constructor, parameters as in \sa swift::core::ISimulatorFactory::create
        CSimulatorFs9(const swift::misc::simulation::CSimulatorPluginInfo &info,
                      const QSharedPointer<CFs9Host> &fs9Host,
                      const QSharedPointer<CLobbyClient> &lobbyClient,
                      swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                      swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                      swift::misc::network::IClientProvider *clientProvider,
                      QObject *parent = nullptr);

        //! Destructor
        virtual ~CSimulatorFs9() override = default;

        //! \name Interface implementations
        //! @{
        virtual bool connectTo() override;
        virtual bool disconnectFrom() override;
        virtual bool physicallyAddRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &newRemoteAircraft) override;
        virtual bool physicallyRemoveRemoteAircraft(const swift::misc::aviation::CCallsign &callsign) override;
        virtual int physicallyRemoveAllRemoteAircraft() override;
        virtual swift::misc::aviation::CCallsignSet physicallyRenderedAircraft() const override;
        virtual bool isPhysicallyRenderedAircraft(const swift::misc::aviation::CCallsign &callsign) const override;
        virtual bool updateOwnSimulatorCockpit(const swift::misc::simulation::CSimulatedAircraft &ownAircraft, const swift::misc::CIdentifier &originator) override;
        virtual bool updateOwnSimulatorSelcal(const swift::misc::aviation::CSelcal &selcal, const swift::misc::CIdentifier &originator) override;
        virtual void displayStatusMessage(const swift::misc::CStatusMessage &message) const override;
        virtual void displayTextMessage(const swift::misc::network::CTextMessage &message) const override;
        virtual swift::misc::CStatusMessageList getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const override;
        virtual bool testSendSituationAndParts(const swift::misc::aviation::CCallsign &callsign, const swift::misc::aviation::CAircraftSituation &situation, const swift::misc::aviation::CAircraftParts &parts) override;
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
        void updateOwnAircraftFromSimulator(const swift::misc::simulation::CSimulatedAircraft &ownAircraft);

        //! Render status
        void updateRenderStatus(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, BlackSimPlugin::Fs9::CFs9Client::ClientStatus);

        //! Disconnect all clients
        void disconnectAllClients();

        //! Sync time with user's computer
        void synchronizeTime();

        BlackSimPlugin::FsCommon::CFsuipc *m_fsuipc = nullptr; //!< FSUIPC
        QHash<swift::misc::aviation::CCallsign, QPointer<CFs9Client>> m_hashFs9Clients;
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
    class CSimulatorFs9Listener : public swift::core::ISimulatorListener
    {
        Q_OBJECT

    public:
        //! Constructor
        CSimulatorFs9Listener(const swift::misc::simulation::CSimulatorPluginInfo &info,
                              const QSharedPointer<CFs9Host> &fs9Host,
                              const QSharedPointer<CLobbyClient> &lobbyClient);

    protected:
        //! \copydoc swift::core::ISimulatorListener::startImpl
        virtual void startImpl() override;

        //! \copydoc swift::core::ISimulatorListener::stopImpl
        virtual void stopImpl() override;

        //! \copydoc swift::core::ISimulatorListener::checkImpl
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
    class CSimulatorFs9Factory : public QObject, public swift::core::ISimulatorFactory
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blaswift_coreckcore.simulatorinterface" FILE "simulatorfs9.json")
        Q_INTERFACES(swift::core::ISimulatorFactory)

    public:
        //! Constructor
        CSimulatorFs9Factory(QObject *parent = nullptr);

        //! Destructor
        virtual ~CSimulatorFs9Factory() override;

        //! \copydoc swift::core::ISimulatorFactory::create
        virtual swift::core::ISimulator *create(
            const swift::misc::simulation::CSimulatorPluginInfo &info,
            swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
            swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
            swift::misc::network::IClientProvider *clientProvider) override;

        //! \copydoc swift::core::ISimulatorFactory::createListener
        virtual swift::core::ISimulatorListener *createListener(const swift::misc::simulation::CSimulatorPluginInfo &info) override;

    private:
        QSharedPointer<CFs9Host> m_fs9Host;
        QSharedPointer<CLobbyClient> m_lobbyClient;
    };
} // ns

#endif // guard
