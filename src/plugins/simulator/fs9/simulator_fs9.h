/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIMPLUGIN_SIMULATOR_FS9_H
#define BLACKSIMPLUGIN_SIMULATOR_FS9_H

#include "fs9_host.h"
#include "fs9_client.h"
#include "lobby_client.h"
#include "../fscommon/fsuipc.h"
#include "blackcore/simulator.h"
#include "blackcore/interpolator_linear.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/nwaircraftmodel.h"
#include "blacksim/simulatorinfo.h"
#include <QObject>
#include <QtPlugin>
#include <QList>
#include <QThread>
#include <QHash>

//! \file

namespace BlackSimPlugin
{
    namespace Fs9
    {
        //! Factory implementation to create CSimulatorFs9 instances
        class CSimulatorFs9Factory : public QObject, public BlackCore::ISimulatorFactory
        {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "net.vatsim.PilotClient.BlackCore.SimulatorInterface")
            Q_INTERFACES(BlackCore::ISimulatorFactory)

        public:
            //! \copydoc BlackCore::ISimulatorFactory::create()
            virtual BlackCore::ISimulator *create(QObject *parent) override;

            //! Simulator info
            virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;
        };

        //! FSX Simulator Implementation
        class CSimulatorFs9 : public BlackCore::ISimulator
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorFs9(QObject *parent = nullptr);

            //! Destructor
            virtual ~CSimulatorFs9();

            //! \copydoc ISimulator::isConnected()
            virtual bool isConnected() const override;

            //! \copydoc ISimulator::canConnect()
            virtual bool canConnect() const override { return true; }

            //! \copydoc ISimulator::isPaused
            virtual bool isPaused() const override { return m_simPaused; }

            //! \copydoc ISimulator::isSimulating
            virtual bool isSimulating() const override { return isConnected(); }

            //! Is time synchronization on?
            virtual bool isTimeSynchronized() const override { return m_syncTime; }

        public slots:

            //! \copydoc ISimulator::connectTo()
            virtual bool connectTo() override;

            //! \copydoc ISimulator::connectTo()
            virtual void asyncConnectTo() override;

            //! \copydoc ISimulator::disconnectFrom()
            virtual bool disconnectFrom() override;

            //! \copydoc ISimulator::getOwnAircraft()
            virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const override { return m_ownAircraft; }

            //! \copydoc ISimulator::addRemoteAircraft()
            virtual void addRemoteAircraft(const BlackMisc::Aviation::CAircraft &remoteAircraft, const BlackMisc::Network::CClient &remoteClient) override;

            //! \copydoc ISimulator::addAircraftSituation()
            virtual void addAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &initialSituation) override;

            //! \copydoc ISimulator::removeRemoteAircraft()
            virtual void removeRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc ISimulator::updateOwnSimulatorCockpit()
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &ownAircraft) override;

            //! \copydoc ISimulator::getSimulatorInfo()
            virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;

            //! \copydoc ISimulator::displayStatusMessage()
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;

            //! \copydoc ISimulator::displayTextMessage()
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;

            //! \copydoc ISimulator::getAircraftModel()
            virtual BlackMisc::Network::CAircraftModel getOwnAircraftModel() const override { return m_aircraftModel; }

            //! \copydoc BlackCore::ISimulator::getInstalledModels
            virtual BlackMisc::Network::CAircraftModelList getInstalledModels() const override { return {}; }

            //! \copydoc BlackCore::ISimulator::getCurrentlyMatchedModels
            virtual BlackMisc::Network::CAircraftModelList getCurrentlyMatchedModels() const override { return BlackMisc::Network::CAircraftModelList(); }

            //! Airports in range
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

            //! Set time synchronization between simulator and user's computer time
            //! \remarks not all drivers implement this, e.g. if it is an intrinsic simulator feature
            virtual void setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) override;

            //! Time synchronization offset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override { return m_syncTimeOffset; }

        protected:
            //! Timer event
            virtual void timerEvent(QTimerEvent *event);

        private slots:

            //! Dispatch SimConnect messages
            void ps_dispatch();

            //! Process incoming FS9 message
            void ps_processFs9Message(const QByteArray &message);

            //! Change own aircraft model string
            void ps_changeOwnAircraftModel(const QString &modelname);

            //! Change DirectPlay host status
            void ps_changeHostStatus(BlackSimPlugin::Fs9::CFs9Host::HostStatus status);

        private:

            //! Called when data about our own aircraft are received
            void updateOwnAircraftFromSim(const BlackMisc::Aviation::CAircraft &ownAircraft);

            void disconnectAllClients();

            // DirectPlay object handling
            QPointer<CFs9Host> m_fs9Host;
            bool    m_isHosting = false;        //!< Is sim connected
            bool    m_startedLobbyConnection = false;
            bool    m_syncTime = false;         //!< Time synchronized?
            int     m_syncDeferredCounter = 0;  //!< Set when synchronized, used to wait some time
            bool    m_simPaused = false;        //!< Simulator paused?

            QHash<BlackMisc::Aviation::CCallsign, QPointer<CFs9Client>> m_hashFs9Clients;

            CLobbyClient *m_lobbyClient;

            BlackSim::CSimulatorInfo m_simulatorInfo;
            BlackMisc::Aviation::CAircraft m_ownAircraft; //!< Object representing our own aircraft from simulator
            BlackMisc::Aviation::CAirportList m_airportsInRange;
            BlackMisc::Network::CAircraftModel m_aircraftModel;
            BlackMisc::PhysicalQuantities::CTime m_syncTimeOffset;

            QScopedPointer<FsCommon::CFsuipc> m_fsuipc;
        };
    }

} // namespace BlackCore

#endif // guard
