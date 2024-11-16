//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORPBH_H
#define SWIFT_MISC_SIMULATION_INTERPOLATION_INTERPOLATORPBH_H

#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/heading.h"
#include "misc/pq/angle.h"
#include "misc/pq/speed.h"

namespace swift::misc::simulation
{
    //! Base class for Pitch-Bank-Heading interpolators
    class IInterpolatorPbh
    {
    public:
        virtual ~IInterpolatorPbh() = default;

        //! @{
        //! Getter
        virtual aviation::CHeading getHeading() const = 0;
        virtual physical_quantities::CAngle getPitch() const = 0;
        virtual physical_quantities::CAngle getBank() const = 0;
        virtual physical_quantities::CSpeed getGroundSpeed() const = 0;
        virtual const aviation::CAircraftSituation &getStartSituation() const = 0;
        virtual const aviation::CAircraftSituation &getEndSituation() const = 0;
        //! @}
    };
} // namespace swift::misc::simulation

#endif
