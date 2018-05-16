/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATED_H
#define BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATED_H

#include "../plugincommon/simulatorplugincommon.h"
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
#include <QScopedPointer>

namespace BlackSimPlugin
{
    namespace Emulated
    {
        //! swift simulator implementation
        class CSimulatorEmulated : public Common::CSimulatorPluginCommon
        {
            Q_OBJECT
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

            // functions implemented
            virtual bool isTimeSynchronized() const override;
            virtual bool connectTo() override;
            virtual bool disconnectFrom() override;
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

            // functions just logged
            virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;
            virtual bool logicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;
            virtual bool logicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual int physicallyRemoveMultipleRemoteAircraft(const BlackMisc::Aviation::CCallsignSet &callsigns) override;

            // functions logged and used
            //! \addtogroup swiftdotcommands
            //! @{
            //! <pre>
            //! .drv show   show emulated driver window     BlackSimPlugin::Swift::CSimulatorEmulated
            //! .drv hide   hide emulated driver window     BlackSimPlugin::Swift::CSimulatorEmulated
            //! </pre>
            //! @}
            //! \copydoc BlackCore::ISimulator::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

            //! UI setter
            void setCombinedStatus(bool connected, bool simulating, bool paused);

            //! Internal own aircraft
            //! \remark normally used by corresponding BlackSimPlugin::Emulated::CSimulatorEmulatedMonitorDialog
            const BlackMisc::Simulation::CSimulatedAircraft &getInternalOwnAircraft() const { return m_myAircraft; }

            //! Simulator internal change of COM values
            //! \remark normally used by corresponding BlackSimPlugin::Emulated::CSimulatorEmulatedMonitorDialog
            bool changeInternalCom(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

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
            virtual bool isConnected() const override;
            virtual bool isPaused() const override;
            virtual bool isSimulating() const override;
            virtual bool physicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;
            virtual bool physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

            // just logged
            virtual int physicallyRemoveAllRemoteAircraft() override;

            //! \copydoc BlackCore::CSimulatorCommon::parseDetails
            virtual bool parseDetails(const BlackMisc::CSimpleCommandParser &parser) override;

        private:
            //! Set object name
            void setObjectName(const BlackMisc::Simulation::CSimulatorInfo &info);

            //! Can append log messages?
            bool canLog() const;

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
            void fetchFromInterpolator();

            bool m_log = false; //!< from settings
            bool m_paused = false;
            bool m_connected = true;
            bool m_simulating = true;
            bool m_timeSyncronized = false;
            int m_countInterpolatedSituations = 0;
            int m_countInterpolatedParts = 0;
            QTimer m_interpolatorFetchTimer; //!< fetch data from interpolator
            BlackMisc::PhysicalQuantities::CTime m_offsetTime;
            BlackMisc::Simulation::CSimulatedAircraft m_myAircraft;           //!< represents own aircraft of simulator
            BlackMisc::Simulation::CSimulatedAircraftList m_renderedAircraft; //!< represents remote aircraft in simulator
            QScopedPointer<CSimulatorEmulatedMonitorDialog> m_monitorWidget;  //!< parent will be main window, so we need to destroy widget when destroyed
            BlackMisc::CConnectionGuard m_connectionGuard;                    //!< connected with provider
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TSwiftPlugin> m_settings { this, &CSimulatorEmulated::onSettingsChanged };
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
        };
    } // ns
} // ns

#endif // guard
