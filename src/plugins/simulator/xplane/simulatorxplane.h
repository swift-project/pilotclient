/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_XPLANE_H
#define BLACKSIMPLUGIN_SIMULATOR_XPLANE_H

#include "xplanempaircraft.h"
#include "plugins/simulator/xplaneconfig/simulatorxplaneconfig.h"
#include "plugins/simulator/plugincommon/simulatorplugincommon.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/settings/xswiftbussettings.h"
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

namespace BlackSimPlugin::XPlane
{
    class CXSwiftBusServiceProxy;
    class CXSwiftBusTrafficProxy;
    class CXSwiftBusWeatherProxy;

    //! X-Plane data
    struct XPlaneData
    {
        QString aircraftModelPath;          //!< Aircraft model path
        QString aircraftIcaoCode;           //!< Aircraft ICAO code
        double latitudeDeg = 0;             //!< Longitude [deg]
        double longitudeDeg = 0;            //!< Latitude  [deg]
        double altitudeM = 0;               //!< Altitude  [m]
        double pressureAltitudeFt = 0;      //!< Pressure altitude [ft, XP12]
        double heightAglM = 0;              //!< Height AGL [m]
        double groundspeedMs = 0;           //!< Ground speed [m/s]
        double pitchDeg = 0;                //!< Pitch [deg]
        double rollDeg = 0;                 //!< Roll  [deg]
        double trueHeadingDeg = 0;          //!< True heading [deg]
        double localXVelocityMs = 0;        //!< Local x velocity [m/s]
        double localYVelocityMs = 0;        //!< Local y velocity [m/s]
        double localZVelocityMs = 0;        //!< Local z velocity [m/s]
        double pitchRadPerSec = 0;          //!< Pitch angular velocity [rad/s]
        double rollRadPerSec = 0;           //!< Roll angular velocity [rad/s]
        double headingRadPerSec = 0;        //!< Heading angular velocity [rad/s]
        bool onGroundAll = false;           //!< All wheels on ground?
        double groundElevation = 0;         //!< Elevation of ground [m]
        int com1ActiveKhz = 122800;         //!< COM1 active  [kHz]
        int com1StandbyKhz = 122800;        //!< COM1 standby [kHz]
        bool isCom1Receiving = true;        //!< COM1 receiving
        bool isCom1Transmitting = true;     //!< COM1 transmittings
        double com1Volume = 1;              //!< COM1 volume 0..1
        int com2ActiveKhz = 122800;         //!< COM2 active  [kHz]
        int com2StandbyKhz = 122800;        //!< COM2 standby [kHz]
        bool isCom2Receiving = true;        //!< COM2 receiving
        bool isCom2Transmitting = true;     //!< COM2 transmittings
        double com2Volume = 1;              //!< COM2 volume 0..1
        int xpdrCode = 2000;                //!< Transpondder code
        int xpdrMode = 0;                   //!< Transponder mode (off=0,stdby=1,on=2,test=3)
        bool xpdrIdent = false;             //!< Is transponder in ident?
        bool beaconLightsOn = false;        //!< Beacon lights on?
        bool landingLightsOn = false;       //!< Landing lights on?
        bool navLightsOn = false;           //!< NAV lights on?
        bool strobeLightsOn = false;        //!< Strobe lights on?
        bool taxiLightsOn = false;          //!< Taxi lights on?
        double flapsDeployRatio = 0;        //!< Flaps deployment ratio [%]
        double gearDeployRatio = 0;         //!< Gear deployment ratio  [%]
        QList<double> enginesN1Percentage;  //!< N1 per engine [%]
        double speedBrakeRatio = 0;         //!< Speed break ratio [%]
        double seaLevelPressureInHg = 0;    //!< Sea level pressure [inhg]
    };

    //! X-Plane ISimulator implementation
    class CSimulatorXPlane : public Common::CSimulatorPluginCommon
    {
        Q_OBJECT

    public:
        //! Constructor
        CSimulatorXPlane(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                            BlackMisc::Simulation::IOwnAircraftProvider    *ownAircraftProvider,
                            BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                            BlackMisc::Weather::IWeatherGridProvider       *weatherGridProvider,
                            BlackMisc::Network::IClientProvider            *clientProvider,
                            QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorXPlane() override;

        //! \name ISimulator implementations
        //! @{
        virtual bool isTimeSynchronized() const override { return false; } // TODO: Can we query the XP intrinisc feature?
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
        virtual bool followAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void unload() override;
        virtual QString getStatisticsSimulatorSpecific() const override;
        virtual void resetAircraftStatistics() override;
        virtual BlackMisc::CStatusMessageList getInterpolationMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual bool testSendSituationAndParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CAircraftParts &parts) override;
        virtual void injectWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid) override;
        virtual void callbackReceivedRequestedElevation(const BlackMisc::Geo::CElevationPlane &plane, const BlackMisc::Aviation::CCallsign &callsign, bool isWater) override;
        virtual void setFlightNetworkConnected(bool connected) override;
        //! @}

