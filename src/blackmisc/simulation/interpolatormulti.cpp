/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#include "blackmisc/simulation/interpolatormulti.h"

namespace BlackMisc
{
    namespace Simulation
    {
        CInterpolatorMulti::CInterpolatorMulti(const BlackMisc::Aviation::CCallsign &callsign, QObject *parent) :
            QObject(parent),
            m_spline(callsign, this),
            m_linear(callsign, this)
        {}

        BlackMisc::Aviation::CAircraftSituation CInterpolatorMulti::getInterpolatedSituation(
            qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints, CInterpolationStatus &status)
        {
            switch (m_mode)
            {
            case ModeLinear:
                return m_linear.getInterpolatedSituation(currentTimeSinceEpoc, setup, hints, status);
            case ModeSpline:
                return m_spline.getInterpolatedSituation(currentTimeSinceEpoc, setup, hints, status);
            }
            return {};
        }

        BlackMisc::Aviation::CAircraftParts CInterpolatorMulti::getInterpolatedParts(
            qint64 cutoffTime, const CInterpolationAndRenderingSetup &setup, CPartsStatus &partsStatus, bool log)
        {
            switch (m_mode)
            {
            case ModeLinear:
                return m_linear.getInterpolatedParts(cutoffTime, setup, partsStatus, log);
            case ModeSpline:
                return m_spline.getInterpolatedParts(cutoffTime, setup, partsStatus, log);
            }
            return {};
        }

        void CInterpolatorMulti::addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
        {
            m_linear.addAircraftSituation(situation);
            m_spline.addAircraftSituation(situation);
        }

        bool CInterpolatorMulti::hasAircraftSituations() const
        {
            switch (m_mode)
            {
            case ModeLinear: return m_linear.hasAircraftSituations();
            case ModeSpline: return m_spline.hasAircraftSituations();
            }
            return false;
        }

        void CInterpolatorMulti::addAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts)
        {
            m_linear.addAircraftParts(parts);
            m_spline.addAircraftParts(parts);
        }

        bool CInterpolatorMulti::hasAircraftParts() const
        {
            switch (m_mode)
            {
            case ModeLinear: return m_linear.hasAircraftParts();
            case ModeSpline: return m_spline.hasAircraftParts();
            }
            return false;
        }

        void CInterpolatorMulti::attachLogger(CInterpolationLogger *logger)
        {
            m_linear.attachLogger(logger);
            m_spline.attachLogger(logger);
        }

        bool CInterpolatorMulti::setMode(Mode mode)
        {
#ifdef QT_DEBUG
            if (m_mode != mode)
            {
                m_mode = mode;
                return true;
            }
#else
            Q_UNUSED(mode);
#endif
            return false;
        }
    }
}
