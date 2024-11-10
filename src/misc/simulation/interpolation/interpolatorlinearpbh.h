// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORLINEARPBH_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORLINEARPBH_H

#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/heading.h"
#include "misc/pq/angle.h"
#include "misc/pq/speed.h"
#include "misc/swiftmiscexport.h"
#include "misc/simulation/interpolation/interpolatorpbh.h"

namespace swift::misc::simulation
{
    //! Simple linear interpolator for pitch, bank, heading and groundspeed from start to end situation
    class SWIFT_MISC_EXPORT CInterpolatorLinearPbh : public IInterpolatorPbh
    {
    public:
        //! @{
        //! Constructor
        CInterpolatorLinearPbh() = default;
        CInterpolatorLinearPbh(const aviation::CAircraftSituation &start, const aviation::CAircraftSituation &end) : m_startSituation(start), m_endSituation(end) {}
        CInterpolatorLinearPbh(double simulationTimeFraction, const aviation::CAircraftSituation &start, const aviation::CAircraftSituation &end);
        //! @}

        //! @{
        //! Getter
        aviation::CHeading getHeading() const override;
        physical_quantities::CAngle getPitch() const override;
        physical_quantities::CAngle getBank() const override;
        physical_quantities::CSpeed getGroundSpeed() const override;
        const aviation::CAircraftSituation &getStartSituation() const override { return m_startSituation; }
        const aviation::CAircraftSituation &getEndSituation() const override { return m_endSituation; }
        //! @}

        //! Change time fraction
        void setTimeFraction(double tf);

    private:
        //! Interpolate angle
        static physical_quantities::CAngle interpolateAngle(const physical_quantities::CAngle &begin, const physical_quantities::CAngle &end, double timeFraction0to1);

        double m_simulationTimeFraction = 0.0; //!< Value within [0;1] to blend between the situations
        aviation::CAircraftSituation m_startSituation;
        aviation::CAircraftSituation m_endSituation;
    };
} // namespace

#endif
