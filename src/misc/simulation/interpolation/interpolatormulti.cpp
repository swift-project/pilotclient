// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "misc/simulation/interpolation/interpolatormulti.h"

using namespace swift::misc::aviation;

namespace swift::misc::simulation
{
    CInterpolatorMulti::CInterpolatorMulti(const CCallsign &callsign, ISimulationEnvironmentProvider *p1,
                                           IInterpolationSetupProvider *p2, IRemoteAircraftProvider *p3,
                                           CInterpolationLogger *logger)
        : m_spline(callsign, p1, p2, p3, logger), m_linear(callsign, p1, p2, p3, logger),
          m_velocity(callsign, p1, p2, p3, logger)
    {}

    CInterpolationResult CInterpolatorMulti::getInterpolation(qint64 currentTimeSinceEpoch,
                                                              const CInterpolationAndRenderingSetupPerCallsign &setup,
                                                              uint32_t aircraftNumber)
    {
        const CInterpolationAndRenderingSetupBase::InterpolatorMode mode = setup.getInterpolatorMode();

        // auto mode: an aircraft delivering velocity data (VATSIM visual positions ^ / #SL / #ST) is
        // extrapolated from those, no matter whether spline or linear is configured
        if (mode == CInterpolationAndRenderingSetupBase::Velocity || m_velocity.hasVelocityData())
        {
            return m_velocity.getInterpolation(currentTimeSinceEpoch, setup, aircraftNumber);
        }

        switch (mode)
        {
        case CInterpolationAndRenderingSetupBase::Linear:
            return m_linear.getInterpolation(currentTimeSinceEpoch, setup, aircraftNumber);
        case CInterpolationAndRenderingSetupBase::Spline:
            return m_spline.getInterpolation(currentTimeSinceEpoch, setup, aircraftNumber);
        default: break;
        }

        return {};
    }

    const CAircraftSituation &
    CInterpolatorMulti::getLastInterpolatedSituation(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
    {
        switch (mode)
        {
        case CInterpolationAndRenderingSetupBase::Linear: return m_linear.getLastInterpolatedSituation();
        case CInterpolationAndRenderingSetupBase::Spline: return m_spline.getLastInterpolatedSituation();
        case CInterpolationAndRenderingSetupBase::Velocity: return m_velocity.getLastInterpolatedSituation();
        default: break;
        }
        return CAircraftSituation::null();
    }

    void CInterpolatorMulti::attachLogger(CInterpolationLogger *logger)
    {
        m_linear.attachLogger(logger);
        m_spline.attachLogger(logger);
        m_velocity.attachLogger(logger);
    }

    void CInterpolatorMulti::initCorrespondingModel(const CAircraftModel &model)
    {
        m_linear.initCorrespondingModel(model);
        m_spline.initCorrespondingModel(model);
        m_velocity.initCorrespondingModel(model);
    }

    const CStatusMessageList &
    CInterpolatorMulti::getInterpolationMessages(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
    {
        switch (mode)
        {
        case CInterpolationAndRenderingSetupBase::Spline: return m_spline.getInterpolationMessages();
        case CInterpolationAndRenderingSetupBase::Linear: return m_linear.getInterpolationMessages();
        case CInterpolationAndRenderingSetupBase::Velocity: return m_velocity.getInterpolationMessages();
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
        case CInterpolationAndRenderingSetupBase::Velocity: return m_velocity.getInterpolatorInfo();
        default: break;
        }
        return ("Illegal mode");
    }

    CInterpolatorMultiWrapper::CInterpolatorMultiWrapper(const aviation::CCallsign &callsign,
                                                         ISimulationEnvironmentProvider *p1,
                                                         IInterpolationSetupProvider *p2, IRemoteAircraftProvider *p3,
                                                         CInterpolationLogger *logger)
    {
        m_interpolator.reset(new CInterpolatorMulti(callsign, p1, p2, p3));
        m_interpolator->attachLogger(logger);
    }
} // namespace swift::misc::simulation
