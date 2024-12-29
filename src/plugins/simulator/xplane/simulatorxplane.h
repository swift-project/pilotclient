// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_SIMULATOR_XPLANE_H
#define SWIFT_SIMPLUGIN_SIMULATOR_XPLANE_H

#include <QDBusConnection>
#include <QHash>
#include <QList>
#include <QObject>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "xplanempaircraft.h"

#include "misc/aviation/airportlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/identifier.h"
#include "misc/pixmap.h"
#include "misc/pq/time.h"
#include "misc/pq/units.h"
#include "misc/sequence.h"
#include "misc/settingscache.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/settings/xswiftbussettings.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/statusmessage.h"
#include "plugins/simulator/plugincommon/simulatorplugincommon.h"
#include "plugins/simulator/xplaneconfig/simulatorxplaneconfig.h"

class QDBusServiceWatcher;

namespace swift::misc
{
    namespace aviation
    {
        class CAircraftParts;
        class CAircraftSituation;
        class CCallsign;
    } // namespace aviation
    namespace network
    {
        class CTextMessage;
    }
    namespace simulation
    {
        class CSimulatedAircraft;
        class CSimulatorPluginInfo;
        class IOwnAircraftProvider;
        class IRemoteAircraftProvider;
    } // namespace simulation
} // namespace swift::misc

namespace swift::simplugin::xplane
{
    class CXSwiftBusServiceProxy;
    class CXSwiftBusTrafficProxy;

    //! X-Plane data
    struct XPlaneData
    {
        QString aircraftModelPath; //!< Aircraft model path
        QString aircraftIcaoCode; //!< Aircraft ICAO code
        double latitudeDeg = 0; //!< Longitude [deg]
        double longitudeDeg = 0; //!< Latitude  [deg]
        double altitudeM = 0; //!< Altitude  [m]
        double pressureAltitudeFt = 0; //!< Pressure altitude [ft, XP12]
        double heightAglM = 0; //!< Height AGL [m]
        double groundspeedMs = 0; //!< Ground speed [m/s]
        double pitchDeg = 0; //!< Pitch [deg]
        double rollDeg = 0; //!< Roll  [deg]
        double trueHeadingDeg = 0; //!< True heading [deg]
        double localXVelocityMs = 0; //!< Local x velocity [m/s]
        double localYVelocityMs = 0; //!< Local y velocity [m/s]
        double localZVelocityMs = 0; //!< Local z velocity [m/s]
        double pitchRadPerSec = 0; //!< Pitch angular velocity [rad/s]
        double rollRadPerSec = 0; //!< Roll angular velocity [rad/s]
        double headingRadPerSec = 0; //!< Heading angular velocity [rad/s]
        bool onGroundAll = false; //!< All wheels on ground?
        double groundElevation = 0; //!< Elevation of ground [m]
        int com1ActiveKhz = 122800; //!< COM1 active  [kHz]
        int com1StandbyKhz = 122800; //!< COM1 standby [kHz]
        bool isCom1Receiving = true; //!< COM1 receiving
        bool isCom1Transmitting = true; //!< COM1 transmittings
        double com1Volume = 1; //!< COM1 volume 0..1
        int com2ActiveKhz = 122800; //!< COM2 active  [kHz]
        int com2StandbyKhz = 122800; //!< COM2 standby [kHz]
        bool isCom2Receiving = true; //!< COM2 receiving
        bool isCom2Transmitting = true; //!< COM2 transmittings
        double com2Volume = 1; //!< COM2 volume 0..1
        int xpdrCode = 2000; //!< Transpondder code
        int xpdrMode = 0; //!< Transponder mode (off=0,stdby=1,on=2,test=3)
        bool xpdrIdent = false; //!< Is transponder in ident?
        bool beaconLightsOn = false; //!< Beacon lights on?
        bool landingLightsOn = false; //!< Landing lights on?
        bool navLightsOn = false; //!< NAV lights on?
        bool strobeLightsOn = false; //!< Strobe lights on?
        bool taxiLightsOn = false; //!< Taxi lights on?
        double flapsDeployRatio = 0; //!< Flaps deployment ratio [%]
        double gearDeployRatio = 0; //!< Gear deployment ratio  [%]
        QList<double> enginesN1Percentage; //!< N1 per engine [%]
        double speedBrakeRatio = 0; //!< Speed break ratio [%]
        double seaLevelPressureInHg = 0; //!< Sea level pressure [inhg]
    };

    //! X-Plane ISimulator implementation
    class CSimulatorXPlane : public common::CSimulatorPluginCommon
    {
        Q_OBJECT

