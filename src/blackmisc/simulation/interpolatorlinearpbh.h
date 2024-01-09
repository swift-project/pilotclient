// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORLINEARPBH_H
#define BLACKMISC_SIMULATION_INTERPOLATORLINEARPBH_H

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/interpolatorpbh.h"

namespace BlackMisc::Simulation
{
    //! Simple linear interpolator for pitch, bank, heading and groundspeed from start to end situation
    class BLACKMISC_EXPORT CInterpolatorLinearPbh : public IInterpolatorPbh
    {
    public:
        //! @{
        //! Constructor
        CInterpolatorLinearPbh() = default;
        CInterpolatorLinearPbh(const Aviation::CAircraftSituation &start, const Aviation::CAircraftSituation &end) : m_startSituation(start), m_endSituation(end) {}
        CInterpolatorLinearPbh(double simulationTimeFraction, const Aviation::CAircraftSituation &start, const Aviation::CAircraftSituation &end);
        //! @}

        //! @{
        //! Getter
        Aviation::CHeading getHeading() const override;
        PhysicalQuantities::CAngle getPitch() const override;
        PhysicalQuantities::CAngle getBank() const override;
        PhysicalQuantities::CSpeed getGroundSpeed() const override;
        const Aviation::CAircraftSituation &getStartSituation() const override { return m_startSituation; }
        const Aviation::CAircraftSituation &getEndSituation() const override { return m_endSituation; }
        //! @}

        //! Change time fraction
        void setTimeFraction(double tf);

    private:
        //! Interpolate angle
        static PhysicalQuantities::CAngle interpolateAngle(const PhysicalQuantities::CAngle &begin, const PhysicalQuantities::CAngle &end, double timeFraction0to1);

        double m_simulationTimeFraction = 0.0; //!< Value within [0;1] to blend between the situations
        Aviation::CAircraftSituation m_startSituation;
        Aviation::CAircraftSituation m_endSituation;
    };
} // namespace
#endif // guard
