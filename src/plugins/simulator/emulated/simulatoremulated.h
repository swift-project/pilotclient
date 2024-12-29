// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_EMULATED_SIMULATOREMULATED_H
#define SWIFT_SIMPLUGIN_EMULATED_SIMULATOREMULATED_H

#include <QMap>
#include <QPointer>
#include <QTimer>

#include "../plugincommon/simulatorplugincommon.h"
#include "simulatoremulatedmonitordialog.h"

#include "misc/aviation/comsystem.h"
#include "misc/connectionguard.h"
#include "misc/pq/time.h"
#include "misc/simulation/interpolation/interpolationlogger.h"
#include "misc/simulation/interpolation/interpolationrenderingsetup.h"
#include "misc/simulation/interpolation/interpolatormulti.h"
#include "misc/simulation/settings/swiftpluginsettings.h"
#include "misc/simulation/simulatorplugininfo.h"

namespace swift::simplugin::emulated
{
    //! swift simulator implementation
    class CSimulatorEmulated : public common::CSimulatorPluginCommon
    {
        Q_OBJECT
        Q_INTERFACES(swift::core::ISimulator)
        Q_INTERFACES(swift::misc::simulation::ISimulationEnvironmentProvider)
        Q_INTERFACES(swift::misc::simulation::IInterpolationSetupProvider)

        friend class CSimulatorEmulatedMonitorDialog; //!< the monitor widget represents the simulator and needs access
                                                      //!< to internals (i.e. private/protected)

    public:
        //! Constructor, parameters as in \sa swift::core::ISimulatorFactory::create
        CSimulatorEmulated(const swift::misc::simulation::CSimulatorPluginInfo &info,
                           swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                           swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                           swift::misc::network::IClientProvider *clientProvider, QObject *parent = nullptr);

        //! Destructor
        virtual ~CSimulatorEmulated() override;

        // functions implemented
        virtual bool connectTo() override;
        virtual bool disconnectFrom() override;
        virtual void unload() override;
        virtual bool isConnected() const override;
        virtual bool isPaused() const override;
        virtual bool isSimulating() const override;
        virtual bool changeRemoteAircraftModel(const swift::misc::simulation::CSimulatedAircraft &aircraft) override;
        virtual bool changeRemoteAircraftEnabled(const swift::misc::simulation::CSimulatedAircraft &aircraft) override;
        virtual bool updateOwnSimulatorCockpit(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                                               const swift::misc::CIdentifier &originator) override;
        virtual bool updateOwnSimulatorSelcal(const swift::misc::aviation::CSelcal &selcal,
                                              const swift::misc::CIdentifier &originator) override;
        virtual void displayStatusMessage(const swift::misc::CStatusMessage &message) const override;
        virtual void displayTextMessage(const swift::misc::network::CTextMessage &message) const override;
        virtual bool isPhysicallyRenderedAircraft(const swift::misc::aviation::CCallsign &callsign) const override;
        virtual swift::misc::aviation::CCallsignSet physicallyRenderedAircraft() const override;
        virtual swift::misc::CStatusMessageList
        getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const override;
        virtual bool testSendSituationAndParts(const swift::misc::aviation::CCallsign &callsign,
                                               const swift::misc::aviation::CAircraftSituation &situation,
                                               const swift::misc::aviation::CAircraftParts &parts) override;
        virtual bool requestElevation(const swift::misc::geo::ICoordinateGeodetic &reference,
                                      const swift::misc::aviation::CCallsign &callsign) override;

        // ----- functions just logged -------
        virtual bool
        logicallyAddRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft) override;
        virtual bool logicallyRemoveRemoteAircraft(const swift::misc::aviation::CCallsign &callsign) override;
        virtual int
        physicallyRemoveMultipleRemoteAircraft(const swift::misc::aviation::CCallsignSet &callsigns) override;

        // functions logged and used
        //! \ingroup swiftdotcommands
        //! <pre>
        //! .drv show   show emulated driver window     swift::simplugin::emulated::CSimulatorEmulated
        //! .drv hide   hide emulated driver window     swift::simplugin::emulated::CSimulatorEmulated
        //! </pre>
        //! \copydoc swift::core::ISimulator::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator) override;

