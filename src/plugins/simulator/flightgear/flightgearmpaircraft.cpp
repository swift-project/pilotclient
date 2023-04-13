/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "flightgearmpaircraft.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/interpolatormulti.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackSimPlugin::Flightgear
{
    CFlightgearMPAircraft::CFlightgearMPAircraft()
    {}

    CFlightgearMPAircraft::CFlightgearMPAircraft(
        const CSimulatedAircraft &aircraft, ISimulator *simulator, CInterpolationLogger *logger) : m_aircraft(aircraft),
                                                                                                   m_interpolator(QSharedPointer<CInterpolatorMulti>::create(aircraft.getCallsign(), simulator, simulator, simulator->getRemoteAircraftProvider(), logger))
    {
        m_interpolator->attachLogger(logger);
        m_interpolator->initCorrespondingModel(aircraft.getModel());
    }

    void CFlightgearMPAircraft::setSimulatedAircraft(const CSimulatedAircraft &simulatedAircraft)
    {
        m_aircraft = simulatedAircraft;
    }

    QString CFlightgearMPAircraft::getInterpolatorInfo(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
    {
        Q_ASSERT(m_interpolator);
        return m_interpolator->getInterpolatorInfo(mode);
    }

    void CFlightgearMPAircraft::attachInterpolatorLogger(CInterpolationLogger *logger) const
    {
        Q_ASSERT(m_interpolator);
        m_interpolator->attachLogger(logger);
    }

    CInterpolationResult CFlightgearMPAircraft::getInterpolation(qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber) const
    {
        Q_ASSERT(m_interpolator);
        return m_interpolator->getInterpolation(currentTimeSinceEpoc, setup, aircraftNumber);
    }

    CStatusMessageList CFlightgearMPAircraft::getInterpolationMessages(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
    {
        return this->getInterpolator() ? this->getInterpolator()->getInterpolationMessages(mode) : CStatusMessageList();
    }

    CCallsignSet CFlightgearMPAircraftObjects::getAllCallsigns() const
    {
        return CCallsignSet(this->keys());
    }

    QStringList CFlightgearMPAircraftObjects::getAllCallsignStrings(bool sorted) const
    {
        return this->getAllCallsigns().getCallsignStrings(sorted);
    }
} // namespace
