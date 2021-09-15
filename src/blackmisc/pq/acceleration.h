/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_ACCELERATION_H
#define BLACKMISC_PQ_ACCELERATION_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc::PhysicalQuantities
{

    //! Acceleration
    class BLACKMISC_EXPORT CAcceleration : public CPhysicalQuantity<CAccelerationUnit, CAcceleration>
    {
    public:
        //! Default constructor
        CAcceleration() : CPhysicalQuantity(0, CAccelerationUnit::defaultUnit()) {}

        //! Init by double value
        CAcceleration(double value, const CAccelerationUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CAcceleration(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };

}

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAcceleration)

#endif // guard
