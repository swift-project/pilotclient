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
            switch (m_mode)
            {
            case ModeLinear: return m_linear.getInterpolatedSituation(currentTimeSinceEpoc, setup, status);
            case ModeSpline: return m_spline.getInterpolatedSituation(currentTimeSinceEpoc, setup, status);
            default: break;
            }
            return {};
        }

        CAircraftParts CInterpolatorMulti::getInterpolatedParts(
            qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup,
            CPartsStatus &partsStatus, bool log) const
        {
            switch (m_mode)
            {
            // currently calls the same interpolation for parts
            case ModeLinear: return m_linear.getInterpolatedParts(currentTimeSinceEpoc, setup, partsStatus, log);
            case ModeSpline: return m_spline.getInterpolatedParts(currentTimeSinceEpoc, setup, partsStatus, log);
            default: break;
            }
            return {};
        }

        CAircraftParts CInterpolatorMulti::getInterpolatedOrGuessedParts(
            qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup,
            CPartsStatus &partsStatus, bool log) const
        {
            switch (m_mode)
            {
            // currently calls the same interpolation for parts
            case ModeLinear: return m_linear.getInterpolatedOrGuessedParts(currentTimeSinceEpoc, setup, partsStatus, log);
            case ModeSpline: return m_spline.getInterpolatedOrGuessedParts(currentTimeSinceEpoc, setup, partsStatus, log);
            default: break;
            }
            return {};
        }

        const CAircraftSituation &CInterpolatorMulti::getLastInterpolatedSituation() const
        {
            switch (m_mode)
            {
            case ModeLinear: return m_linear.getLastInterpolatedSituation();
            case ModeSpline: return m_spline.getLastInterpolatedSituation();
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

        bool CInterpolatorMulti::setMode(Mode mode)
        {
            if (m_mode == mode) { return false; }
            m_mode = mode;
            return true;
        }

        bool CInterpolatorMulti::setMode(const QString &mode)
        {
            Mode m = modeFromString(mode);
            if (m == ModeUnknown) { return false; }
            return setMode(m);
        }

        void CInterpolatorMulti::toggleMode()
        {
            switch (m_mode)
            {
            case ModeSpline: m_mode = ModeLinear; break;
            case ModeLinear: m_mode = ModeSpline; break;
            default: m_mode = ModeSpline; break;
            }
        }

        QString CInterpolatorMulti::getInterpolatorInfo() const
        {
            switch (m_mode)
            {
            case ModeSpline: return m_spline.getInterpolatorInfo();
            case ModeLinear: return m_linear.getInterpolatorInfo();
            default: break;
            }
            return ("Illegal mode");
        }

        CInterpolatorMulti::Mode CInterpolatorMulti::modeFromString(const QString &mode)
        {
            if (mode.contains("spli"), Qt::CaseInsensitive) { return ModeSpline; }
            if (mode.contains("lin"), Qt::CaseInsensitive) { return ModeLinear; }
            return ModeUnknown;
        }

        const QString &CInterpolatorMulti::modeToString(CInterpolatorMulti::Mode mode)
        {
            static const QString l("linear");
            static const QString s("spline");
            static const QString u("unknown");

            switch (mode)
            {
            case ModeLinear: return l;
            case ModeSpline: return s;
            case ModeUnknown:
            default: return u;
            }
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
