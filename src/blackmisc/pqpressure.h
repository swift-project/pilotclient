/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQPRESSURE_H
#define BLACKMISC_PQPRESSURE_H

#include "pqphysicalquantity.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

        /*!
         * \brief Physical unit distance
         */
        class CPressure : public CPhysicalQuantity<CPressureUnit, CPressure>
        {
        public:
            //! Default constructor
            CPressure() : CPhysicalQuantity(0, CPressureUnit::defaultUnit()) {}

            //! Init by double value
            CPressure(double value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit) {}

            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! Virtual destructor
            virtual ~CPressure() {}
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CPressure)

#endif // BLACKMISC_PQPRESSURE_H
