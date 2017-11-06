/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorfscommon.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/stringutils.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackCore;

namespace BlackSimPlugin
{
    namespace FsCommon
    {
        CSimulatorFsCommon::CSimulatorFsCommon(
            const CSimulatorPluginInfo &info,
            IOwnAircraftProvider *ownAircraftProvider,
            IRemoteAircraftProvider *renderedAircraftProvider,
            Weather::IWeatherGridProvider *weatherGridProvider,
            QObject *parent) :
            CSimulatorCommon(info, ownAircraftProvider, renderedAircraftProvider, weatherGridProvider, parent),
            m_fsuipc(std::make_unique<CFsuipc>(this))
        {
            CSimulatorFsCommon::registerHelp();
        }

        CSimulatorFsCommon::~CSimulatorFsCommon() { }

        void CSimulatorFsCommon::initSimulatorInternals()
        {
            CSimulatorInternals s;
            s.setSimulatorName(m_simulatorName);
            s.setSimulatorVersion(m_simulatorVersion);
            s.setValue("fscommon/fsuipc", boolToOnOff(m_useFsuipc));
            if (m_fsuipc)
            {
                const QString v(m_fsuipc->getVersion());
                if (!v.isEmpty()) { s.setValue("fscommon/fsuipcversion", v); }
                s.setValue("fscommon/fsuipcconnect", boolToYesNo(m_fsuipc->isConnected()));
            }
            m_simulatorInternals = s;
        }

        bool CSimulatorFsCommon::parseDetails(const CSimpleCommandParser &parser)
        {
            // .driver fsuipc on|off
            if (parser.matchesPart(1, "fsuipc") && parser.hasPart(2))
            {
                const bool on = parser.toBool(2);
                const bool s = this->useFsuipc(on);
                CLogMessage(this, CLogCategory::cmdLine()).info("FSUIPC is '%1'") << boolToOnOff(s);
                return s;
            }
            return false;
        }

        void CSimulatorFsCommon::registerHelp()
        {
            if (CSimpleCommandParser::registered("BlackSimPlugin::FsCommon::CSimulatorFsCommon")) { return; }
            CSimpleCommandParser::registerCommand({".drv", "alias: .driver .plugin"});
            CSimpleCommandParser::registerCommand({".drv fsuipc on|off", "FSUIPC on|off if applicable"});
        }

        bool CSimulatorFsCommon::disconnectFrom()
        {
            if (m_fsuipc) { m_fsuipc->disconnect(); }

            // reset flags
            m_simPaused = false;
            this->emitSimulatorCombinedStatus();
            return true;
        }

        bool CSimulatorFsCommon::isFsuipcConnected() const
        {
            return m_fsuipc && m_fsuipc->isConnected();
        }

        bool CSimulatorFsCommon::useFsuipc(bool on)
        {
            if (!m_fsuipc) { return false; } // no FSUIPC available
            m_useFsuipc = on;
            if (on)
            {
                m_useFsuipc = m_fsuipc->connect();
            }
            else
            {
                m_fsuipc->disconnect();
            }
            return m_useFsuipc;
        }

        CTime CSimulatorFsCommon::getTimeSynchronizationOffset() const
        {
            return m_syncTimeOffset;
        }

        bool CSimulatorFsCommon::setTimeSynchronization(bool enable, const PhysicalQuantities::CTime &offset)
        {
            m_simTimeSynced = enable;
            m_syncTimeOffset = offset;
            return true;
        }

        CAirportList CSimulatorFsCommon::getAirportsInRange() const
        {
            if (!m_airportsInRangeFromSimulator.isEmpty())
            {
                return m_airportsInRangeFromSimulator;
            }
            return CSimulatorCommon::getAirportsInRange();
        }

        bool CSimulatorFsCommon::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft)
        {
            // remove upfront, and then enable / disable again
            const CCallsign callsign = aircraft.getCallsign();
            if (!this->isPhysicallyRenderedAircraft(callsign)) { return false; }
            this->physicallyRemoveRemoteAircraft(callsign);
            return this->changeRemoteAircraftEnabled(aircraft);
        }

        bool CSimulatorFsCommon::changeRemoteAircraftEnabled(const CSimulatedAircraft &aircraft)
        {
            if (aircraft.isEnabled())
            {
                this->physicallyAddRemoteAircraft(aircraft);
            }
            else
            {
                this->physicallyRemoveRemoteAircraft(aircraft.getCallsign());
            }
            return true;
        }

        void CSimulatorFsCommon::onSwiftDbAirportsRead()
        {
            const CAirportList webServiceAirports = this->getWebServiceAirports();
            if (!webServiceAirports.isEmpty())
            {
                m_airportsInRangeFromSimulator.updateMissingParts(webServiceAirports);
            }
            CSimulatorCommon::onSwiftDbAirportsRead();
        }
    } // namespace
} // namespace
