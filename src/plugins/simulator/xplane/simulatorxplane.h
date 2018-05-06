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

#include "xplanempaircraft.h"
#include "blackcore/simulator.h"
#include "blackcore/simulatorcommon.h"
#include "plugins/simulator/xplaneconfig/simulatorxplaneconfig.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/settings/xswiftbussettings.h"
#include "blackmisc/weather/weathergrid.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/identifier.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/sequence.h"

#include <QDBusConnection>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

class QDBusServiceWatcher;

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftParts;
        class CAircraftSituation;
        class CCallsign;
    }
    namespace Network { class CTextMessage; }
    namespace Simulation
    {
        class CSimulatedAircraft;
        class CSimulatorPluginInfo;
        class IOwnAircraftProvider;
        class IRemoteAircraftProvider;
    }
    namespace Weather { class IWeatherGridProvider; }
}

namespace BlackSimPlugin
{
    namespace XPlane
    {
        class CXSwiftBusServiceProxy;
        class CXSwiftBusTrafficProxy;
        class CXSwiftBusWeatherProxy;

        //! X-Plane data
        //! \todo Add units to members? pitchDeg?, altitudeFt?
        struct XPlaneData
        {
            QString aircraftModelPath;          //!< Aircraft model path
            QString aircraftIcaoCode;           //!< Aircraft model path
            double latitude;                    //!< Longitude [deg]
            double longitude;                   //!< Latitude [deg]
            double altitude;                    //!< Altitude [m]
            double groundspeed;                 //!< Ground speed [m/s]
            double pitch;                       //!< Pitch [deg]
            double roll;                        //!< Roll [deg]
            double trueHeading;                 //!< True heading [deg]
            bool onGroundAll;                   //!< All wheels on ground?
            int com1Active;                     //!< COM1 active [kHz]
            int com1Standby;                    //!< COM1 standby [kHz]
            int com2Active;                     //!< COM2 active [kHz]
            int com2Standby;                    //!< COM2 standby [kHz]
            int xpdrCode;                       //!< Transpondder code
            int xpdrMode;                       //!< Transponder mode (off=0,stdby=1,on=2,test=3)
            bool xpdrIdent;                     //!< Is transponder in ident?
            bool beaconLightsOn;                //!< Beacon lights on?
            bool landingLightsOn;               //!< Landing lights on?
            bool navLightsOn;                   //!< NAV lights on?
            bool strobeLightsOn;                //!< Strobe lights on?
            bool taxiLightsOn;                  //!< Taxi lights on?
            double flapsReployRatio;            //!< Flaps deployment ratio [%]
            double gearReployRatio;             //!< Gear deployment ratio [%]
            QList<double> enginesN1Percentage;  //!< N1 per engine [%]
            double speedBrakeRatio;             //!< Speed break ratio [%]
            double seaLeveLPressure;            //!< Sea level pressure [inhg]
        };

        //! X-Plane ISimulator implementation
        class CSimulatorXPlane : public BlackCore::CSimulatorCommon
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorXPlane(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                             BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                             BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                             BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
                             BlackMisc::Network::IClientProvider *clientProvider,
                             QObject *parent = nullptr);

