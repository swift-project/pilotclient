/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "xplanempaircraft.h"
#include "blackmisc/simulation/interpolatormulti.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CXPlaneMPAircraft::CXPlaneMPAircraft()
        { }

        CXPlaneMPAircraft::CXPlaneMPAircraft(const CSimulatedAircraft &aircraft,
                                             CInterpolationLogger *logger) :
            m_aircraft(aircraft),
            m_interpolator(QSharedPointer<CInterpolatorMulti>::create(aircraft.getCallsign()))
        {
            m_interpolator->attachLogger(logger);

            // if available set situation and parts
            if (aircraft.isPartsSynchronized()) { this->addAircraftParts(aircraft.getParts()); }
            if (aircraft.getSituation().hasValidTimestamp()) { this->addAircraftSituation(aircraft.getSituation()); }
        }

        CXPlaneMPAircraft::CXPlaneMPAircraft(const CAircraftSituation &situation) :
            m_interpolator(QSharedPointer<CInterpolatorMulti>::create(situation.getCallsign()))
        {
            if (situation.hasValidTimestamp()) { this->addAircraftSituation(situation); }
        }

        CXPlaneMPAircraft::CXPlaneMPAircraft(const CAircraftParts &parts, const CCallsign &callsign) :
            m_interpolator(QSharedPointer<CInterpolatorMulti>::create(callsign))
        {
            if (parts.hasValidTimestamp()) { this->addAircraftParts(parts); }
        }

        void CXPlaneMPAircraft::addAircraftParts(const CAircraftParts &parts)
        {
            Q_ASSERT(m_interpolator);
            Q_ASSERT(parts.hasValidTimestamp());
            m_interpolator->addAircraftParts(parts);
            m_aircraft.setParts(parts);
        }

        void CXPlaneMPAircraft::addAircraftSituation(const CAircraftSituation &situation)
        {
            Q_ASSERT(m_interpolator);
            Q_ASSERT(situation.hasValidTimestamp());
            m_interpolator->addAircraftSituation(situation);
            m_aircraft.setSituation(situation); // update with last situation
        }

        bool CXPlaneMPAircraft::isSameAsSent(const CAircraftSituation &position) const
        {
            return m_positionAsSent == position;
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
            const CInterpolationAndRenderingSetup &setup,
            const CInterpolationHints &hints, CInterpolationStatus &status) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatedSituation(currentTimeSinceEpoc, setup, hints, status);
        }

        CAircraftParts CXPlaneMPAircraft::getInterpolatedParts(
            qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetup &setup,
            CPartsStatus &partsStatus, bool log) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatedParts(currentTimeSinceEpoc, setup, partsStatus, log);
        }

        CCallsignSet CXPlaneMPAircrafts::getAllCallsigns() const
        {
            return CCallsignSet(this->keys());
        }

        int CXPlaneMPAircrafts::setInterpolatorModes(CInterpolatorMulti::Mode mode)
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
