// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_PQ_LENGTH_H
#define BLACKMISC_PQ_LENGTH_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"
#include <QPair>

namespace BlackMisc::PhysicalQuantities
{
    //! Physical unit length (length)
    class BLACKMISC_EXPORT CLength : public CPhysicalQuantity<CLengthUnit, CLength>
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
} // ns

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CLength)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CLengthPair)

#endif // guard
