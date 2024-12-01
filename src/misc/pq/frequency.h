// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_FREQUENCY_H
#define SWIFT_MISC_PQ_FREQUENCY_H

#include "misc/pq/physicalquantity.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::physical_quantities
{
    /*!
     * Physical unit frequency
     */
    class SWIFT_MISC_EXPORT CFrequency : public CPhysicalQuantity<CFrequencyUnit, CFrequency>
    {
    public:
        //! Default constructor
        CFrequency() : CPhysicalQuantity(0, CFrequencyUnit::defaultUnit()) {}

        //! Init by double value
        CFrequency(double value, const CFrequencyUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! Init by int value converted to double
        CFrequency(int value, const CFrequencyUnit &unit) : CFrequency(double(value), unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CFrequency(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };
} // namespace swift::misc::physical_quantities

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CFrequency)

#endif // SWIFT_MISC_PQ_FREQUENCY_H
