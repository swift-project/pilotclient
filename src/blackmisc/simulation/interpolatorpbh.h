/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORPBH_H
#define BLACKMISC_SIMULATION_INTERPOLATORPBH_H

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc::Simulation
{
    //! Simple interpolator for pitch, bank, heading, groundspeed
    class BLACKMISC_EXPORT CInterpolatorPbh
    {
    public:
        //! Constructor
        //! @{
        CInterpolatorPbh() {}
        CInterpolatorPbh(const Aviation::CAircraftSituation &older, const Aviation::CAircraftSituation &newer) : m_oldSituation(older), m_newSituation(newer) {}
        CInterpolatorPbh(double time, const Aviation::CAircraftSituation &older, const Aviation::CAircraftSituation &newer) : m_simulationTimeFraction(time), m_oldSituation(older), m_newSituation(newer) {}
        //! @}

        //! Getter
        //! @{
        Aviation::CHeading getHeading() const;
        PhysicalQuantities::CAngle getPitch() const;
        PhysicalQuantities::CAngle getBank() const;
        PhysicalQuantities::CSpeed getGroundSpeed() const;
        const Aviation::CAircraftSituation &getOldSituation() const { return m_oldSituation; }
        const Aviation::CAircraftSituation &getNewSituation() const { return m_newSituation; }
        //! @}

        //! Set situations
        //! \remark mostly needed for UNIT tests
        void setSituations(const Aviation::CAircraftSituation &older, const Aviation::CAircraftSituation &newer);

        //! Change time fraction
        void setTimeFraction(double tf);

    private:
        //! Interpolate angle
        static PhysicalQuantities::CAngle interpolateAngle(const PhysicalQuantities::CAngle &begin, const PhysicalQuantities::CAngle &end, double timeFraction0to1);

        double m_simulationTimeFraction = 0.0;
        Aviation::CAircraftSituation m_oldSituation;
        Aviation::CAircraftSituation m_newSituation;
    };
} // namespace
#endif // guard
