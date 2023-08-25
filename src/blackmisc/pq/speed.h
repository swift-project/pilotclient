// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_PQ_SPEED_H
#define BLACKMISC_PQ_SPEED_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"
#include <QPair>

namespace BlackMisc::PhysicalQuantities
{
    /*!
     * Speed class, e.g. "m/s", "NM/h", "km/h", "ft/s"
     */
    class BLACKMISC_EXPORT CSpeed : public CPhysicalQuantity<CSpeedUnit, CSpeed>
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
} // ns

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CSpeed)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CSpeedPair)

#endif // guard
