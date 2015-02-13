/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FS9_H
#define BLACKSIMPLUGIN_SIMULATOR_FS9_H

#include "fs9_host.h"
#include "fs9_client.h"
#include "lobby_client.h"
#include "../fscommon/simulator_fscommon.h"
#include "blackcore/simulator.h"
#include "blackcore/interpolator_linear.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/avaircraft.h"
#include "blacksim/simulatorinfo.h"
#include "blackmisc/pixmap.h"
#include <QObject>
#include <QtPlugin>
#include <QList>
#include <QThread>
#include <QHash>

namespace BlackSimPlugin
{
    namespace Fs9
    {
        //! Factory implementation to create CSimulatorFs9 instances
        class CSimulatorFs9Factory : public QObject, public BlackCore::ISimulatorFactory
        {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "org.swift.pilotclient.BlackCore.SimulatorInterface")
            Q_INTERFACES(BlackCore::ISimulatorFactory)

        public:
            //! \copydoc BlackCore::ISimulatorFactory::create()
            virtual BlackCore::ISimulator *create(
                    BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                    BlackMisc::Simulation::IRenderedAircraftProvider *renderedAircraftProvider,
                    QObject *parent) override;

            //! Simulator info
            virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override;
        };

        //! FSX Simulator Implementation
        class CSimulatorFs9 : public BlackSimPlugin::FsCommon::CSimulatorFsCommon
        {
            Q_OBJECT

        public:
            //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
            CSimulatorFs9(
                BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                BlackMisc::Simulation::IRenderedAircraftProvider *renderedAircraft,
                QObject *parent = nullptr);

            //! Destructor
            virtual ~CSimulatorFs9();

            //! \copydoc ISimulator::isConnected()
            virtual bool isConnected() const override;

            //! \copydoc ISimulator::canConnect()
            virtual bool canConnect() const override { return true; }

            //! \copydoc ISimulator::isSimulating
            virtual bool isSimulating() const override { return isConnected(); }

        public slots:

            //! \copydoc ISimulator::connectTo()
            virtual bool connectTo() override;

            //! \copydoc ISimulator::connectTo()
            virtual void asyncConnectTo() override;

            //! \copydoc ISimulator::disconnectFrom()
            virtual bool disconnectFrom() override;

            //! \copydoc ISimulator::addRemoteAircraft()
            virtual bool addRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;

            //! \copydoc ISimulator::removeRemoteAircraft()
            virtual bool removeRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc ISimulator::updateOwnSimulatorCockpit()
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &ownAircraft, const QString &originator) override;

            //! \copydoc ISimulator::displayStatusMessage()
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;

            //! \copydoc ISimulator::displayTextMessage()
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;

        protected:
            //! Timer event
            virtual void timerEvent(QTimerEvent *event);

        private slots:

            //! Dispatch SimConnect messages
            void ps_dispatch();

            //! Process incoming FS9 message
            void ps_processFs9Message(const QByteArray &message);

            //! Change DirectPlay host status
            void ps_changeHostStatus(BlackSimPlugin::Fs9::CFs9Host::HostStatus status);

        private:

            //! Called when data about our own aircraft are received
            void updateOwnAircraftFromSimulator(const BlackMisc::Aviation::CAircraft &ownAircraft);

            void disconnectAllClients();

            // DirectPlay object handling
            QPointer<CFs9Host> m_fs9Host;
            bool m_isHosting = false;        //!< Is sim connected
            bool m_startedLobbyConnection = false;
            QHash<BlackMisc::Aviation::CCallsign, QPointer<CFs9Client>> m_hashFs9Clients;
            CLobbyClient *m_lobbyClient;
        };
    } // namespace
} // namespace

#endif // guard
