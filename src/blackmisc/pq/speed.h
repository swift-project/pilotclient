/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_SPEED_H
#define BLACKMISC_PQ_SPEED_H

#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

        /*!
         * Speed class, e.g. "m/s", "NM/h", "km/h", "ft/s"
         */
        class CSpeed : public CValueObject<CSpeed, CPhysicalQuantity<CSpeedUnit, CSpeed>>
        {
        public:
            //! Default constructor
            CSpeed() : CValueObject(0, CSpeedUnit::defaultUnit()) {}

            //! Init by double value
            CSpeed(double value, const CSpeedUnit &unit) : CValueObject(value, unit) {}

            //! \copydoc CPhysicalQuantity(const QString &unitString)
            CSpeed(const QString &unitString) : CValueObject(unitString) {}
        };

    }
}

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CSpeed)

#endif // guard
