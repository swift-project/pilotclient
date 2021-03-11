/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FLIGHTGEAR_H
#define BLACKSIMPLUGIN_SIMULATOR_FLIGHTGEAR_H

#include "flightgearmpaircraft.h"
#include "plugins/simulator/flightgearconfig/simulatorflightgearconfig.h"
#include "plugins/simulator/plugincommon/simulatorplugincommon.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/settings/fgswiftbussettings.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/weather/weathergrid.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
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
#include <QHash>
#include <QPair>
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
    namespace Flightgear
    {
        extern int FGSWIFTBUS_API_VERSION;
        extern QList<int> incompatibleVersions;
        class CFGSwiftBusServiceProxy;
        class CFGSwiftBusTrafficProxy;

        //! Flightgear data
        struct FlightgearData
        {
            QString aircraftModelPath;          //!< Aircraft model path
            QString aircraftIcaoCode;           //!< Aircraft ICAO code
            double latitudeDeg = 0;             //!< Longitude [deg]
            double longitudeDeg = 0;            //!< Latitude [deg]
            double altitudeFt = 0;              //!< Altitude [ft]
            double groundspeedKts = 0;          //!< Ground speed [kts]
            double pitchDeg = 0;                //!< Pitch [deg]
            double rollDeg = 0;                 //!< Roll [deg]
            double trueHeadingDeg = 0;          //!< True heading [deg]
            double velocityXMs = 0;             //!< x velocity [m/s]
            double velocityYMs = 0;             //!< y velocity [m/s]
            double velocityZMs = 0;             //!< z velocity [m/s]
            double pitchRateRadPerSec = 0;      //!< Pitch angular velocity [rad/s]
            double rollRateRadPerSec = 0;       //!< Roll angular velocity [rad/s]
            double yawRateRadPerSec = 0;        //!< Yaw angular velocity [rad/s]
            bool onGroundAll = false;           //!< All wheels on ground?
            int com1ActiveKhz = 122800;         //!< COM1 active [kHz]
            int com1StandbyKhz = 122800;        //!< COM1 standby [kHz]
            int com2ActiveKhz = 122800;         //!< COM2 active [kHz]
            int com2StandbyKhz = 122800;        //!< COM2 standby [kHz]
            int xpdrCode = 2000;                //!< Transpondder code
            int xpdrMode = 0;                   //!< Transponder mode (off=0,stdby=1-2, >2 on)
            bool xpdrIdent = false;             //!< Is transponder in ident?
            bool beaconLightsOn = false;        //!< Beacon lights on?
            bool landingLightsOn = false;       //!< Landing lights on?
            bool navLightsOn = false;           //!< NAV lights on?
            bool strobeLightsOn = false;        //!< Strobe lights on?
            bool taxiLightsOn = false;          //!< Taxi lights on?
            double flapsReployRatio = false;    //!< Flaps deployment ratio [%]
            double gearReployRatio = false;     //!< Gear deployment ratio [%]
            QList<double> enginesN1Percentage;  //!< N1 per engine [%]
            double speedBrakeRatio = 0;         //!< Speed break ratio [%]
            double pressureAltitudeFt = 0;      //!< Pressure altitude [inhg]
            double groundElevation = 0;         //!< Ground Elevation [m]
        };

        //! Flightgear ISimulator implementation
        class CSimulatorFlightgear : public Common::CSimulatorPluginCommon
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorFlightgear(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                                 BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                                 BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                 BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
                                 BlackMisc::Network::IClientProvider *clientProvider,
                                 QObject *parent = nullptr);

            //! Dtor
            virtual ~CSimulatorFlightgear() override;

            //! \name ISimulator implementations
            //! @{
            virtual bool isTimeSynchronized() const override { return false; }
            virtual bool connectTo() override;
            virtual bool disconnectFrom() override;
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateOwnSimulatorSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange(bool recalculateDistance) const override;
            virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override { return BlackMisc::PhysicalQuantities::CTime(0, BlackMisc::PhysicalQuantities::CTimeUnit::hrmin()); }
            virtual bool isPhysicallyRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Aviation::CCallsignSet physicallyRenderedAircraft() const override;
            virtual void unload() override;
            virtual QString getStatisticsSimulatorSpecific() const override;
            virtual void resetAircraftStatistics() override;
            virtual BlackMisc::CStatusMessageList getInterpolationMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual bool testSendSituationAndParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CAircraftParts &parts) override;
            virtual bool requestElevation(const BlackMisc::Geo::ICoordinateGeodetic &reference, const BlackMisc::Aviation::CCallsign &callsign) override;
            //! @}

        protected:
            //! \name ISimulator implementations
            //! @{
            virtual bool isConnected() const override;
            virtual bool physicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &newRemoteAircraft) override;
            virtual bool physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual int physicallyRemoveAllRemoteAircraft() override;
            virtual void clearAllRemoteAircraftData() override;
            virtual bool isPaused() const override
            {
                return false;
            }
            //! @}

        private slots:
            //! Unregister
            void onDBusServiceUnregistered();

        private:
            enum DBusMode
            {
                Session,
                P2P
            };

            using QDoubleList = QList<double>;

            void setAirportsInRange(const QStringList &icaoCodes, const QStringList &names, const BlackMisc::CSequence<double> &lats, const BlackMisc::CSequence<double> &lons, const BlackMisc::CSequence<double> &alts);
            void emitOwnAircraftModelChanged(const QString &path, const QString &filename, const QString &livery, const QString &icao,
                                             const QString &modelString, const QString &name, const QString &description);
            void fastTimerTimeout();
            void slowTimerTimeout();

            //! Update remote aircraft
            //! \remark this is where the interpolated data are set
            void updateRemoteAircraft();

            //! Request elevation and CG from Flightgear
            //! @{
            void requestRemoteAircraftDataFromFlightgear();
            void requestRemoteAircraftDataFromFlightgear(const BlackMisc::Aviation::CCallsignSet &callsigns);
            void triggerRequestRemoteAircraftDataFromFlightgear(const BlackMisc::Aviation::CCallsignSet &callsigns);
            //! @}

            //! Adding new aircraft
            //! @{
            void addNextPendingAircraft();
            void triggerAddNextPendingAircraft();
            //! @}

            //! Detect timeouts on adding
            int detectTimeoutAdding();

            //! Trigger a removal of an aircraft
            void triggerRemoveAircraft(const BlackMisc::Aviation::CCallsign &callsign, qint64 deferMs);

            //! Timestamps of aircraft currently adding
            QPair<qint64, qint64> minMaxTimestampsAddInProgress() const;

            //! Can the next aircraft be added?
            bool canAddAircraft() const;

            //! Callbacks from simulator
            //! @{
            void onRemoteAircraftAdded(const QString &callsign);
            void onRemoteAircraftAddingFailed(const QString &callsign);
            void updateRemoteAircraftFromSimulator(const QStringList &callsigns, const QDoubleList &latitudesDeg, const QDoubleList &longitudesDeg,
                                                   const QDoubleList &elevationsMeters, const QDoubleList &verticalOffsetsMeters);
            //! @}

            //! Dsiconnect from DBus
            void disconnectFromDBus();

            DBusMode m_dbusMode;
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TFGSwiftBusServer> m_fgswiftbusServerSetting { this };
            static constexpr qint64 TimeoutAdding = 10000;
            QDBusConnection m_dBusConnection { "default" };
            QDBusServiceWatcher *m_watcher { nullptr };
            CFGSwiftBusServiceProxy *m_serviceProxy { nullptr };
            CFGSwiftBusTrafficProxy *m_trafficProxy { nullptr };
            QTimer m_fastTimer;
            QTimer m_slowTimer;
            QTimer m_airportUpdater;
            QTimer m_pendingAddedTimer;
            BlackMisc::Aviation::CAirportList m_airportsInRange; //!< aiports in range of own aircraft
            BlackMisc::CData<BlackMisc::Simulation::Data::TModelSetCacheFG> m_modelSet { this }; //!< Flightgear model set
            BlackMisc::Simulation::CSimulatedAircraftList m_pendingToBeAddedAircraft; //!< aircraft to be added
            QHash<BlackMisc::Aviation::CCallsign, qint64> m_addingInProgressAircraft; //!< aircraft just adding
            BlackMisc::Simulation::CSimulatedAircraftList m_aircraftAddedFailed; //! aircraft for which adding failed
            CFlightgearMPAircraftObjects m_flightgearAircraftObjects; //!< Flightgear multiplayer aircraft
            FlightgearData m_flightgearData; //!< Flightgear data

            // statistics
            qint64 m_statsAddMaxTimeMs = -1;
            qint64 m_statsAddCurrentTimeMs = -1;

            //! Reset the Flightgear data
            void resetFlightgearData()
            {
                m_flightgearData = {};

            }
        };

        //! Listener waits for fgswiftbus service to show up
        class CSimulatorFlightgearListener : public BlackCore::ISimulatorListener
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorFlightgearListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        protected:
            //! \copydoc BlackCore::ISimulatorListener::startImpl
            virtual void startImpl() override;

            //! \copydoc BlackCore::ISimulatorListener::stopImpl
            virtual void stopImpl() override;

            //! \copydoc BlackCore::ISimulatorListener::checkImpl
            virtual void checkImpl() override;

        private:
            void checkConnection();
            void checkConnectionViaSessionBus();
            void checkConnectionViaPeer(const QString &address);
            void checkConnectionCommon();

            void serviceRegistered(const QString &serviceName);
            void fgSwiftBusServerSettingChanged();

            QTimer m_timer { this };
            QDBusConnection m_conn { "default" };
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TFGSwiftBusServer> m_fgSswiftBusServerSetting { this, &CSimulatorFlightgearListener::fgSwiftBusServerSettingChanged };
        };

        //! Factory for creating CSimulatorFlightgear instance
        class CSimulatorFlightgearFactory : public QObject, public BlackCore::ISimulatorFactory
        {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "org.swift-project.blackcore.simulatorinterface" FILE "simulatorflightgear.json")
            Q_INTERFACES(BlackCore::ISimulatorFactory)

        public:
            //! \copydoc BlackCore::ISimulatorFactory::create
            virtual BlackCore::ISimulator *create(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                                                  BlackMisc::Simulation::IOwnAircraftProvider    *ownAircraftProvider,
                                                  BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                                  BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
                                                  BlackMisc::Network::IClientProvider *clientProvider) override;

            //! \copydoc BlackCore::ISimulatorFactory::createListener
            virtual BlackCore::ISimulatorListener *createListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info) override { return new CSimulatorFlightgearListener(info); }
        };
    } // ns
} // ns

#endif // guard