            //! \name ISimulator implementations
            //! @{
            virtual bool isTimeSynchronized() const override { return false; } // TODO: Can we query the XP intrinisc feature?
            virtual bool connectTo() override;
            virtual bool disconnectFrom() override;
            virtual bool physicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &newRemoteAircraft) override;
            virtual bool physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual int physicallyRemoveAllRemoteAircraft() override;
            virtual BlackMisc::Aviation::CCallsignSet physicallyRenderedAircraft() const override;
            virtual bool isPhysicallyRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual bool changeRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft) override;
            virtual bool changeRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft) override;
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateOwnSimulatorSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;
            virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override { return BlackMisc::PhysicalQuantities::CTime(0, BlackMisc::PhysicalQuantities::CTimeUnit::hrmin()); }
            virtual void unload() override;
            //! @}

            //! \copydoc BlackMisc::Simulation::ISimulationEnvironmentProvider::requestElevation
            virtual bool requestElevation(const BlackMisc::Geo::ICoordinateGeodetic &reference, const BlackMisc::Aviation::CCallsign &callsign) override;

            //! Creates an appropriate dbus connection from the string describing it
            static QDBusConnection connectionFromString(const QString &str);

        protected:
            //! \copydoc BlackCore::ISimulator::isConnected
            virtual bool isConnected() const override;

            //! \copydoc BlackCore::ISimulator::isPaused
            virtual bool isPaused() const override
            {
                //! \todo XP: provide correct pause state
                return false;
            }

            //! \copydoc BlackCore::ISimulator::isSimulating
            virtual bool isSimulating() const override { return isConnected(); }

            //! \name Base class overrides
            //! @{
            virtual void injectWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid) override;
            //! @}

        private:
            void serviceUnregistered();
            void setAirportsInRange(const QStringList &icaoCodes, const QStringList &names, const BlackMisc::CSequence<double> &lats, const BlackMisc::CSequence<double> &lons, const BlackMisc::CSequence<double> &alts);
            void emitOwnAircraftModelChanged(const QString &path, const QString &filename, const QString &livery, const QString &icao,
                                             const QString &modelString, const QString &name, const QString &description);
            void fastTimerTimeout();
            void slowTimerTimeout();

            void loadCslPackages();
            QString findCslPackage(const QString &modelFileName);

            //! Update remote aircraft
            //! \remark this is where the interpolated data are set
            void updateRemoteAircraft();

            //! Update remote aircraft parts (send to XSwiftBus)
            bool updateRemoteAircraftParts(const CXPlaneMPAircraft &xplaneAircraft, const BlackMisc::Simulation::CInterpolationResult &result);

            //! Send parts to simulator
            bool sendRemoteAircraftPartsToSimulator(const CXPlaneMPAircraft &xplaneAircraft, const BlackMisc::Aviation::CAircraftParts &parts);

            void requestRemoteAircraftDataFromXPlane();
            void updateRemoteAircraftFromSimulator(const QString &callsign, double latitudeDeg, double longitudeDeg, double elevationMeters, double modelVerticalOffsetMeters);
            void updateAirportsInRange();

            static constexpr int GuessRemoteAircraftPartsCycle = 20; //!< guess every n-th cycle

            // XSwiftBus interpolation
            QDBusConnection m_conn { "default" };
            QDBusServiceWatcher *m_watcher { nullptr };
            CXSwiftBusServiceProxy *m_serviceProxy { nullptr };
            CXSwiftBusTrafficProxy *m_trafficProxy { nullptr };
            CXSwiftBusWeatherProxy *m_weatherProxy { nullptr };
            QTimer m_fastTimer;
            QTimer m_slowTimer;
            QTimer m_airportUpdater;
            BlackMisc::Aviation::CAirportList m_airportsInRange; //!< aiports in range of own aircraft
            BlackMisc::CData<BlackMisc::Simulation::Data::TModelSetCacheXP> m_modelSet { this };

            // Driver Interpolation
            CXPlaneMPAircraftObjects m_xplaneAircraftObjects; //!< XPlane multiplayer aircraft
            int m_interpolationRequest = 0; //!< current interpolation request

            XPlaneData m_xplaneData;

            void resetXPlaneData()
            {
                m_xplaneData = { "", "", 0, 0, 0, 0, 0, 0, 0, false, 122800, 122800, 122800, 122800, 2000, 0, false, false, false, false,
                                 false, false, 0, 0, {}, 0.0, 0.0
                               };

            }
        };

        //! Listener waits for xswiftbus service to show up
        class CSimulatorXPlaneListener : public BlackCore::ISimulatorListener
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorXPlaneListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        protected:
            //! \copydoc BlackCore::ISimulatorListener::startImpl
            virtual void startImpl() override;

            //! \copydoc BlackCore::ISimulatorListener::stopImpl
            virtual void stopImpl() override;

        private:
            //! Check if XSwiftBus service is already registered
            bool isXSwiftBusRunning() const;

            void serviceRegistered(const QString &serviceName);
            void xSwiftBusServerSettingChanged();

            QDBusConnection m_conn { "default" };
            QDBusServiceWatcher *m_watcher { nullptr };
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TXSwiftBusServer> m_xswiftbusServerSetting { this, &CSimulatorXPlaneListener::xSwiftBusServerSettingChanged };
        };

        //! Factory for creating CSimulatorXPlane instance
        class CSimulatorXPlaneFactory : public QObject, public BlackCore::ISimulatorFactory
        {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "org.swift-project.blackcore.simulatorinterface" FILE "simulatorxplane.json")
            Q_INTERFACES(BlackCore::ISimulatorFactory)

        public:
            //! \copydoc BlackCore::ISimulatorFactory::create
            virtual BlackCore::ISimulator *create(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                                                  BlackMisc::Simulation::IOwnAircraftProvider    *ownAircraftProvider,
                                                  BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                                  BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
                                                  BlackMisc::Network::IClientProvider *clientProvider) override;

            //! \copydoc BlackCore::ISimulatorFactory::createListener
            virtual BlackCore::ISimulatorListener *createListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info) override { return new CSimulatorXPlaneListener(info); }
        };
    } // ns
} // ns

#endif // guard
