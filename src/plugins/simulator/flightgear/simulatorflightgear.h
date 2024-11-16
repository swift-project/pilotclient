// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_SIMULATOR_FLIGHTGEAR_H
#define SWIFT_SIMPLUGIN_SIMULATOR_FLIGHTGEAR_H

#include <QDBusConnection>
#include <QHash>
#include <QList>
#include <QObject>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "flightgearmpaircraft.h"

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
#include "misc/simulation/settings/fgswiftbussettings.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/statusmessage.h"
#include "plugins/simulator/flightgearconfig/simulatorflightgearconfig.h"
#include "plugins/simulator/plugincommon/simulatorplugincommon.h"

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

namespace swift::simplugin::flightgear
{
    extern int FGSWIFTBUS_API_VERSION;
    extern QList<int> incompatibleVersions;
    class CFGSwiftBusServiceProxy;
    class CFGSwiftBusTrafficProxy;

    //! Flightgear data
    struct FlightgearData
    {
        QString aircraftModelPath; //!< Aircraft model path
        QString aircraftIcaoCode; //!< Aircraft ICAO code
        double latitudeDeg = 0; //!< Longitude [deg]
        double longitudeDeg = 0; //!< Latitude [deg]
        double altitudeFt = 0; //!< Altitude [ft]
        double groundspeedKts = 0; //!< Ground speed [kts]
        double pitchDeg = 0; //!< Pitch [deg]
        double rollDeg = 0; //!< Roll [deg]
        double trueHeadingDeg = 0; //!< True heading [deg]
        double velocityXMs = 0; //!< x velocity [m/s]
        double velocityYMs = 0; //!< y velocity [m/s]
        double velocityZMs = 0; //!< z velocity [m/s]
        double pitchRateRadPerSec = 0; //!< Pitch angular velocity [rad/s]
        double rollRateRadPerSec = 0; //!< Roll angular velocity [rad/s]
        double yawRateRadPerSec = 0; //!< Yaw angular velocity [rad/s]
        bool onGroundAll = false; //!< All wheels on ground?
        int com1ActiveKhz = 122800; //!< COM1 active [kHz]
        int com1StandbyKhz = 122800; //!< COM1 standby [kHz]
        int com2ActiveKhz = 122800; //!< COM2 active [kHz]
        int com2StandbyKhz = 122800; //!< COM2 standby [kHz]
        int xpdrCode = 2000; //!< Transpondder code
        int xpdrMode = 0; //!< Transponder mode (off=0,stdby=1-2, >2 on)
        bool xpdrIdent = false; //!< Is transponder in ident?
        bool beaconLightsOn = false; //!< Beacon lights on?
        bool landingLightsOn = false; //!< Landing lights on?
        bool navLightsOn = false; //!< NAV lights on?
        bool strobeLightsOn = false; //!< Strobe lights on?
        bool taxiLightsOn = false; //!< Taxi lights on?
        double flapsReployRatio = false; //!< Flaps deployment ratio [%]
        double gearReployRatio = false; //!< Gear deployment ratio [%]
        QList<double> enginesN1Percentage; //!< N1 per engine [%]
        double speedBrakeRatio = 0; //!< Speed break ratio [%]
        double pressureAltitudeFt = 0; //!< Pressure altitude [inhg]
        double groundElevation = 0; //!< Ground Elevation [m]
        double volumeCom1 = 1; //!< Volume com1 [0..1]
        double volumeCom2 = 1; //!< Volume com2 [0..1]
    };

    //! Flightgear ISimulator implementation
    class CSimulatorFlightgear : public common::CSimulatorPluginCommon
    {
        Q_OBJECT

    public:
        //! Constructor
        CSimulatorFlightgear(const swift::misc::simulation::CSimulatorPluginInfo &info,
                             swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                             swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                             swift::misc::network::IClientProvider *clientProvider, QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorFlightgear() override;

        //! \name ISimulator implementations
        //! @{
        virtual bool isTimeSynchronized() const override { return false; }
        virtual bool connectTo() override;
        virtual bool disconnectFrom() override;
        virtual bool updateOwnSimulatorCockpit(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                                               const swift::misc::CIdentifier &originator) override;
        virtual bool updateOwnSimulatorSelcal(const swift::misc::aviation::CSelcal &selcal,
                                              const swift::misc::CIdentifier &originator) override;
        virtual void displayStatusMessage(const swift::misc::CStatusMessage &message) const override;
        virtual void displayTextMessage(const swift::misc::network::CTextMessage &message) const override;
        virtual swift::misc::aviation::CAirportList getAirportsInRange(bool recalculateDistance) const override;
        virtual bool setTimeSynchronization(bool enable,
                                            const swift::misc::physical_quantities::CTime &offset) override;
        virtual swift::misc::physical_quantities::CTime getTimeSynchronizationOffset() const override
        {
            return swift::misc::physical_quantities::CTime(0, swift::misc::physical_quantities::CTimeUnit::hrmin());
        }
        virtual bool isPhysicallyRenderedAircraft(const swift::misc::aviation::CCallsign &callsign) const override;
        virtual swift::misc::aviation::CCallsignSet physicallyRenderedAircraft() const override;
        virtual void unload() override;
        virtual QString getStatisticsSimulatorSpecific() const override;
        virtual void resetAircraftStatistics() override;
        virtual swift::misc::CStatusMessageList
        getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const override;
        virtual bool testSendSituationAndParts(const swift::misc::aviation::CCallsign &callsign,
                                               const swift::misc::aviation::CAircraftSituation &situation,
                                               const swift::misc::aviation::CAircraftParts &parts) override;
        virtual bool requestElevation(const swift::misc::geo::ICoordinateGeodetic &reference,
                                      const swift::misc::aviation::CCallsign &callsign) override;
        //! @}

    protected:
        //! \name ISimulator implementations
        //! @{
        virtual bool isConnected() const override;
        virtual bool
        physicallyAddRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &newRemoteAircraft) override;
        virtual bool physicallyRemoveRemoteAircraft(const swift::misc::aviation::CCallsign &callsign) override;
        virtual int physicallyRemoveAllRemoteAircraft() override;
        virtual void clearAllRemoteAircraftData() override;
        virtual bool isPaused() const override { return false; }
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

