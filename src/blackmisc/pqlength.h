/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQLENGTH_H
#define BLACKMISC_PQLENGTH_H
#include "blackmisc/pqphysicalquantity.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

        //! Physical unit length (length)
        class CLength : public CPhysicalQuantity<CLengthUnit, CLength>
        {
        public:
            //! Default constructor
            CLength() : CPhysicalQuantity(0, CLengthUnit::defaultUnit()) {}

            //! Init by double value
            CLength(double value, const CLengthUnit &unit) : CPhysicalQuantity(value, unit) {}

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! Virtual destructor
            virtual ~CLength() {}
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CLength)

#endif // guard
