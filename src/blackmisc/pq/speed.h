/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_SPEED_H
#define BLACKMISC_PQ_SPEED_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"
#include <QPair>

namespace BlackMisc
{
    namespace PhysicalQuantities
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

        using CSpeedPair  = QPair<CSpeed, CSpeed>;   //!< Pair of speeds
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CSpeed)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CSpeedPair)

#endif // guard