        void setAirportsInRange(const QStringList &icaoCodes, const QStringList &names,
                                const swift::misc::CSequence<double> &lats, const swift::misc::CSequence<double> &lons,
                                const swift::misc::CSequence<double> &alts);
        void emitOwnAircraftModelChanged(const QString &path, const QString &filename, const QString &livery,
                                         const QString &icao, const QString &modelString, const QString &name,
                                         const QString &description);
        void fastTimerTimeout();
        void slowTimerTimeout();

        //! Update remote aircraft
        //! \remark this is where the interpolated data are set
        void updateRemoteAircraft();

        //! @{
        //! Request elevation and CG from Flightgear
        void requestRemoteAircraftDataFromFlightgear();
        void requestRemoteAircraftDataFromFlightgear(const swift::misc::aviation::CCallsignSet &callsigns);
        void triggerRequestRemoteAircraftDataFromFlightgear(const swift::misc::aviation::CCallsignSet &callsigns);
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

        //! Can the next aircraft be added?
        bool canAddAircraft() const;

        //! @{
        //! Callbacks from simulator
        void onRemoteAircraftAdded(const QString &callsign);
        void onRemoteAircraftAddingFailed(const QString &callsign);
        void updateRemoteAircraftFromSimulator(const QStringList &callsigns, const QDoubleList &latitudesDeg,
                                               const QDoubleList &longitudesDeg, const QDoubleList &elevationsMeters,
                                               const QDoubleList &verticalOffsetsMeters);
        //! @}

        //! Dsiconnect from DBus
        void disconnectFromDBus();

        DBusMode m_dbusMode;
        swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TFGSwiftBusServer> m_fgswiftbusServerSetting {
            this
        };
        static constexpr qint64 TimeoutAdding = 10000;
        QDBusConnection m_dBusConnection { "default" };
        QDBusServiceWatcher *m_watcher { nullptr };
        CFGSwiftBusServiceProxy *m_serviceProxy { nullptr };
        CFGSwiftBusTrafficProxy *m_trafficProxy { nullptr };
        QTimer m_fastTimer;
        QTimer m_slowTimer;
        QTimer m_airportUpdater;
        QTimer m_pendingAddedTimer;
        swift::misc::aviation::CAirportList m_airportsInRange; //!< aiports in range of own aircraft
        swift::misc::CData<swift::misc::simulation::data::TModelSetCacheFG> m_modelSet {
            this
        }; //!< Flightgear model set
        swift::misc::simulation::CSimulatedAircraftList m_pendingToBeAddedAircraft; //!< aircraft to be added
        QHash<swift::misc::aviation::CCallsign, qint64> m_addingInProgressAircraft; //!< aircraft just adding
        swift::misc::simulation::CSimulatedAircraftList m_aircraftAddedFailed; //! aircraft for which adding failed
        CFlightgearMPAircraftObjects m_flightgearAircraftObjects; //!< Flightgear multiplayer aircraft
        FlightgearData m_flightgearData; //!< Flightgear data

        // statistics
        qint64 m_statsAddMaxTimeMs = -1;
        qint64 m_statsAddCurrentTimeMs = -1;

        bool m_simulatorPaused = false;

        //! Reset the Flightgear data
        void resetFlightgearData() { m_flightgearData = {}; }
    };

    //! Listener waits for fgswiftbus service to show up
    class CSimulatorFlightgearListener : public swift::core::ISimulatorListener
    {
        Q_OBJECT

    public:
        //! Constructor
        CSimulatorFlightgearListener(const swift::misc::simulation::CSimulatorPluginInfo &info);

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
        void fgSwiftBusServerSettingChanged();

        QTimer m_timer { this };
        QDBusConnection m_conn { "default" };
        swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TFGSwiftBusServer> m_fgSswiftBusServerSetting {
            this, &CSimulatorFlightgearListener::fgSwiftBusServerSettingChanged
        };
    };

    //! Factory for creating CSimulatorFlightgear instance
    class CSimulatorFlightgearFactory : public QObject, public swift::core::ISimulatorFactory
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.swift_core.simulatorinterface" FILE "simulatorflightgear.json")
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
            return new CSimulatorFlightgearListener(info);
        }
    };
} // namespace swift::simplugin::flightgear

#endif // guard