        //! \copydoc BlackMisc::Simulation::ISimulationEnvironmentProvider::requestElevation
        virtual bool requestElevation(const BlackMisc::Geo::ICoordinateGeodetic &reference, const BlackMisc::Aviation::CCallsign &callsign) override;

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
            //! \todo XP: provide correct pause state
            return false;
        }
        //! @}

    private slots:
        //! Unregister
        void onDBusServiceUnregistered();

    private:
        //! Mode
        enum DBusMode
        {
            Session,
            P2P
        };

        using QDoubleList = QList<double>;
        using QBoolList = QList<bool>;

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

        //! Update airports
        void updateAirportsInRange();

        //! Request elevation and CG from XPlane
        //! @{
        void requestRemoteAircraftDataFromXPlane();
        void requestRemoteAircraftDataFromXPlane(const BlackMisc::Aviation::CCallsignSet &callsigns);
        void triggerRequestRemoteAircraftDataFromXPlane(const BlackMisc::Aviation::CCallsignSet &callsigns);
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

        //! Can the next aircraft be added? No other aircraft adding in progress?
        bool canAddAircraft() const;

        //! Callbacks from simulator
        //! @{
        void onRemoteAircraftAdded(const QString &callsign);
        void onRemoteAircraftAddingFailed(const QString &callsign);
        void updateRemoteAircraftFromSimulator(const QStringList &callsigns, const QDoubleList &latitudesDeg, const QDoubleList &longitudesDeg,
                                                const QDoubleList &elevationsMeters, const QBoolList &waterFlags, const QDoubleList &verticalOffsetsMeters);
        //! @}

        //! Disconnect from DBus
        void disconnectFromDBus();

        //! Send/receive settings
        //! @{
        bool sendXSwiftBusSettings();
        BlackMisc::Simulation::Settings::CXSwiftBusSettings receiveXSwiftBusSettings(bool &ok);
        //! @}

        //! Settings have changed
        void onXSwiftBusSettingsChanged();

        //! Min.distance of "failed" (suspicious) terrain probe requests
        void setMinTerrainProbeDistance(const BlackMisc::PhysicalQuantities::CLength &distance);

        //! Handle a probe value
        bool handleProbeValue(const BlackMisc::Geo::CElevationPlane &plane, const BlackMisc::Aviation::CCallsign &callsign, bool waterFlag, const QString &hint, bool ignoreOutsideRange);

        static bool isSuspiciousTerrainValue(const BlackMisc::Geo::CElevationPlane &elevation);
        static const BlackMisc::PhysicalQuantities::CLength &maxTerrainRequestDistance();

        static BlackMisc::PhysicalQuantities::CLength fixSimulatorCg(const BlackMisc::PhysicalQuantities::CLength &cg, const BlackMisc::Simulation::CAircraftModel &model);

        DBusMode m_dbusMode;
        BlackMisc::CSetting<BlackMisc::Simulation::Settings::TXSwiftBusSettings> m_xSwiftBusServerSettings { this, &CSimulatorXPlane::onXSwiftBusSettingsChanged };
        static constexpr qint64 TimeoutAdding = 10000;
        QDBusConnection m_dBusConnection     { "default" };
        QDBusServiceWatcher    *m_watcher      { nullptr };
        CXSwiftBusServiceProxy *m_serviceProxy { nullptr };
        CXSwiftBusTrafficProxy *m_trafficProxy { nullptr };
        CXSwiftBusWeatherProxy *m_weatherProxy { nullptr };
        QTimer m_fastTimer;
        QTimer m_slowTimer;
        QTimer m_airportUpdater;
        QTimer m_pendingAddedTimer;
        unsigned int m_fastTimerCalls = 0; //!< how often called
        unsigned int m_slowTimerCalls = 0; //!< how often called

        BlackMisc::Aviation::CAirportList m_airportsInRange; //!< aiports in range of own aircraft
        CXPlaneMPAircraftObjects m_xplaneAircraftObjects;    //!< XPlane multiplayer aircraft

        BlackMisc::Simulation::CSimulatedAircraftList m_pendingToBeAddedAircraft;      //!< aircraft to be added
        QHash<BlackMisc::Aviation::CCallsign, qint64> m_addingInProgressAircraft;      //!< aircraft just adding
        BlackMisc::Simulation::CSimulatedAircraftList m_aircraftAddedFailed;           //!< aircraft for which adding failed
        BlackMisc::PhysicalQuantities::CLength m_minSuspicousTerrainProbe { nullptr }; //!< min. distance of "failed" (suspicious) terrain probe requests
        XPlaneData m_xplaneData; //!< XPlane data
        BlackMisc::PhysicalQuantities::CLength m_altitudeDelta;     //!< XP12 altitude difference cause by temperature effect

        // statistics
        qint64 m_statsAddMaxTimeMs     = -1;
        qint64 m_statsAddCurrentTimeMs = -1;

        //! Reset the XPlane data
        void resetXPlaneData()
        {
            m_xplaneData = {};
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

        //! \copydoc BlackCore::ISimulatorListener::checkImpl
        virtual void checkImpl() override;

    private:
        void checkConnection();
        void checkConnectionViaSessionBus();
        void checkConnectionViaPeer(const QString &address);
        void checkConnectionCommon();

        void serviceRegistered(const QString &serviceName);
        void onXSwiftBusServerSettingChanged();

        QTimer m_timer { this };
        QDBusConnection m_DBusConnection { "default" };
        QString m_dBusServerAddress;
        BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TXSwiftBusSettings> m_xSwiftBusServerSettings { this, &CSimulatorXPlaneListener::onXSwiftBusServerSettingChanged };
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
                                                BlackMisc::Weather::IWeatherGridProvider       *weatherGridProvider,
                                                BlackMisc::Network::IClientProvider            *clientProvider) override;

        //! \copydoc BlackCore::ISimulatorFactory::createListener
        virtual BlackCore::ISimulatorListener *createListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info) override { return new CSimulatorXPlaneListener(info); }
    };
} // ns

#endif // guard