    public:
        //! Constructor
        CSimulatorXPlane(const swift::misc::simulation::CSimulatorPluginInfo &info,
                         swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                         swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                         swift::misc::network::IClientProvider *clientProvider, QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorXPlane() override;

        //! \name ISimulator implementations
        //! @{
        virtual bool connectTo() override;
        virtual bool disconnectFrom() override;
        virtual bool updateOwnSimulatorCockpit(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                                               const swift::misc::CIdentifier &originator) override;
        virtual bool updateOwnSimulatorSelcal(const swift::misc::aviation::CSelcal &selcal,
                                              const swift::misc::CIdentifier &originator) override;
        virtual void displayStatusMessage(const swift::misc::CStatusMessage &message) const override;
        virtual void displayTextMessage(const swift::misc::network::CTextMessage &message) const override;
        virtual swift::misc::aviation::CAirportList getAirportsInRange(bool recalculateDistance) const override;
        virtual bool isPhysicallyRenderedAircraft(const swift::misc::aviation::CCallsign &callsign) const override;
        virtual swift::misc::aviation::CCallsignSet physicallyRenderedAircraft() const override;
        virtual bool followAircraft(const swift::misc::aviation::CCallsign &callsign) override;
        virtual void unload() override;
        virtual QString getStatisticsSimulatorSpecific() const override;
        virtual void resetAircraftStatistics() override;
        virtual swift::misc::CStatusMessageList
        getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const override;
        virtual bool testSendSituationAndParts(const swift::misc::aviation::CCallsign &callsign,
                                               const swift::misc::aviation::CAircraftSituation &situation,
                                               const swift::misc::aviation::CAircraftParts &parts) override;
        virtual void callbackReceivedRequestedElevation(const swift::misc::geo::CElevationPlane &plane,
                                                        const swift::misc::aviation::CCallsign &callsign,
                                                        bool isWater) override;
        virtual void setFlightNetworkConnected(bool connected) override;
        //! @}

        //! \copydoc swift::misc::simulation::ISimulationEnvironmentProvider::requestElevation
        virtual bool requestElevation(const swift::misc::geo::ICoordinateGeodetic &reference,
                                      const swift::misc::aviation::CCallsign &callsign) override;

    protected:
        //! \name ISimulator implementations
        //! @{
        virtual bool isConnected() const override;
        virtual bool
        physicallyAddRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &newRemoteAircraft) override;
        virtual bool physicallyRemoveRemoteAircraft(const swift::misc::aviation::CCallsign &callsign) override;
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

        void setAirportsInRange(const QStringList &icaoCodes, const QStringList &names,
                                const swift::misc::CSequence<double> &lats, const swift::misc::CSequence<double> &lons,
                                const swift::misc::CSequence<double> &alts);
        void emitOwnAircraftModelChanged(const QString &path, const QString &filename, const QString &livery,
                                         const QString &icao, const QString &modelString, const QString &name,
                                         const QString &description);
        void fastTimerTimeout();
        void slowTimerTimeout();

        void loadCslPackages();
        QString findCslPackage(const QString &modelFileName);

        //! Update remote aircraft
        //! \remark this is where the interpolated data are set
        void updateRemoteAircraft();

        //! Update airports
        void updateAirportsInRange();

        //! @{
        //! Request elevation and CG from XPlane
        void requestRemoteAircraftDataFromXPlane();
        void requestRemoteAircraftDataFromXPlane(const swift::misc::aviation::CCallsignSet &callsigns);
        void triggerRequestRemoteAircraftDataFromXPlane(const swift::misc::aviation::CCallsignSet &callsigns);
        //! @}

        //! @{
        //! Adding new aircraft
        void addNextPendingAircraft();
        void triggerAddNextPendingAircraft();
        //! @}

        //! Detect timeouts on adding
        int detectTimeoutAdding();

        //! Trigger a removal of an aircraft
        void triggerRemoveAircraft(const swift::misc::aviation::CCallsign &callsign, qint64 deferMs);

        //! Timestamps of aircraft currently adding
        QPair<qint64, qint64> minMaxTimestampsAddInProgress() const;

        //! Can the next aircraft be added? No other aircraft adding in progress?
        bool canAddAircraft() const;

        //! @{
        //! Callbacks from simulator
        void onRemoteAircraftAdded(const QString &callsign);
        void onRemoteAircraftAddingFailed(const QString &callsign);
        void updateRemoteAircraftFromSimulator(const QStringList &callsigns, const QDoubleList &latitudesDeg,
                                               const QDoubleList &longitudesDeg, const QDoubleList &elevationsMeters,
                                               const QBoolList &waterFlags, const QDoubleList &verticalOffsetsMeters);
        //! @}

        //! Disconnect from DBus
        void disconnectFromDBus();

        //! @{
        //! Send/receive settings
        bool sendXSwiftBusSettings();
        swift::misc::simulation::settings::CXSwiftBusSettings receiveXSwiftBusSettings(bool &ok);
        //! @}

