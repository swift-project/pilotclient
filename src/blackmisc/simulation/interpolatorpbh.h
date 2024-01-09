//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORPBH_H
#define BLACKMISC_SIMULATION_INTERPOLATORPBH_H

#include "blackmisc/aviation/heading.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/aviation/aircraftsituation.h"

namespace BlackMisc::Simulation
{
    //! Base class for Pitch-Bank-Heading interpolators
    class IInterpolatorPbh
    {
    public:
        virtual ~IInterpolatorPbh() = default;

        //! @{
        //! Getter
        virtual Aviation::CHeading getHeading() const = 0;
        virtual PhysicalQuantities::CAngle getPitch() const = 0;
        virtual PhysicalQuantities::CAngle getBank() const = 0;
        virtual PhysicalQuantities::CSpeed getGroundSpeed() const = 0;
        virtual const Aviation::CAircraftSituation &getStartSituation() const = 0;
        virtual const Aviation::CAircraftSituation &getEndSituation() const = 0;
        //! @}
    };
}

#endif // BLACKMISC_SIMULATION_INTERPOLATORPBH_H
