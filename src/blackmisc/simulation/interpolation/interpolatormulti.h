// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATION_INTERPOLATORMULTI_H
#define BLACKMISC_SIMULATION_INTERPOLATION_INTERPOLATORMULTI_H

#include "blackmisc/simulation/interpolation/interpolatorlinear.h"
#include "blackmisc/simulation/interpolation/interpolatorspline.h"
#include "blackmisc/statusmessagelist.h"

namespace BlackMisc::Simulation
{
    //! Multiplexed interpolator which allows switching between modes at runtime.
    class BLACKMISC_EXPORT CInterpolatorMulti
    {
    public:
        //! Constructor
        CInterpolatorMulti(const Aviation::CCallsign &callsign,
                           ISimulationEnvironmentProvider *p1, IInterpolationSetupProvider *p2, IRemoteAircraftProvider *p3,
                           CInterpolationLogger *logger = nullptr);

        //! \copydoc CInterpolator::getInterpolation
        CInterpolationResult getInterpolation(qint64 currentTimeSinceEpoch, const CInterpolationAndRenderingSetupPerCallsign &setup, uint32_t aircraftNumber);

        //! \copydoc CInterpolator::getLastInterpolatedSituation
        const Aviation::CAircraftSituation &getLastInterpolatedSituation(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! \copydoc CInterpolator::getInterpolationMessages
        const CStatusMessageList &getInterpolationMessages(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

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
    class BLACKMISC_EXPORT CInterpolatorMultiWrapper
    {
    public:
        //! Default ctor, needed for QMap/QHash
        CInterpolatorMultiWrapper();

        //! Constructor
        CInterpolatorMultiWrapper(
            const Aviation::CCallsign &callsign,
            ISimulationEnvironmentProvider *p1, IInterpolationSetupProvider *p2, IRemoteAircraftProvider *p3,
            CInterpolationLogger *logger = nullptr);

        //! Has interpolator initialized?
        bool hasInterpolator() const { return m_interpolator; }

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
        QSharedPointer<CInterpolatorMulti> m_interpolator; //!< shared pointer because CInterpolatorMultiWrapper can be copied
    };
} // ns

#endif
