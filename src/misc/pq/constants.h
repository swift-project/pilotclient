// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_CONSTANTS_H
#define SWIFT_MISC_PQ_CONSTANTS_H

#include "misc/pq/frequency.h"
#include "misc/pq/pressure.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::physical_quantities
{
    /*!
     * Physical quantities constants
     */
    class SWIFT_MISC_EXPORT CPhysicalQuantitiesConstants
    {
    public:
        //! International Standard Atmosphere pressure at mean sea level, 1013.25hPa
        static const CPressure &ISASeaLevelPressure()
        {
            static CPressure p(1013.25, CPressureUnit::mbar());
            return p;
        }

        //! Unicom frequency
        static const CFrequency &FrequencyUnicom()
        {
            static CFrequency f(122.8, CFrequencyUnit::MHz());
            return f;
        }
    };

} // namespace swift::misc::physical_quantities

#endif // SWIFT_MISC_PQ_CONSTANTS_H
