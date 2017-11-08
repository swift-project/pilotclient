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
        CInterpolatorMulti::CInterpolatorMulti(const CCallsign &callsign, QObject *parent) :
            QObject(parent),
            m_spline(callsign, this),
            m_linear(callsign, this)
        {}

        CAircraftSituation CInterpolatorMulti::getInterpolatedSituation(
            qint64 currentTimeSinceEpoc,
            const CInterpolationAndRenderingSetup &setup,
            const CInterpolationHints &hints, CInterpolationStatus &status)
        {
            switch (m_mode)
            {
            case ModeLinear: return m_linear.getInterpolatedSituation(currentTimeSinceEpoc, setup, hints, status);
            case ModeSpline: return m_spline.getInterpolatedSituation(currentTimeSinceEpoc, setup, hints, status);
            default: break;
            }
            return {};
        }

        CAircraftParts CInterpolatorMulti::getInterpolatedParts(
            qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetup &setup,
            CPartsStatus &partsStatus, bool log)
        {
            switch (m_mode)
            {
            case ModeLinear: return m_linear.getInterpolatedParts(currentTimeSinceEpoc, setup, partsStatus, log);
            case ModeSpline: return m_spline.getInterpolatedParts(currentTimeSinceEpoc, setup, partsStatus, log);
            default: break;
            }
            return {};
        }

        void CInterpolatorMulti::addAircraftSituation(const CAircraftSituation &situation)
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
            default: break;
            }
            return false;
        }

        void CInterpolatorMulti::addAircraftParts(const CAircraftParts &parts)
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
            default: break;
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

        CInterpolatorMultiWrapper::CInterpolatorMultiWrapper(const CCallsign &callsign, QObject *parent)
        {
            m_interpolator.reset(new CInterpolatorMulti(callsign, parent));
        }

        CInterpolatorMultiWrapper::CInterpolatorMultiWrapper(const CCallsign &callsign, CInterpolationLogger *logger, QObject *parent)
        {
            m_interpolator.reset(new CInterpolatorMulti(callsign, parent));
            m_interpolator->attachLogger(logger);
        }
    } // ns
} // ns
