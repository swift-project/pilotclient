// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_PRESSURE_H
#define SWIFT_MISC_PQ_PRESSURE_H

#include "misc/pq/physicalquantity.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::physical_quantities
{
    /*!
     * Physical unit distance
     */
    class SWIFT_MISC_EXPORT CPressure : public CPhysicalQuantity<CPressureUnit, CPressure>
    {
    public:
        //! Default constructor
        CPressure() : CPhysicalQuantity(0, CPressureUnit::defaultUnit()) {}

        //! Init by double value
        CPressure(double value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CPressure(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };
} // namespace swift::misc::physical_quantities

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CPressure)

#endif // guard
