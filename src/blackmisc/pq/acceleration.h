// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
