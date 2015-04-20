/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_LENGTH_H
#define BLACKMISC_PQ_LENGTH_H

#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc
{

    //! \private
    template <> struct CValueObjectPolicy<PhysicalQuantities::CLength> : public CValueObjectPolicy<>
    {
        using MetaType = Policy::MetaType::DefaultAndQList;
    };

    namespace PhysicalQuantities
    {

        //! Physical unit length (length)
        class CLength : public CValueObject<CLength, CPhysicalQuantity<CLengthUnit, CLength>>
        {
        public:
            //! Default constructor
            CLength() : CValueObject(0, CLengthUnit::defaultUnit()) {}

            //! Init by double value
            CLength(double value, const CLengthUnit &unit) : CValueObject(value, unit) {}

            //! \copydoc CPhysicalQuantity(const QString &unitString)
            CLength(const QString &unitString) : CValueObject(unitString) {}
        };

    }
}

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CLength)

#endif // guard
