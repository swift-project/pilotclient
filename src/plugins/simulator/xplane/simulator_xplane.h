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

#include "blackcore/simulator.h"
#include "blackmisc/pixmap.h"
#include <QDBusConnection>

class QDBusServiceWatcher;

namespace BlackSimPlugin
{
    namespace XPlane
    {

        class CXBusServiceProxy;
        class CXBusTrafficProxy;

        /*!
         * X-Plane ISimulator implementation
         */
        class CSimulatorXPlane : public BlackCore::ISimulator
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorXPlane(QObject *parent = nullptr);

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

        public slots:
            //! \copydoc BlackCore::ISimulator::connectTo
            virtual bool connectTo() override;

            //! \copydoc BlackCore::ISimulator::asyncConnectTo
            virtual void asyncConnectTo() override;

            //! \copydoc BlackCore::ISimulator::disconnectFrom
            virtual bool disconnectFrom() override;

            //! \copydoc BlackCore::ISimulator::getOwnAircraft
            virtual BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const override;

            //! \copydoc ISimulator::addRemoteAircraft()
            virtual void addRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;

            //! \copydoc BlackCore::ISimulator::getRemoteAircraft
            virtual BlackMisc::Simulation::CSimulatedAircraftList getRemoteAircraft() const override { return m_remoteAircraft; }

            //! \copydoc BlackCore::ISimulator::addAircraftSituation
            virtual void addAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign,
                                              const BlackMisc::Aviation::CAircraftSituation &situation) override;

            //! \copydoc BlackCore::ISimulator::removeRemoteAircraft
            virtual int removeRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc ISimulator::changeRemoteAircraft
            virtual int changeRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &changedAircraft, const BlackMisc::CPropertyIndexVariantMap &changeValues) override;

            //! \copydoc BlackCore::ISimulator::updateOwnSimulatorCockpit
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft) override;

            //! \copydoc BlackCore::ISimulator::getSimulatorInfo
            virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override { return BlackSim::CSimulatorInfo::XP(); }

            //! \copydoc BlackCore::ISimulator::displayStatusMessage
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;

            //! \copydoc ISimulator::displayTextMessage
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;

            //! \copydoc BlackCore::ISimulator::getAircraftModel
            virtual BlackMisc::Simulation::CAircraftModel getOwnAircraftModel() const override;

            //! \copydoc BlackCore::ISimulator::getInstalledModels
            virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const override;

            //! Airports in range
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

            //! \copydoc ISimulator::setTimeSynchronization
            virtual void setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) override;

            //! \copydoc ISimulator::getTimeSynchronizationOffset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override { return BlackMisc::PhysicalQuantities::CTime(0, BlackMisc::PhysicalQuantities::CTimeUnit::hrmin()); }

            //! \copydoc ISimulator::iconForModel
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

        private slots:
            void ps_serviceRegistered(const QString &serviceName);
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

            BlackMisc::Aviation::CAirportList m_airports;
            BlackMisc::Simulation::CSimulatedAircraftList m_remoteAircraft;

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
                int com1Active;
                int com1Standby;
                int com2Active;
                int com2Standby;
                int xpdrCode;
                int xpdrMode;
                bool xpdrIdent;
            } m_xplaneData;

            void resetData()
            {
                m_xplaneData = { "", "", 0, 0, 0, 0, 0, 0, 0, 122800, 122800, 122800, 122800, 2000, 0, false };
            }
        };

        //! Factory for creating CSimulatorXPlane instance
        class CSimulatorXPlaneFactory : public QObject, public BlackCore::ISimulatorFactory
        {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "net.vatsim.PilotClient.BlackCore.SimulatorInterface")
            Q_INTERFACES(BlackCore::ISimulatorFactory)

        public:
            //! \copydoc BlackCore::ISimulatorFactory::create
            virtual BlackCore::ISimulator *create(QObject *parent = nullptr) override { return new CSimulatorXPlane(parent); }

            //! \copydoc BlackCore::ISimulatorFactory::getSimulatorInfo
            virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override { return BlackSim::CSimulatorInfo::XP(); }
        };

    }
}

#endif // guard
