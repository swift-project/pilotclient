// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORMULTI_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORMULTI_H

#include "misc/simulation/interpolation/interpolatorlinear.h"
#include "misc/simulation/interpolation/interpolatorspline.h"
#include "misc/statusmessagelist.h"

namespace swift::misc::simulation
{
    //! Multiplexed interpolator which allows switching between modes at runtime.
    class SWIFT_MISC_EXPORT CInterpolatorMulti
    {
    public:
        //! Constructor
        CInterpolatorMulti(const aviation::CCallsign &callsign, ISimulationEnvironmentProvider *p1,
                           IInterpolationSetupProvider *p2, IRemoteAircraftProvider *p3,
                           CInterpolationLogger *logger = nullptr);

        //! \copydoc CInterpolator::getInterpolation
        CInterpolationResult getInterpolation(qint64 currentTimeSinceEpoch,
                                              const CInterpolationAndRenderingSetupPerCallsign &setup,
                                              uint32_t aircraftNumber);

        //! \copydoc CInterpolator::getLastInterpolatedSituation
        const aviation::CAircraftSituation &
        getLastInterpolatedSituation(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! \copydoc CInterpolator::getInterpolationMessages
        const CStatusMessageList &
        getInterpolationMessages(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! \copydoc CInterpolator::attachLogger
        void attachLogger(CInterpolationLogger *logger);

        //! \copydoc CInterpolator::initCorrespondingModel
        void initCorrespondingModel(const CAircraftModel &model);

        //! Info string
        QString getInterpolatorInfo(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

    private:
        CInterpolatorSpline m_spline;
        CInterpolatorLinear m_linear;
    };

    /*!
     * CInterpolatorMulti which can be used with QMap/QHash
     * \remark Use case is emulated driver
     */
    class SWIFT_MISC_EXPORT CInterpolatorMultiWrapper
    {
    public:
        //! Default ctor, needed for QMap/QHash
        CInterpolatorMultiWrapper() = default;

        //! Constructor
        CInterpolatorMultiWrapper(const aviation::CCallsign &callsign, ISimulationEnvironmentProvider *p1,
                                  IInterpolationSetupProvider *p2, IRemoteAircraftProvider *p3,
                                  CInterpolationLogger *logger = nullptr);

        //! Has interpolator initialized?
        bool hasInterpolator() const { return !m_interpolator.isNull(); }

        //! Interpolator
        CInterpolatorMulti *interpolator() const { return m_interpolator.data(); }

        //! Allows implicit conversion
        operator CInterpolatorMulti *() const { return this->interpolator(); }

        //! Bool conversion
        operator bool() const { return this->hasInterpolator(); }

        //! Bool conversion
        bool operator!() const { return !this->hasInterpolator(); }

        //! * operator
        CInterpolatorMulti &operator*() const { return *this->interpolator(); }

        //! -> operator
        CInterpolatorMulti *operator->() const { return this->interpolator(); }

    private:
        QSharedPointer<CInterpolatorMulti>
            m_interpolator; //!< shared pointer because CInterpolatorMultiWrapper can be copied
    };
} // namespace swift::misc::simulation

#endif // SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORMULTI_H
