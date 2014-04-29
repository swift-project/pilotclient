/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQACCELERATION_H
#define BLACKMISC_PQACCELERATION_H
#include "blackmisc/pqphysicalquantity.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

        //! Acceleration
        class CAcceleration : public CPhysicalQuantity<CAccelerationUnit, CAcceleration>
        {
        public:
            //! Default constructor
            CAcceleration() : CPhysicalQuantity(0, CAccelerationUnit::defaultUnit()) {}

            //! Init by double value
            CAcceleration(double value, const CAccelerationUnit &unit) : CPhysicalQuantity(value, unit) {}

            //! \copydoc CPhysicalQuantity(const QString &unitString)
            //! copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Virtual destructor
            virtual ~CAcceleration() {}
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAcceleration)

#endif // guard
