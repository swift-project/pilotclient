// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_ACCELERATION_H
#define SWIFT_MISC_PQ_ACCELERATION_H

#include "misc/pq/physicalquantity.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::physical_quantities
{

    //! Acceleration
    class SWIFT_MISC_EXPORT CAcceleration : public CPhysicalQuantity<CAccelerationUnit, CAcceleration>
    {
    public:
        //! Default constructor
        CAcceleration() : CPhysicalQuantity(0, CAccelerationUnit::defaultUnit()) {}

        //! Init by double value
        CAcceleration(double value, const CAccelerationUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CAcceleration(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };

} // namespace swift::misc::physical_quantities

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CAcceleration)

#endif // guard
