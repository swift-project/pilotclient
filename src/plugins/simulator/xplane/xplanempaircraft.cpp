/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "xplanempaircraft.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/interpolatormulti.h"

using namespace BlackCore;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CXPlaneMPAircraft::CXPlaneMPAircraft()
        { }

        CXPlaneMPAircraft::CXPlaneMPAircraft(
            const CSimulatedAircraft &aircraft, ISimulator *simulator, CInterpolationLogger *logger) :
            m_aircraft(aircraft),
            m_interpolator(QSharedPointer<CInterpolatorMulti>::create(aircraft.getCallsign(), simulator, simulator, simulator->getRemoteAircraftProvider(), logger))
        {
            m_interpolator->attachLogger(logger);
        }

        bool CXPlaneMPAircraft::isSameAsSent(const CAircraftSituation &position) const
        {
            return m_situationAsSent == position;
        }

        void CXPlaneMPAircraft::toggleInterpolatorMode()
        {
            Q_ASSERT(m_interpolator);
            m_interpolator->toggleMode();
        }

        bool CXPlaneMPAircraft::setInterpolatorMode(CInterpolatorMulti::Mode mode)
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->setMode(mode);
        }

        QString CXPlaneMPAircraft::getInterpolatorInfo() const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatorInfo();
        }

        void CXPlaneMPAircraft::attachInterpolatorLogger(CInterpolationLogger *logger)
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->attachLogger(logger);
        }

        CAircraftSituation CXPlaneMPAircraft::getInterpolatedSituation(
            qint64 currentTimeSinceEpoc,
            const CInterpolationAndRenderingSetupPerCallsign &setup,
            CInterpolationStatus &status) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatedSituation(currentTimeSinceEpoc, setup, status);
        }

        CAircraftParts CXPlaneMPAircraft::getInterpolatedParts(
            qint64 currentTimeSinceEpoc,
            const CInterpolationAndRenderingSetupPerCallsign &setup,
            CPartsStatus &partsStatus, bool log) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatedParts(currentTimeSinceEpoc, setup, partsStatus, log);
        }

        CCallsignSet CXPlaneMPAircraftObjects::getAllCallsigns() const
        {
            return CCallsignSet(this->keys());
        }

        int CXPlaneMPAircraftObjects::setInterpolatorModes(CInterpolatorMulti::Mode mode)
        {
            int c = 0;
            for (const CCallsign &cs : this->keys())
            {
                if ((*this)[cs].setInterpolatorMode(mode)) c++;
            }
            return c;
        }
    } // namespace
} // namespace
