/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SWIFT_SIMULATORSWIFT_H
#define BLACKSIMPLUGIN_SWIFT_SIMULATORSWIFT_H

#include "blackcore/simulatorcommon.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/settings/swiftpluginsettings.h"
#include "blackmisc/pq/time.h"
#include "simulatorswiftmonitordialog.h"

#include <QScopedPointer>

namespace BlackSimPlugin
{
    namespace Swift
    {
        //! swift simulator implementation
        class CSimulatorSwift : public BlackCore::CSimulatorCommon
        {
            Q_OBJECT
            friend class CSimulatorSwiftMonitorDialog; //!< the monitor widget represents the simulator and needs access to internals (i.e. private/protected)

        public:
            //! Constructor, parameters as in \sa BlackCore::ISimulatorFactory::create
            CSimulatorSwift(
                const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
                QObject *parent = nullptr);

            //! \copydoc BlackCore::CSimulatorCommon::getSimulatorInfo
            virtual BlackMisc::Simulation::CSimulatorInfo getSimulatorInfo() const override;

            // functions implemented
            virtual bool isTimeSynchronized() const override;
            virtual bool connectTo() override;
            virtual bool disconnectFrom() override;
            virtual bool changeRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft) override;
            virtual bool changeRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft) override;
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator) override;
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;
            virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const override;
            virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;
            virtual bool isPhysicallyRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::Aviation::CCallsignSet physicallyRenderedAircraft() const override;
            virtual bool setInterpolatorMode(BlackMisc::Simulation::CInterpolatorMulti::Mode mode, const BlackMisc::Aviation::CCallsign &callsign) override;

            // functions just logged
            virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;
            virtual bool logicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;
            virtual bool logicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual int physicallyRemoveMultipleRemoteAircraft(const BlackMisc::Aviation::CCallsignSet &callsigns) override;

            // functions logged and used
            //! \addtogroup swiftdotcommands
            //! @{
            //! <pre>
            //! .drv show   show swift driver window     BlackSimPlugin::Swift::CSimulatorSwift
            //! .drv hide   hide swift driver window     BlackSimPlugin::Swift::CSimulatorSwift
            //! </pre>
            //! @}
            //! \copydoc BlackCore::ISimulator::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

            //! Register help
            static void registerHelp();

            //! UI setter
            void setCombinedStatus(bool connected, bool simulating, bool paused);

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
            //! Can append log messages?
            bool canLog() const;

            //! Close window
            void closeMonitor();

            //! Set own aircraft position
            void setOwnAircraftPosition(const QString &wgsLatitude, const QString &wgsLongitude, const BlackMisc::Aviation::CAltitude &altitude);

            //! Settings changed
            void onSettingsChanged();

            //! Values from UI
            void onSimulatorStatusChanged();

            bool m_log = false; //!< from settings
            bool m_paused = false;
            bool m_connected = true;
            bool m_simulating = true;
            bool m_timeSyncronized = false;
            BlackMisc::PhysicalQuantities::CTime m_offsetTime;
            BlackMisc::Simulation::CSimulatedAircraftList m_renderedAircraft;

            QScopedPointer<CSimulatorSwiftMonitorDialog> m_monitorWidget; //!< parent will be main window, so we need to destroy widget when destroyed
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TSwiftPlugin> m_settings { this, &CSimulatorSwift::onSettingsChanged };
        };

        //! Listener for swift
        class CSimulatorSwiftListener : public BlackCore::ISimulatorListener
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorSwiftListener(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        protected:
            //! \copydoc BlackCore::ISimulatorListener::startImpl
            virtual void startImpl() override;

            //! \copydoc BlackCore::ISimulatorListener::stopImpl
            virtual void stopImpl() override;
        };
    } // ns
} // ns

#endif // guard