        //! UI setter
        void setCombinedStatus(bool connected, bool simulating, bool paused);

        //! The emulated simulated simulator
        swift::misc::simulation::CSimulatorInfo getEmulatedSimulator() const;

        //! Internal own aircraft
        //! \remark normally used by corresponding swift::simplugin::emulated::CSimulatorEmulatedMonitorDialog
        const swift::misc::simulation::CSimulatedAircraft &getInternalOwnAircraft() const { return m_myAircraft; }

        //! Simulator internal change of COM values
        //! \remark normally used by corresponding swift::simplugin::emulated::CSimulatorEmulatedMonitorDialog
        bool changeInternalCom(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Simulator internal change of COM values
        //! \remark normally used by corresponding swift::simplugin::emulated::CSimulatorEmulatedMonitorDialog
        bool changeInternalCom(const swift::misc::aviation::CComSystem &comSystem,
                               swift::misc::aviation::CComSystem::ComUnit unit);

        //! Simulator internal change of SELCAL
        //! \remark normally used by corresponding swift::simplugin::emulated::CSimulatorEmulatedMonitorDialog
        bool changeInternalSelcal(const swift::misc::aviation::CSelcal &selcal);

        //! Simulator internal change of situation
        //! \remark normally used by corresponding swift::simplugin::emulated::CSimulatorEmulatedMonitorDialog
        bool changeInternalSituation(const swift::misc::aviation::CAircraftSituation &situation);

        //! Simulator internal change of parts
        //! \remark normally used by corresponding swift::simplugin::emulated::CSimulatorEmulatedMonitorDialog
        bool changeInternalParts(const swift::misc::aviation::CAircraftParts &parts);

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
        virtual bool
        physicallyAddRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft) override;
        virtual bool physicallyRemoveRemoteAircraft(const swift::misc::aviation::CCallsign &callsign) override;
        virtual int physicallyRemoveAllRemoteAircraft() override;

        //! \copydoc swift::core::ISimulator::parseDetails
        virtual bool parseDetails(const swift::misc::CSimpleCommandParser &parser) override;

    private:
        //! Set object name
        void setObjectName(const swift::misc::simulation::CSimulatorInfo &info);

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

        bool m_log = false; //!< from settings
        bool m_paused = false;
        bool m_connected = true;
        bool m_simulating = true;
        int m_countInterpolatedSituations = 0;
        int m_countInterpolatedParts = 0;
        QTimer m_interpolatorFetchTimer; //!< fetch data from interpolator
        swift::misc::simulation::CSimulatedAircraft m_myAircraft; //!< represents own aircraft of simulator
        swift::misc::simulation::CSimulatedAircraftList m_renderedAircraft; //!< represents remote aircraft in simulator
        QPointer<CSimulatorEmulatedMonitorDialog>
            m_monitorWidget; //!< parent will be main window, so we need to destroy widget when destroyed
        swift::misc::CConnectionGuard m_connectionGuard; //!< connected with provider
        swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TSwiftPlugin> m_pluginSettings {
            this, &CSimulatorEmulated::onSettingsChanged
        };
        QMap<swift::misc::aviation::CCallsign, swift::misc::simulation::CInterpolatorMultiWrapper>
            m_interpolators; //!< interpolators per callsign
    };

    //! Listener for swift
    class CSimulatorEmulatedListener : public swift::core::ISimulatorListener
    {
        Q_OBJECT

    public:
        //! Constructor
        CSimulatorEmulatedListener(const swift::misc::simulation::CSimulatorPluginInfo &info);

    protected:
        //! \copydoc swift::core::ISimulatorListener::startImpl
        virtual void startImpl() override;

        //! \copydoc swift::core::ISimulatorListener::stopImpl
        virtual void stopImpl() override;

        //! \copydoc swift::core::ISimulatorListener::stopImpl
        virtual void checkImpl() override;
    };
} // namespace swift::simplugin::emulated

#endif // SWIFT_SIMPLUGIN_EMULATED_SIMULATOREMULATED_H
