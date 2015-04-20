/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_ACCELERATION_H
#define BLACKMISC_PQ_ACCELERATION_H

#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc
{

    //! \private
    template <> struct CValueObjectPolicy<PhysicalQuantities::CAcceleration> : public CValueObjectPolicy<>
    {
        using MetaType = Policy::MetaType::DefaultAndQList;
    };

    namespace PhysicalQuantities
    {

        //! Acceleration
        class CAcceleration : public CValueObject<CAcceleration, CPhysicalQuantity<CAccelerationUnit, CAcceleration>>
        {
        public:
            //! Default constructor
            CAcceleration() : CValueObject(0, CAccelerationUnit::defaultUnit()) {}

            //! Init by double value
            CAcceleration(double value, const CAccelerationUnit &unit) : CValueObject(value, unit) {}

            //! \copydoc CPhysicalQuantity(const QString &unitString)
            CAcceleration(const QString &unitString) : CValueObject(unitString) {}
        };

    }
}

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAcceleration)

#endif // guard
