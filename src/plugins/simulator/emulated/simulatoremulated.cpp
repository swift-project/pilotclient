/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatoremulated.h"
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
    namespace Emulated
    {
        CSimulatorEmulated::CSimulatorEmulated(const CSimulatorPluginInfo &info,
                                         IOwnAircraftProvider *ownAircraftProvider,
                                         IRemoteAircraftProvider *remoteAircraftProvider,
                                         IWeatherGridProvider *weatherGridProvider,
                                         QObject *parent) :
            CSimulatorCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, parent)
        {
            Q_ASSERT_X(sApp && sApp->getIContextSimulator(), Q_FUNC_INFO, "Need context");

            CSimulatorEmulated::registerHelp();
            m_monitorWidget.reset(new CSimulatorEmulatedMonitorDialog(this, sGui->mainApplicationWindow()));
            connect(qApp, &QApplication::aboutToQuit, this, &CSimulatorEmulated::closeMonitor);
            this->onSettingsChanged();
        }

        CSimulatorInfo CSimulatorEmulated::getSimulatorInfo() const
        {
            const CSwiftPluginSettings s = m_settings.get();
            return s.getEmulatedSimulator();
        }

        bool CSimulatorEmulated::isTimeSynchronized() const
        {
            return m_timeSyncronized;
        }

        bool CSimulatorEmulated::connectTo()
        {
            QTimer::singleShot(1000, this, [ = ]
            {
                this->emitSimulatorCombinedStatus();
                m_monitorWidget->show();
            });

            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return true;
        }

        bool CSimulatorEmulated::disconnectFrom()
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            m_renderedAircraft.clear();
            return true;
        }

        bool CSimulatorEmulated::logicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, remoteAircraft.toQString());
            return CSimulatorCommon::logicallyAddRemoteAircraft(remoteAircraft);
        }

        bool CSimulatorEmulated::logicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, callsign.toQString());
            return CSimulatorCommon::logicallyRemoveRemoteAircraft(callsign);
        }

        int CSimulatorEmulated::physicallyRemoveMultipleRemoteAircraft(const CCallsignSet &callsigns)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, callsigns.toQString());
            return CSimulatorCommon::physicallyRemoveMultipleRemoteAircraft(callsigns);
        }

        bool CSimulatorEmulated::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, aircraft.toQString());
            return true;
        }

        bool CSimulatorEmulated::changeRemoteAircraftEnabled(const CSimulatedAircraft &aircraft)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, aircraft.toQString());
            const int c = m_renderedAircraft.setEnabled(aircraft.getCallsign(), aircraft.isEnabled(), true);
            emit this->aircraftRenderingChanged(m_renderedAircraft.findFirstByCallsign(aircraft.getCallsign(), aircraft));
            return c > 0;
        }

        bool CSimulatorEmulated::updateOwnSimulatorCockpit(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, aircraft.toQString(), originator.toQString());
            return true;
        }

        void CSimulatorEmulated::displayStatusMessage(const CStatusMessage &message) const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, message.toQString());
        }

        void CSimulatorEmulated::displayTextMessage(const CTextMessage &message) const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, message.toQString());
        }

        bool CSimulatorEmulated::setTimeSynchronization(bool enable, const CTime &offset)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, boolToTrueFalse(enable), offset.toQString());
            m_timeSyncronized = enable;
            m_offsetTime = offset;
            return enable;
        }

        CTime CSimulatorEmulated::getTimeSynchronizationOffset() const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return m_offsetTime;
        }

        bool CSimulatorEmulated::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, callsign.toQString());
            return m_renderedAircraft.containsCallsign(callsign);
        }

        CCallsignSet CSimulatorEmulated::physicallyRenderedAircraft() const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return m_renderedAircraft.getCallsigns();
        }

        void CSimulatorEmulated::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, aircraftToHighlight.toQString(), boolToTrueFalse(enableHighlight), displayTime.toQString());
            CSimulatorCommon::highlightAircraft(aircraftToHighlight, enableHighlight, displayTime);
        }

        bool CSimulatorEmulated::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, commandLine, originator.toQString());
            return CSimulatorCommon::parseCommandLine(commandLine, originator);
        }

        void CSimulatorEmulated::registerHelp()
        {
            if (BlackMisc::CSimpleCommandParser::registered("BlackSimPlugin::Swift::CSimulatorEmulated")) { return; }
            BlackMisc::CSimpleCommandParser::registerCommand({".drv", "alias: .driver .plugin"});
            BlackMisc::CSimpleCommandParser::registerCommand({".drv show", "show swift driver window"});
            BlackMisc::CSimpleCommandParser::registerCommand({".drv hide", "hide swift driver window"});
        }

        void CSimulatorEmulated::setCombinedStatus(bool connected, bool simulating, bool paused)
        {
            m_connected = connected;
            m_simulating = simulating;
            m_paused = paused;
            this->emitSimulatorCombinedStatus();
        }

        bool CSimulatorEmulated::isConnected() const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return m_connected;
        }

        bool CSimulatorEmulated::isPaused() const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return m_paused;
        }

        bool CSimulatorEmulated::isSimulating() const
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return m_simulating;
        }

        bool CSimulatorEmulated::physicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, remoteAircraft.toQString());
            CSimulatedAircraft aircraft(remoteAircraft);
            aircraft.setRendered(true);
            m_renderedAircraft.push_back(aircraft);
            emit this->aircraftRenderingChanged(aircraft);
            return true;
        }

        bool CSimulatorEmulated::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, callsign.toQString());
            const int c = m_renderedAircraft.removeByCallsign(callsign);
            return c > 0;
        }

        bool CSimulatorEmulated::setInterpolatorMode(CInterpolatorMulti::Mode mode, const CCallsign &callsign)
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO, CInterpolatorMulti::modeToString(mode), callsign.toQString());
            return false;
        }

        int CSimulatorEmulated::physicallyRemoveAllRemoteAircraft()
        {
            if (canLog()) m_monitorWidget->appendFunctionCall(Q_FUNC_INFO);
            return CSimulatorCommon::physicallyRemoveAllRemoteAircraft();
        }

        bool CSimulatorEmulated::parseDetails(const CSimpleCommandParser &parser)
        {
            if (m_monitorWidget && parser.isKnownCommand())
            {
                if (parser.matchesPart(1, "show")) { this->m_monitorWidget->show(); return true; }
                if (parser.matchesPart(1, "hide")) { this->m_monitorWidget->hide(); return true; }
            }
            return false;
        }

        bool CSimulatorEmulated::canLog() const
        {
            return sApp && !sApp->isShuttingDown() && m_log && m_monitorWidget;
        }

        void CSimulatorEmulated::closeMonitor()
        {
            if (m_monitorWidget)
            {
                m_monitorWidget->close();
            }
        }

        void CSimulatorEmulated::setOwnAircraftPosition(const QString &wgsLatitude, const QString &wgsLongitude, const CAltitude &altitude)
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

        void CSimulatorEmulated::onSettingsChanged()
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

        CSimulatorEmulatedListener::CSimulatorEmulatedListener(const CSimulatorPluginInfo &info)
            : ISimulatorListener(info)
        { }

        void CSimulatorEmulatedListener::startImpl()
        {
            if (this->isShuttingDown()) { return; }
            QTimer::singleShot(2000, this, [ = ]
            {
                Q_ASSERT_X(this->getPluginInfo().isValid(), Q_FUNC_INFO, "Invalid plugin");
                emit this->simulatorStarted(this->getPluginInfo());
            });
        }

        void CSimulatorEmulatedListener::stopImpl()
        { }
    } // ns
} // ns
