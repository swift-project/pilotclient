// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLANT_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLANT_H

#include "misc/simulation/interpolation/interpolatorlinearpbh.h"

namespace swift::misc::simulation
{
    //! Interpolant interface
    class IInterpolant
    {
    public:
        //! Default ctor
        IInterpolant() = default;

        //! Constructor
        explicit IInterpolant(qint64 interpolatedTime) : m_interpolatedTime(interpolatedTime) {}

        virtual ~IInterpolant() = default;

        //! Perform the interpolation
        //! \return interpolated position and altitude
        virtual std::tuple<geo::CCoordinateGeodetic, aviation::CAltitude> interpolatePositionAndAltitude() const = 0;

        //! Interpolate the ground information/factor
        virtual aviation::COnGroundInfo interpolateGroundFactor() const = 0;

        //! Get the PBH interpolator
        virtual const IInterpolatorPbh &pbh() const = 0;

        //! "Real time" representing the interpolated situation
        qint64 getInterpolatedTime() const { return m_interpolatedTime; }

        //! Valid?
        bool isValid() const { return m_valid; }

        //! Valid?
        void setValid(bool valid) { m_valid = valid; }

        //! Is recalculated interpolant?
        bool isRecalculated() const { return m_recalculated; }

        //! Set recalculated interpolant
        void setRecalculated(bool reCalculated) { m_recalculated = reCalculated; }

    protected:
        qint64 m_interpolatedTime = -1; //!< "Real time "of interpolated situation
        bool m_valid = true; //!< valid?
        bool m_recalculated = false; //!< recalculated interpolant
    };
} // namespace
#endif // guard
