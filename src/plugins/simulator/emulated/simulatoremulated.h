// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATED_H
#define BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATED_H

#include "../plugincommon/simulatorplugincommon.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/simulation/interpolatormulti.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/settings/swiftpluginsettings.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/connectionguard.h"
#include "simulatoremulatedmonitordialog.h"

#include <QMap>
#include <QTimer>
#include <QPointer>

namespace BlackSimPlugin::Emulated
{
    //! swift simulator implementation
    class CSimulatorEmulated : public Common::CSimulatorPluginCommon
    {
        Q_OBJECT
        Q_INTERFACES(BlackCore::ISimulator)
        Q_INTERFACES(BlackMisc::Simulation::ISimulationEnvironmentProvider)
        Q_INTERFACES(BlackMisc::Simulation::IInterpolationSetupProvider)

        friend class CSimulatorEmulatedMonitorDialog; //!< the monitor widget represents the simulator and needs access to internals (i.e. private/protected)

    public:
        //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
        CSimulatorEmulated(
            const BlackMisc::Simulation::CSimulatorPluginInfo &info,
            BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
            BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
            BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
            BlackMisc::Network::IClientProvider *clientProvider,
            QObject *parent = nullptr);

        //! Destructor
        virtual ~CSimulatorEmulated() override;

        // functions implemented
        virtual bool isTimeSynchronized() const override;
        virtual bool connectTo() override;
        virtual bool disconnectFrom() override;
        virtual void unload() override;
        virtual bool isConnected() const override;
        virtual bool isPaused() const override;
        virtual bool isSimulating() const override;
        virtual bool changeRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft) override;
        virtual bool changeRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft) override;
        virtual bool updateOwnSimulatorCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator) override;
        virtual bool updateOwnSimulatorSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;
        virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;
        virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;
        virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;
        virtual bool isPhysicallyRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual BlackMisc::Aviation::CCallsignSet physicallyRenderedAircraft() const override;
        virtual BlackMisc::CStatusMessageList getInterpolationMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual bool testSendSituationAndParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CAircraftParts &parts) override;
        virtual bool requestElevation(const BlackMisc::Geo::ICoordinateGeodetic &reference, const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void injectWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid) override;

        // ----- functions just logged -------
        virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;
        virtual bool logicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;
        virtual bool logicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual int physicallyRemoveMultipleRemoteAircraft(const BlackMisc::Aviation::CCallsignSet &callsigns) override;

        // functions logged and used
        //! \ingroup swiftdotcommands
        //! <pre>
        //! .drv show   show emulated driver window     BlackSimPlugin::Swift::CSimulatorEmulated
        //! .drv hide   hide emulated driver window     BlackSimPlugin::Swift::CSimulatorEmulated
        //! </pre>
        //! \copydoc BlackCore::ISimulator::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

        //! UI setter
        void setCombinedStatus(bool connected, bool simulating, bool paused);

        //! The emulated simulated simulator
        BlackMisc::Simulation::CSimulatorInfo getEmulatedSimulator() const;

        //! Internal own aircraft
        //! \remark normally used by corresponding BlackSimPlugin::Emulated::CSimulatorEmulatedMonitorDialog
        const BlackMisc::Simulation::CSimulatedAircraft &getInternalOwnAircraft() const { return m_myAircraft; }

        //! Simulator internal change of COM values
        //! \remark normally used by corresponding BlackSimPlugin::Emulated::CSimulatorEmulatedMonitorDialog
        bool changeInternalCom(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Simulator internal change of COM values
        //! \remark normally used by corresponding BlackSimPlugin::Emulated::CSimulatorEmulatedMonitorDialog
        bool changeInternalCom(const BlackMisc::Aviation::CComSystem &comSystem, BlackMisc::Aviation::CComSystem::ComUnit unit);

        //! Simulator internal change of SELCAL
        //! \remark normally used by corresponding BlackSimPlugin::Emulated::CSimulatorEmulatedMonitorDialog
        bool changeInternalSelcal(const BlackMisc::Aviation::CSelcal &selcal);

        //! Simulator internal change of situation
        //! \remark normally used by corresponding BlackSimPlugin::Emulated::CSimulatorEmulatedMonitorDialog
        bool changeInternalSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Simulator internal change of parts
        //! \remark normally used by corresponding BlackSimPlugin::Emulated::CSimulatorEmulatedMonitorDialog
        bool changeInternalParts(const BlackMisc::Aviation::CAircraftParts &parts);

        //! Interpolator fetch time, <=0 stops
        bool setInterpolatorFetchTime(int timeMs);

        //! Is fetching from interpolator
        bool isInterpolatorFetching() const;

        //! Register help
        static void registerHelp();

    signals:
        //! Internal aircraft changed
        void internalAircraftChanged();

    protected:
        virtual bool physicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;
        virtual bool physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual int physicallyRemoveAllRemoteAircraft() override;

        //! \copydoc BlackCore::ISimulator::parseDetails
        virtual bool parseDetails(const BlackMisc::CSimpleCommandParser &parser) override;

    private:
        //! Set object name
        void setObjectName(const BlackMisc::Simulation::CSimulatorInfo &info);

        //! Can append log messages?
        bool canLog() const;

        //! Can display
        bool canDisplay() const;

        //! Close window
        void closeMonitor();

        //! Settings changed
        void onSettingsChanged();

        //! Values from UI
        void onSimulatorStatusChanged();

        //! Connect own signals for monitoring
        void connectOwnSignals();

        //! Fetch data from interpolator
        //! \remarks basically does the same as a real driver, obtains data from the interpolator
        void updateRemoteAircraft();

        //! Request weather
        bool requestWeather();

        bool m_log = false; //!< from settings
        bool m_paused = false;
        bool m_connected = true;
        bool m_simulating = true;
        bool m_timeSyncronized = false;
        int m_countInterpolatedSituations = 0;
        int m_countInterpolatedParts = 0;
        QTimer m_interpolatorFetchTimer; //!< fetch data from interpolator
        BlackMisc::PhysicalQuantities::CTime m_offsetTime;
        BlackMisc::Simulation::CSimulatedAircraft m_myAircraft; //!< represents own aircraft of simulator
        BlackMisc::Simulation::CSimulatedAircraftList m_renderedAircraft; //!< represents remote aircraft in simulator
        QPointer<CSimulatorEmulatedMonitorDialog> m_monitorWidget; //!< parent will be main window, so we need to destroy widget when destroyed
        BlackMisc::CConnectionGuard m_connectionGuard; //!< connected with provider
        BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TSwiftPlugin> m_pluginSettings { this, &CSimulatorEmulated::onSettingsChanged };
        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Simulation::CInterpolatorMultiWrapper> m_interpolators; //!< interpolators per callsign
    };

    //! Listener for swift
    class CSimulatorEmulatedListener : public BlackCore::ISimulatorListener
    {
        Q_OBJECT

    public:
        //! Constructor
        CSimulatorEmulatedListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

    protected:
        //! \copydoc BlackCore::ISimulatorListener::startImpl
        virtual void startImpl() override;

        //! \copydoc BlackCore::ISimulatorListener::stopImpl
        virtual void stopImpl() override;

        //! \copydoc BlackCore::ISimulatorListener::stopImpl
        virtual void checkImpl() override;
    };
} // ns

#endif // guard
