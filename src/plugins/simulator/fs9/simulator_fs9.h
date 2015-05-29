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
#include "blackcore/interpolator.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/aviation/aircraft.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
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
        //! FS9 Simulator Implementation
        class CSimulatorFs9 : public BlackSimPlugin::FsCommon::CSimulatorFsCommon
        {
            Q_OBJECT

        public:
            //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
            CSimulatorFs9(
                const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                BlackMisc::IPluginStorageProvider *pluginStorageProvider,
                QObject *parent = nullptr);

            //! Destructor
            virtual ~CSimulatorFs9() = default;

            //! \copydoc ISimulator::connectTo()
            virtual bool connectTo() override;

            //! \copydoc ISimulator::disconnectFrom()
            virtual bool disconnectFrom() override;

            //! \copydoc ISimulator::physicallyAddRemoteAircraft()
            virtual bool physicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &newRemoteAircraft) override;

            //! \copydoc ISimulator::physicallyRemoveRemoteAircraft()
            virtual bool physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::ISimulator::physicallyRemoveAllRemoteAircraft
            virtual int physicallyRemoveAllRemoteAircraft() override;

            //! \copydoc ISimulator::physicallyRenderedAircraft
            virtual BlackMisc::Aviation::CCallsignSet physicallyRenderedAircraft() const override;

            //! \copydoc ISimulator::isPhysicallyRenderedAircraft
            virtual bool isPhysicallyRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc ISimulator::updateOwnSimulatorCockpit()
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &ownAircraft, const BlackMisc::COriginator &originator) override;

            //! \copydoc ISimulator::displayStatusMessage()
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;

            //! \copydoc ISimulator::displayTextMessage()
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;

        protected:
            //! \copydoc ISimulator::isConnected()
            virtual bool isConnected() const override;

            //! \copydoc ISimulator::isSimulating
            virtual bool isSimulating() const override { return isConnected(); }

            //! Timer event
            virtual void timerEvent(QTimerEvent *event);

        private slots:

            //! Dispatch SimConnect messages
            void ps_dispatch();

            //! Process incoming FS9 message
            void ps_processFs9Message(const QByteArray &message);

        private:

            //! Called when data about our own aircraft are received
            void updateOwnAircraftFromSimulator(const BlackMisc::Aviation::CAircraft &ownAircraft);

            void disconnectAllClients();

            //! Aircraft object directory
            //! \todo Read from settings if registry fails
            static QString aircraftObjectsDir();

            //! Do not include the following directories for FS
            static const QStringList &excludeDirectories();

            QHash<BlackMisc::Aviation::CCallsign, QPointer<CFs9Client>> m_hashFs9Clients;
            QMetaObject::Connection m_connectionHostMessages;
            int m_dispatchTimerId = -1;

            bool m_simConnected = false; //!< Is simulator connected?
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
            CSimulatorFs9Listener(QObject *parent);

        public slots:
            //! \copydoc BlackCore::ISimulatorListener::start
            virtual void start() override;

            //! \copydoc BlackCore::ISimulatorListener::stop
            virtual void stop() override;

        private:

            QTimer *m_timer = nullptr;
            bool m_isConnecting = false;
            bool m_isStarted = false;

        };

        //! Factory implementation to create CSimulatorFs9 instances
        class CSimulatorFs9Factory : public QObject, public BlackCore::ISimulatorFactory
        {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "org.swift-project.blackcore.simulatorinterface" FILE "simulator_fs9.json")
            Q_INTERFACES(BlackCore::ISimulatorFactory)

        public:
            //! Constructor
            CSimulatorFs9Factory(QObject *parent = nullptr);

            //! Destructor
            virtual ~CSimulatorFs9Factory();

            //! \copydoc BlackCore::ISimulatorFactory::create(ownAircraftProvider, remoteAircraftProvider, parent)
            virtual BlackCore::ISimulator *create(
                const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                BlackMisc::IPluginStorageProvider *pluginStorageProvider) override;

            //! \copydoc BlackCore::ISimulatorFactory::createListener
            virtual BlackCore::ISimulatorListener *createListener(QObject *parent = nullptr) override;

        };
    } // namespace Fs9
} // namespace BlackCore

#endif // guard
