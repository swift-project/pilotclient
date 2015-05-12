/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_XPLANE_H
#define BLACKSIMPLUGIN_SIMULATOR_XPLANE_H

#include "blackcore/simulator_common.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/pixmap.h"
#include <QDBusConnection>

class QDBusServiceWatcher;

namespace BlackSimPlugin
{
    namespace XPlane
    {
        class CXBusServiceProxy;
        class CXBusTrafficProxy;

        //! X-Plane ISimulator implementation
        class CSimulatorXPlane : public BlackCore::CSimulatorCommon
        {
            Q_OBJECT

        public:
            //! \copydoc BlackCore::ISimulatorFactory::create(ownAircraftProvider, remoteAircraftProvider, parent)
            CSimulatorXPlane(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                             BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                             BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                             QObject *parent = nullptr);

            //! \copydoc BlackCore::ISimulator::isConnected
            virtual bool isConnected() const override;

            //! \copydoc BlackCore::ISimulator::canConnect
            virtual bool canConnect() const override;

            //! \copydoc ISimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override { return false; } // TODO: Can we query the XP intrinisc feature?

            //! \copydoc ISimulator::isSimPaused
            virtual bool isPaused() const override { return false; }

            //! \copydoc ISimulator::isSimulating
            virtual bool isSimulating() const override { return isConnected(); }

            //! \copydoc BlackCore::ISimulator::getIcaoForModelString
            virtual BlackMisc::Aviation::CAircraftIcaoData getIcaoForModelString(const QString &modelString) const override;

            //! \copydoc BlackCore::ISimulator::connectTo
            virtual bool connectTo() override;

            //! \copydoc BlackCore::ISimulator::asyncConnectTo
            virtual void asyncConnectTo() override;

            //! \copydoc BlackCore::ISimulator::disconnectFrom
            virtual bool disconnectFrom() override;

            //! \copydoc ISimulator::physicallyAddRemoteAircraft()
            virtual bool physicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &newRemoteAircraft) override;

            //! \copydoc BlackCore::ISimulator::physicallyRemoveRemoteAircraft
            virtual bool physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::ISimulator::physicallyRemoveAllRemoteAircraft
            virtual void physicallyRemoveAllRemoteAircraft() override;

            //! \copydoc ISimulator::physicallyRenderedAircraft
            virtual BlackMisc::Aviation::CCallsignSet physicallyRenderedAircraft() const override;

            //! \copydoc ISimulator::isPhysicallyRenderedAircraft
            virtual bool isPhysicallyRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc ISimulator::changeRenderedAircraftModel
            virtual bool changeRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator) override;

            //! \copydoc ISimulator::changeAircraftEnabled
            virtual bool changeRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator) override;

            //! \copydoc BlackCore::ISimulator::updateOwnSimulatorCockpit
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator) override;

            //! \copydoc BlackCore::ISimulator::displayStatusMessage
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;

            //! \copydoc ISimulator::displayTextMessage
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;

            //! \copydoc BlackCore::ISimulator::getInstalledModels
            virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const override;

            //! \copydoc ISimulator::reloadInstalledModels
            virtual void reloadInstalledModels() override;

            //! \copydoc BlackCore::ISimulator::getAirportsInRange
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const;

            //! \copydoc ISimulator::setTimeSynchronization
            virtual bool setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) override;

            //! \copydoc ISimulator::getTimeSynchronizationOffset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override { return BlackMisc::PhysicalQuantities::CTime(0, BlackMisc::PhysicalQuantities::CTimeUnit::hrmin()); }

            //! \copydoc ISimulator::iconForModel
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

        protected slots:
            //! \copydoc CSimulatorCommon::ps_remoteProviderAddAircraftSituation
            virtual void ps_remoteProviderAddAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;

            //! \copydoc CSimulatorCommon::ps_remoteProvideraddAircraftParts
            virtual void ps_remoteProviderAddAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts) override;

            //! \copydoc CSimulatorCommon::ps_remoteProviderRemovedAircraft
            virtual void ps_remoteProviderRemovedAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

        private slots:
            void ps_serviceUnregistered();
            void ps_setAirportsInRange(const QStringList &icaoCodes, const QStringList &names, const BlackMisc::CSequence<double> &lats, const BlackMisc::CSequence<double> &lons, const BlackMisc::CSequence<double> &alts);
            void ps_emitOwnAircraftModelChanged(const QString &path, const QString &filename, const QString &livery, const QString &icao);
            void ps_fastTimerTimeout();
            void ps_slowTimerTimeout();

        private:
            QDBusConnection m_conn { "default" };
            QDBusServiceWatcher *m_watcher { nullptr };
            CXBusServiceProxy *m_service { nullptr };
            CXBusTrafficProxy *m_traffic { nullptr };
            QTimer *m_fastTimer { nullptr };
            QTimer *m_slowTimer { nullptr };
            BlackMisc::Aviation::CAirportList m_airportsInRange;   //!< aiports in range of own aircraft

            //! \todo Add units to members? pitchDeg?, altitudeFt?
            struct // data is written by DBus async method callbacks
            {
                QString aircraftModelPath;
                QString aircraftIcaoCode;
                double latitude;
                double longitude;
                double altitude;
                double groundspeed;
                double pitch;
                double roll;
                double trueHeading;
                bool onGroundAll;
                int com1Active;
                int com1Standby;
                int com2Active;
                int com2Standby;
                int xpdrCode;
                int xpdrMode;
                bool xpdrIdent;
                bool beaconLightsOn;
                bool landingLightsOn;
                bool navLightsOn;
                bool strobeLightsOn;
                bool taxiLightsOn;
                double flapsReployRatio;
                double gearReployRatio;
                QList<double> enginesN1Percentage;
                double speedBrakeRatio;
            } m_xplaneData;

            void resetData()
            {
                m_xplaneData = { "", "", 0, 0, 0, 0, 0, 0, 0, false, 122800, 122800, 122800, 122800, 2000, 0, false, false, false, false,
                                 false, false, 0, 0, {}, false
                               };

            }
        };

        //! Listener waits for xbus service to show up
        class CSimulatorXPlaneListener : public BlackCore::ISimulatorListener
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorXPlaneListener(QObject *parent);

        public slots:
            //! \copydoc BlackCore::ISimulatorListener::start
            virtual void start() override;

            //! \copydoc BlackCore::ISimulatorListener::stop
            virtual void stop() override;

        private slots:
            void ps_serviceRegistered(const QString &serviceName);

        private:
            QDBusConnection m_conn { "default" };
            QDBusServiceWatcher *m_watcher { nullptr };

        };

        //! Factory for creating CSimulatorXPlane instance
        class CSimulatorXPlaneFactory : public QObject, public BlackCore::ISimulatorFactory
        {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "org.swift-project.blackcore.simulatorinterface" FILE "simulator_xplane.json")
            Q_INTERFACES(BlackCore::ISimulatorFactory)

        public:
            //! \copydoc BlackCore::ISimulatorFactory::create()
            virtual BlackCore::ISimulator *create(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                                                  BlackMisc::Simulation::IOwnAircraftProvider    *ownAircraftProvider,
                                                  BlackMisc::Simulation::IRemoteAircraftProvider *renderedAircraftProvider,
                                                  QObject *parent) override;

            //! \copydoc BlackCore::ISimulatorFactory::createListener
            virtual BlackCore::ISimulatorListener *createListener(QObject *parent = nullptr) override { return new CSimulatorXPlaneListener(parent); }
        };

    } // ns
} // ns

#endif // guard