        //! Settings have changed
        void onXSwiftBusSettingsChanged();

        //! Min.distance of "failed" (suspicious) terrain probe requests
        void setMinTerrainProbeDistance(const swift::misc::physical_quantities::CLength &distance);

        //! Handle a probe value
        bool handleProbeValue(const swift::misc::geo::CElevationPlane &plane,
                              const swift::misc::aviation::CCallsign &callsign, bool waterFlag, const QString &hint,
                              bool ignoreOutsideRange);

        static bool isSuspiciousTerrainValue(const swift::misc::geo::CElevationPlane &elevation);
        static const swift::misc::physical_quantities::CLength &maxTerrainRequestDistance();

        static swift::misc::physical_quantities::CLength
        fixSimulatorCg(const swift::misc::physical_quantities::CLength &cg,
                       const swift::misc::simulation::CAircraftModel &model);

        DBusMode m_dbusMode;
        swift::misc::CSetting<swift::misc::simulation::settings::TXSwiftBusSettings> m_xSwiftBusServerSettings {
            this, &CSimulatorXPlane::onXSwiftBusSettingsChanged
        };
        static constexpr qint64 TimeoutAdding = 10000;
        QDBusConnection m_dBusConnection { "default" };
        QDBusServiceWatcher *m_watcher { nullptr };
        CXSwiftBusServiceProxy *m_serviceProxy { nullptr };
        CXSwiftBusTrafficProxy *m_trafficProxy { nullptr };
        QTimer m_fastTimer;
        QTimer m_slowTimer;
        QTimer m_airportUpdater;
        QTimer m_pendingAddedTimer;
        unsigned int m_fastTimerCalls = 0; //!< how often called
        unsigned int m_slowTimerCalls = 0; //!< how often called

        swift::misc::aviation::CAirportList m_airportsInRange; //!< aiports in range of own aircraft
        CXPlaneMPAircraftObjects m_xplaneAircraftObjects; //!< XPlane multiplayer aircraft

        swift::misc::simulation::CSimulatedAircraftList m_pendingToBeAddedAircraft; //!< aircraft to be added
        QHash<swift::misc::aviation::CCallsign, qint64> m_addingInProgressAircraft; //!< aircraft just adding
        swift::misc::simulation::CSimulatedAircraftList m_aircraftAddedFailed; //!< aircraft for which adding failed
        swift::misc::physical_quantities::CLength m_minSuspicousTerrainProbe {
            nullptr
        }; //!< min. distance of "failed" (suspicious) terrain probe requests
        XPlaneData m_xplaneData; //!< XPlane data
        swift::misc::physical_quantities::CLength
            m_altitudeDelta; //!< XP12 altitude difference cause by temperature effect

        // statistics
        qint64 m_statsAddMaxTimeMs = -1;
        qint64 m_statsAddCurrentTimeMs = -1;

        //! Reset the XPlane data
        void resetXPlaneData() { m_xplaneData = {}; }
    };

    //! Listener waits for xswiftbus service to show up
    class CSimulatorXPlaneListener : public swift::core::ISimulatorListener
    {
        Q_OBJECT

    public:
        //! Constructor
        CSimulatorXPlaneListener(const swift::misc::simulation::CSimulatorPluginInfo &info);

    protected:
        //! \copydoc swift::core::ISimulatorListener::startImpl
        virtual void startImpl() override;

        //! \copydoc swift::core::ISimulatorListener::stopImpl
        virtual void stopImpl() override;

        //! \copydoc swift::core::ISimulatorListener::checkImpl
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
        swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TXSwiftBusSettings> m_xSwiftBusServerSettings {
            this, &CSimulatorXPlaneListener::onXSwiftBusServerSettingChanged
        };
    };

    //! Factory for creating CSimulatorXPlane instance
    class CSimulatorXPlaneFactory : public QObject, public swift::core::ISimulatorFactory
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.swift_core.simulatorinterface" FILE "simulatorxplane.json")
        Q_INTERFACES(swift::core::ISimulatorFactory)

    public:
        //! \copydoc swift::core::ISimulatorFactory::create
        virtual swift::core::ISimulator *
        create(const swift::misc::simulation::CSimulatorPluginInfo &info,
               swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
               swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
               swift::misc::network::IClientProvider *clientProvider) override;

        //! \copydoc swift::core::ISimulatorFactory::createListener
        virtual swift::core::ISimulatorListener *
        createListener(const swift::misc::simulation::CSimulatorPluginInfo &info) override
        {
            return new CSimulatorXPlaneListener(info);
        }
    };
} // namespace swift::simplugin::xplane

#endif // SWIFT_SIMPLUGIN_SIMULATOR_XPLANE_H
