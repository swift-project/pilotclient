// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "blackmisc/simulation/interpolatormulti.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc::Simulation
{
    CInterpolatorMulti::CInterpolatorMulti(const CCallsign &callsign, ISimulationEnvironmentProvider *p1, IInterpolationSetupProvider *p2, IRemoteAircraftProvider *p3, CInterpolationLogger *logger) : m_spline(callsign, p1, p2, p3, logger),
                                                                                                                                                                                                        m_linear(callsign, p1, p2, p3, logger)
    {}

    CInterpolationResult CInterpolatorMulti::getInterpolation(qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber)
    {
        switch (setup.getInterpolatorMode())
        {
        case CInterpolationAndRenderingSetupBase::Linear: return m_linear.getInterpolation(currentTimeSinceEpoc, setup, aircraftNumber);
        case CInterpolationAndRenderingSetupBase::Spline: return m_spline.getInterpolation(currentTimeSinceEpoc, setup, aircraftNumber);
        default: break;
        }

        return CInterpolationResult();
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

    const CStatusMessageList &CInterpolatorMulti::getInterpolationMessages(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
    {
        switch (mode)
        {
        case CInterpolationAndRenderingSetupBase::Spline: return m_spline.getInterpolationMessages();
        case CInterpolationAndRenderingSetupBase::Linear: return m_linear.getInterpolationMessages();
        default: break;
        }
        static const CStatusMessageList empty;
        return empty;
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
    {}

    CInterpolatorMultiWrapper::CInterpolatorMultiWrapper(const Aviation::CCallsign &callsign, ISimulationEnvironmentProvider *p1, IInterpolationSetupProvider *p2, IRemoteAircraftProvider *p3, CInterpolationLogger *logger)
    {
        m_interpolator.reset(new CInterpolatorMulti(callsign, p1, p2, p3));
        m_interpolator->attachLogger(logger);
    }
} // ns
