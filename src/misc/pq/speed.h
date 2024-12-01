// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_SPEED_H
#define SWIFT_MISC_PQ_SPEED_H

#include <QPair>

#include "misc/pq/physicalquantity.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::physical_quantities
{
    /*!
     * Speed class, e.g. "m/s", "NM/h", "km/h", "ft/s"
     */
    class SWIFT_MISC_EXPORT CSpeed : public CPhysicalQuantity<CSpeedUnit, CSpeed>
    {
    public:
        //! Default constructor
        CSpeed() : CPhysicalQuantity(0, CSpeedUnit::defaultUnit()) {}

        //! Init by double value
        CSpeed(double value, const CSpeedUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CSpeed(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };

    using CSpeedPair = QPair<CSpeed, CSpeed>; //!< Pair of speeds
} // namespace swift::misc::physical_quantities

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CSpeed)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CSpeedPair)

#endif // SWIFT_MISC_PQ_SPEED_H
