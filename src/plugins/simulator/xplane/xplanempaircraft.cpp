// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "xplanempaircraft.h"

#include "core/simulator.h"
#include "misc/simulation/interpolation/interpolatormulti.h"

using namespace swift::core;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;

namespace swift::simplugin::xplane
{
    CXPlaneMPAircraft::CXPlaneMPAircraft(const CSimulatedAircraft &aircraft, ISimulator *simulator,
                                         CInterpolationLogger *logger)
        : m_aircraft(aircraft),
          m_interpolator(QSharedPointer<CInterpolatorMulti>::create(aircraft.getCallsign(), simulator, simulator,
                                                                    simulator->getRemoteAircraftProvider(), logger))
    {
        m_interpolator->attachLogger(logger);
        m_interpolator->initCorrespondingModel(aircraft.getModel());
    }

    void CXPlaneMPAircraft::setSimulatedAircraft(const CSimulatedAircraft &simulatedAircraft)
    {
        m_aircraft = simulatedAircraft;
    }

    QString CXPlaneMPAircraft::getInterpolatorInfo(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
    {
        Q_ASSERT(m_interpolator);
        return m_interpolator->getInterpolatorInfo(mode);
    }

    void CXPlaneMPAircraft::attachInterpolatorLogger(CInterpolationLogger *logger) const
    {
        Q_ASSERT(m_interpolator);
        m_interpolator->attachLogger(logger);
    }

    CInterpolationResult CXPlaneMPAircraft::getInterpolation(qint64 currentTimeSinceEpoch,
                                                             const CInterpolationAndRenderingSetupPerCallsign &setup,
                                                             uint32_t aircraftNumber) const
    {
        Q_ASSERT(m_interpolator);
        return m_interpolator->getInterpolation(currentTimeSinceEpoch, setup, aircraftNumber);
    }

    CStatusMessageList
    CXPlaneMPAircraft::getInterpolationMessages(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
    {
        return this->getInterpolator() ? this->getInterpolator()->getInterpolationMessages(mode) : CStatusMessageList();
    }

    CCallsignSet CXPlaneMPAircraftObjects::getAllCallsigns() const { return { this->keys() }; }

    QStringList CXPlaneMPAircraftObjects::getAllCallsignStrings(bool sorted) const
    {
        return this->getAllCallsigns().getCallsignStrings(sorted);
    }
} // namespace swift::simplugin::xplane
