/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorfscommon.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/stringutils.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;

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
            m_fsuipc(new CFsuipc())
        { }

        CSimulatorFsCommon::~CSimulatorFsCommon() { }

        void CSimulatorFsCommon::initInternalsObject()
        {
            CSimulatorInternals s;
            s.setSimulatorName(this->m_simulatorName);
            s.setSimulatorVersion(this->m_simulatorVersion);
            s.setValue("fscommon/fsuipc", boolToOnOff(m_useFsuipc));
            if (m_fsuipc)
            {
                const QString v(m_fsuipc->getVersion());
                if (!v.isEmpty()) { s.setValue("fscommon/fsuipcversion", v); }
                s.setValue("fscommon/fsuipcconnect", boolToYesNo(m_fsuipc->isConnected()));
            }
            this->m_simulatorInternals = s;
        }

        bool CSimulatorFsCommon::disconnectFrom()
        {
            if (this->m_fsuipc) { this->m_fsuipc->disconnect(); }

            // reset flags
            m_simPaused = false;
            emitSimulatorCombinedStatus();
            return true;
        }

        bool CSimulatorFsCommon::isFsuipcConnected() const
        {
            return !m_fsuipc.isNull() && m_fsuipc->isConnected();
        }

        CTime CSimulatorFsCommon::getTimeSynchronizationOffset() const
        {
            return m_syncTimeOffset;
        }

        bool CSimulatorFsCommon::setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset)
        {
            this->m_simTimeSynced = enable;
            this->m_syncTimeOffset = offset;
            return true;
        }

        CAirportList CSimulatorFsCommon::getAirportsInRange() const
        {
            return m_airportsInRange;
        }

        bool CSimulatorFsCommon::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft)
        {
            // remove upfront, and then enable / disable again
            const auto callsign = aircraft.getCallsign();
            if (!isPhysicallyRenderedAircraft(callsign)) { return false; }
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
    } // namespace
} // namespace
