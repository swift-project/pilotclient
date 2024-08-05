// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
using namespace BlackSimPlugin::Common;

namespace BlackSimPlugin::FsCommon
{
    CSimulatorFsCommon::CSimulatorFsCommon(
        const CSimulatorPluginInfo &info,
        IOwnAircraftProvider *ownAircraftProvider,
        IRemoteAircraftProvider *renderedAircraftProvider,
        IClientProvider *clientProvider,
        QObject *parent) : CSimulatorPluginCommon(info, ownAircraftProvider, renderedAircraftProvider, clientProvider, parent)
    {
        CSimulatorFsCommon::registerHelp();
    }

    CSimulatorFsCommon::~CSimulatorFsCommon() {}

    void CSimulatorFsCommon::initSimulatorInternals()
    {
        CSimulatorPluginCommon::initSimulatorInternals();
        m_simulatorInternals.setSimulatorVersion(this->getSimulatorVersion());
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
    }

    bool CSimulatorFsCommon::disconnectFrom()
    {
        // reset flags
        m_simPaused = false;
        const bool r = CSimulatorPluginCommon::disconnectFrom();
        this->emitSimulatorCombinedStatus();
        return r;
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
