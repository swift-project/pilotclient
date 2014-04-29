/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_CSPEED_H
#define BLACKMISC_CSPEED_H
#include "pqphysicalquantity.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

        /*!
         * \brief Speed class, e.g. "m/s", "NM/h", "km/h", "ft/s"
         */
        class CSpeed : public CPhysicalQuantity<CSpeedUnit, CSpeed>
        {

        public:
            //! Default constructor
            CSpeed() : CPhysicalQuantity(0, CSpeedUnit::defaultUnit()) {}

            //! Init by double value
            CSpeed(double value, const CSpeedUnit &unit) : CPhysicalQuantity(value, unit) {}


            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! Destructor
            virtual ~CSpeed() {}
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CSpeed)

#endif // BLACKMISC_CSPEED_H
