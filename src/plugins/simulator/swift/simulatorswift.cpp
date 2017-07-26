/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorswift.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/simulation/simulatorplugininfo.h"

#include <QApplication>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Weather;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackSimPlugin
{
    namespace Swift
    {
        CSimulatorSwift::CSimulatorSwift(const CSimulatorPluginInfo &info,
                                         IOwnAircraftProvider *ownAircraftProvider,
                                         IRemoteAircraftProvider *remoteAircraftProvider,
                                         IWeatherGridProvider *weatherGridProvider,
                                         QObject *parent) :
            CSimulatorCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, parent)
        {
            Q_ASSERT_X(sApp && sApp->getIContextSimulator(), Q_FUNC_INFO, "Need context");

            CSimulatorSwift::registerHelp();
            m_monitorWidget.reset(new CSimulatorSwiftMonitorDialog(this, sGui->mainApplicationWindow()));
            connect(qApp, &QApplication::aboutToQuit, this, &CSimulatorSwift::closeMonitor);
            this->onSettingsChanged();
        }

        CSimulatorInfo CSimulatorSwift::getSimulatorInfo() const
        {
            const CSwiftPluginSettings s = m_settings.get();
            return s.getEmulatedSimulator();
        }

        bool CSimulatorSwift::isTimeSynchronized() const
        {
            return m_timeSyncronized;
        }

        bool CSimulatorSwift::connectTo()
        {
            QTimer::singleShot(1000, this, [ = ]
            {
                this->emitSimulatorCombinedStatus();
                m_monitorWidget->show();
            });

            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return true;
        }

        bool CSimulatorSwift::disconnectFrom()
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            m_renderedAircraft.clear();
            return true;
        }

        bool CSimulatorSwift::logicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, remoteAircraft.toQString());
            return CSimulatorCommon::logicallyAddRemoteAircraft(remoteAircraft);
        }

        bool CSimulatorSwift::logicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, callsign.toQString());
            return CSimulatorCommon::logicallyRemoveRemoteAircraft(callsign);
        }

        int CSimulatorSwift::physicallyRemoveMultipleRemoteAircraft(const CCallsignSet &callsigns)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, callsigns.toQString());
            return CSimulatorCommon::physicallyRemoveMultipleRemoteAircraft(callsigns);
        }

        bool CSimulatorSwift::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, aircraft.toQString());
            return true;
        }

        bool CSimulatorSwift::changeRemoteAircraftEnabled(const CSimulatedAircraft &aircraft)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, aircraft.toQString());
            const int c = m_renderedAircraft.setEnabled(aircraft.getCallsign(), aircraft.isEnabled(), true);
            emit this->aircraftRenderingChanged(m_renderedAircraft.findFirstByCallsign(aircraft.getCallsign(), aircraft));
            return c > 0;
        }

        bool CSimulatorSwift::updateOwnSimulatorCockpit(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, aircraft.toQString(), originator.toQString());
            return true;
        }

        void CSimulatorSwift::displayStatusMessage(const CStatusMessage &message) const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, message.toQString());
        }

        void CSimulatorSwift::displayTextMessage(const CTextMessage &message) const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, message.toQString());
        }

        bool CSimulatorSwift::setTimeSynchronization(bool enable, const CTime &offset)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, boolToTrueFalse(enable), offset.toQString());
            m_timeSyncronized = enable;
            m_offsetTime = offset;
            return enable;
        }

        CTime CSimulatorSwift::getTimeSynchronizationOffset() const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return m_offsetTime;
        }

        bool CSimulatorSwift::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, callsign.toQString());
            return m_renderedAircraft.containsCallsign(callsign);
        }

        CCallsignSet CSimulatorSwift::physicallyRenderedAircraft() const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return m_renderedAircraft.getCallsigns();
        }

        void CSimulatorSwift::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, aircraftToHighlight.toQString(), boolToTrueFalse(enableHighlight), displayTime.toQString());
            CSimulatorCommon::highlightAircraft(aircraftToHighlight, enableHighlight, displayTime);
        }

        bool CSimulatorSwift::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, commandLine, originator.toQString());
            return CSimulatorCommon::parseCommandLine(commandLine, originator);
        }

        void CSimulatorSwift::registerHelp()
        {
            if (BlackMisc::CSimpleCommandParser::registered("BlackSimPlugin::Swift::CSimulatorSwift")) { return; }
            BlackMisc::CSimpleCommandParser::registerCommand({".drv", "alias: .driver .plugin"});
            BlackMisc::CSimpleCommandParser::registerCommand({".drv show", "show swift driver window"});
            BlackMisc::CSimpleCommandParser::registerCommand({".drv hide", "hide swift driver window"});
        }

        void CSimulatorSwift::setCombinedStatus(bool connected, bool simulating, bool paused)
        {
            m_connected = connected;
            m_simulating = simulating;
            m_paused = paused;
            this->emitSimulatorCombinedStatus();
        }

        bool CSimulatorSwift::isConnected() const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return m_connected;
        }

        bool CSimulatorSwift::isPaused() const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return m_paused;
        }

        bool CSimulatorSwift::isSimulating() const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return m_simulating;
        }

        bool CSimulatorSwift::physicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, remoteAircraft.toQString());
            CSimulatedAircraft aircraft(remoteAircraft);
            aircraft.setRendered(true);
            m_renderedAircraft.push_back(aircraft);
            emit this->aircraftRenderingChanged(aircraft);
            return true;
        }

        bool CSimulatorSwift::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, callsign.toQString());
            const int c = m_renderedAircraft.removeByCallsign(callsign);
            return c > 0;
        }

        bool CSimulatorSwift::setInterpolatorMode(CInterpolatorMulti::Mode mode, const CCallsign &callsign)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, CInterpolatorMulti::modeToString(mode), callsign.toQString());
            return false;
        }

        int CSimulatorSwift::physicallyRemoveAllRemoteAircraft()
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return CSimulatorCommon::physicallyRemoveAllRemoteAircraft();
        }

        bool CSimulatorSwift::parseDetails(const CSimpleCommandParser &parser)
        {
            if (m_monitorWidget && parser.isKnownCommand())
            {
                if (parser.matchesPart(1, "show")) { this->m_monitorWidget->show(); return true; }
                if (parser.matchesPart(1, "hide")) { this->m_monitorWidget->hide(); return true; }
            }
            return false;
        }

        bool CSimulatorSwift::canLog() const
        {
            return sApp && !sApp->isShuttingDown() && m_log && m_monitorWidget;
        }

        void CSimulatorSwift::closeMonitor()
        {
            if (m_monitorWidget)
            {
                m_monitorWidget->close();
            }
        }

        void CSimulatorSwift::setOwnAircraftPosition(const QString &wgsLatitude, const QString &wgsLongitude, const CAltitude &altitude)
        {
            const CCoordinateGeodetic coordinate(
                CLatitude::fromWgs84(wgsLatitude),
                CLongitude::fromWgs84(wgsLongitude),
                CAltitude(0, CLengthUnit::m()));

            CAircraftSituation s = this->getOwnAircraftSituation();
            s.setPosition(coordinate);
            s.setAltitude(altitude);

            this->updateOwnSituation(s);
        }

        void CSimulatorSwift::onSettingsChanged()
        {
            const CSwiftPluginSettings settings(m_settings.get());
            m_log = settings.isLoggingFunctionCalls();

            const CSimulatorPluginInfoList plugins = sApp->getIContextSimulator()->getAvailableSimulatorPlugins();
            const CSimulatorPluginInfo plugin = plugins.findBySimulator(settings.getEmulatedSimulator());
            if (plugin.isValid())
            {
                // ? restart driver, disconnect/reconnect
                this->setNewPluginInfo(plugin, settings.getDefaultModel());
            }
            else
            {
                CLogMessage(this).validationError("No valid plugin for %1") << settings.getEmulatedSimulator().getSimulator();
            }

            // update provider
            this->updateOwnModel(settings.getOwnModel());
        }

        CSimulatorSwiftListener::CSimulatorSwiftListener(const CSimulatorPluginInfo &info)
            : ISimulatorListener(info)
        { }

        void CSimulatorSwiftListener::startImpl()
        {
            if (this->isShuttingDown()) { return; }
            QTimer::singleShot(2000, this, [ = ]
            {
                Q_ASSERT_X(this->getPluginInfo().isValid(), Q_FUNC_INFO, "Invalid plugin");
                emit this->simulatorStarted(this->getPluginInfo());
            });
        }

        void CSimulatorSwiftListener::stopImpl()
        { }
    } // ns
} // ns
