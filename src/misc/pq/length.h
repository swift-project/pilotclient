// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_LENGTH_H
#define SWIFT_MISC_PQ_LENGTH_H

#include <QPair>

#include "misc/pq/physicalquantity.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::physical_quantities
{
    //! Physical unit length (length)
    class SWIFT_MISC_EXPORT CLength : public CPhysicalQuantity<CLengthUnit, CLength>
    {
    public:
        //! Default constructor
        CLength() : CPhysicalQuantity(0, CLengthUnit::defaultUnit()) {}

        //! Init by double value
        CLength(double value, const CLengthUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! Init by double value and switch unit
        CLength(double value, const CLengthUnit &unit, const CLengthUnit &switchUnit) : CPhysicalQuantity(value, unit)
        {
            this->switchUnit(switchUnit);
        }

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CLength(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };

    using CLengthPair = QPair<CLength, CLength>; //!< Pair of length
} // namespace swift::misc::physical_quantities

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CLength)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CLengthPair)

#endif // guard
