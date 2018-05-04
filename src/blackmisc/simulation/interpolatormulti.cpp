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

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {
        CInterpolatorMulti::CInterpolatorMulti(const CCallsign &callsign, ISimulationEnvironmentProvider *p1, IInterpolationSetupProvider *p2, IRemoteAircraftProvider *p3, CInterpolationLogger *logger) :
            m_spline(callsign, p1, p2, p3, logger),
            m_linear(callsign, p1, p2, p3, logger)
        {}

        CAircraftSituation CInterpolatorMulti::getInterpolatedSituation(qint64 currentTimeSinceEpoc,
                const CInterpolationAndRenderingSetupPerCallsign &setup,
                CInterpolationStatus &status)
        {
            switch (setup.getInterpolatorMode())
            {
            case CInterpolationAndRenderingSetupBase::Linear: return m_linear.getInterpolatedSituation(currentTimeSinceEpoc, setup, status);
            case CInterpolationAndRenderingSetupBase::Spline: return m_spline.getInterpolatedSituation(currentTimeSinceEpoc, setup, status);
            default: break;
            }
            return {};
        }

        CAircraftParts CInterpolatorMulti::getInterpolatedParts(
            qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup,
            CPartsStatus &partsStatus, bool log) const
        {
            switch (setup.getInterpolatorMode())
            {
            // currently calls the same interpolation for parts
            case CInterpolationAndRenderingSetupBase::Linear: return m_linear.getInterpolatedParts(currentTimeSinceEpoc, setup, partsStatus, log);
            case CInterpolationAndRenderingSetupBase::Spline: return m_spline.getInterpolatedParts(currentTimeSinceEpoc, setup, partsStatus, log);
            default: break;
            }
            return {};
        }

        CAircraftParts CInterpolatorMulti::getInterpolatedOrGuessedParts(
            qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup,
            CPartsStatus &partsStatus, bool log) const
        {
            switch (setup.getInterpolatorMode())
            {
            // currently calls the same interpolation for parts
            case CInterpolationAndRenderingSetupBase::Linear: return m_linear.getInterpolatedOrGuessedParts(currentTimeSinceEpoc, setup, partsStatus, log);
            case CInterpolationAndRenderingSetupBase::Spline: return m_spline.getInterpolatedOrGuessedParts(currentTimeSinceEpoc, setup, partsStatus, log);
            default: break;
            }
            return {};
        }

        const CAircraftSituation &CInterpolatorMulti::getLastInterpolatedSituation(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
        {
            switch (mode)
            {
            case CInterpolationAndRenderingSetupBase::Linear: return m_linear.getLastInterpolatedSituation();
            case CInterpolationAndRenderingSetupBase::Spline: return m_spline.getLastInterpolatedSituation();
            default: break;
            }
            return CAircraftSituation::null();
        }

        void CInterpolatorMulti::attachLogger(CInterpolationLogger *logger)
        {
            m_linear.attachLogger(logger);
            m_spline.attachLogger(logger);
        }

        void CInterpolatorMulti::initCorrespondingModel(const CAircraftModel &model)
        {
            m_linear.initCorrespondingModel(model);
            m_spline.initCorrespondingModel(model);
        }

        QString CInterpolatorMulti::getInterpolatorInfo(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
        {
            switch (mode)
            {
            case CInterpolationAndRenderingSetupBase::Spline: return m_spline.getInterpolatorInfo();
            case CInterpolationAndRenderingSetupBase::Linear: return m_linear.getInterpolatorInfo();
            default: break;
            }
            return ("Illegal mode");
        }

        CInterpolatorMultiWrapper::CInterpolatorMultiWrapper()
        { }

        CInterpolatorMultiWrapper::CInterpolatorMultiWrapper(const Aviation::CCallsign &callsign, ISimulationEnvironmentProvider *p1, IInterpolationSetupProvider *p2, IRemoteAircraftProvider *p3, CInterpolationLogger *logger)
        {
            m_interpolator.reset(new CInterpolatorMulti(callsign, p1, p2, p3));
            m_interpolator->attachLogger(logger);
        }
    } // ns
} // ns
