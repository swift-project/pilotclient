/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
using namespace BlackMisc::Weather;
using namespace BlackCore;
using namespace BlackSimPlugin::Common;

namespace BlackSimPlugin::FsCommon
{
    CSimulatorFsCommon::CSimulatorFsCommon(
        const CSimulatorPluginInfo &info,
        IOwnAircraftProvider *ownAircraftProvider,
        IRemoteAircraftProvider *renderedAircraftProvider,
        IWeatherGridProvider *weatherGridProvider,
        IClientProvider *clientProvider,
        QObject *parent) : CSimulatorPluginCommon(info, ownAircraftProvider, renderedAircraftProvider, weatherGridProvider, clientProvider, parent),
                           m_fsuipc(new CFsuipc(this))
    {
        CSimulatorFsCommon::registerHelp();
    }

    CSimulatorFsCommon::~CSimulatorFsCommon() {}

    void CSimulatorFsCommon::initSimulatorInternals()
    {
        CSimulatorPluginCommon::initSimulatorInternals();
        m_simulatorInternals.setSimulatorVersion(this->getSimulatorVersion());
        m_simulatorInternals.setValue("fscommon/fsuipc", boolToOnOff(m_useFsuipc));
        if (m_fsuipc)
        {
            const QString v(m_fsuipc->getVersion());
            if (!v.isEmpty()) { m_simulatorInternals.setValue("fscommon/fsuipcversion", v); }
            m_simulatorInternals.setValue("fscommon/fsuipcopen", boolToYesNo(m_fsuipc->isOpened()));
        }
    }

    bool CSimulatorFsCommon::parseDetails(const CSimpleCommandParser &parser)
    {
        // .driver fsuipc on|off
        if (parser.matchesPart(1, "fsuipc") && parser.hasPart(2))
        {
            const bool on = parser.toBool(2);
            const bool s = this->useFsuipc(on);
            CLogMessage(this, CLogCategories::cmdLine()).info(u"FSUIPC is '%1'") << boolToOnOff(s);
            return s;
        }
        return CSimulatorPluginCommon::parseDetails(parser);
    }

    void CSimulatorFsCommon::reset()
    {
        m_ownAircraftUpdateCycles = 0;
        m_skipCockpitUpdateCycles = 0;

        ISimulator::reset();
    }

    void CSimulatorFsCommon::registerHelp()
    {
        if (CSimpleCommandParser::registered("BlackSimPlugin::FsCommon::CSimulatorFsCommon")) { return; }
        CSimpleCommandParser::registerCommand({ ".drv", "alias: .driver .plugin" });
        CSimpleCommandParser::registerCommand({ ".drv fsuipc on|off", "FSUIPC on|off if applicable" });
    }

    bool CSimulatorFsCommon::disconnectFrom()
    {
        if (m_fsuipc) { m_fsuipc->close(); }

        // reset flags
        m_simPaused = false;
        const bool r = CSimulatorPluginCommon::disconnectFrom();
        this->emitSimulatorCombinedStatus();
        return r;
    }

    bool CSimulatorFsCommon::isFsuipcOpened() const
    {
        return m_fsuipc && m_fsuipc->isOpened();
    }

    bool CSimulatorFsCommon::useFsuipc(bool on)
    {
        if (!m_fsuipc) { return false; } // no FSUIPC available
        if (m_useFsuipc == on) { return m_useFsuipc; } // nothing changed
        m_useFsuipc = on;
        if (on)
        {
            m_useFsuipc = m_fsuipc->open();
        }
        else
        {
            m_fsuipc->close();
        }

        this->initSimulatorInternals(); // update internals
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

    CAirportList CSimulatorFsCommon::getAirportsInRange(bool recalculateDistance) const
    {
        if (!m_airportsInRangeFromSimulator.isEmpty())
        {
            CAirportList airports = m_airportsInRangeFromSimulator;
            if (recalculateDistance) { airports.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftPosition()); }
            return airports;
        }
        return ISimulator::getAirportsInRange(recalculateDistance);
    }

    void CSimulatorFsCommon::onSwiftDbAirportsRead()
    {
        const CAirportList webServiceAirports = this->getWebServiceAirports();
        if (!webServiceAirports.isEmpty())
        {
            m_airportsInRangeFromSimulator.updateMissingParts(webServiceAirports);
        }
        ISimulator::onSwiftDbAirportsRead();
    }
} // namespace
