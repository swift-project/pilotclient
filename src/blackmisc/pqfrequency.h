/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQFREQUENCY_H
#define BLACKMISC_PQFREQUENCY_H
#include "pqphysicalquantity.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        /*!
         * \brief Physical unit frequency
         */
        class CFrequency : public CPhysicalQuantity<CFrequencyUnit, CFrequency>
        {
        public:
            /*!
             * \brief Default constructor
             */
            CFrequency() : CPhysicalQuantity(0, CFrequencyUnit::defaultUnit()) {}

            /*!
             * \brief Init by double value
             * \param value
             * \param unit
             */
            CFrequency(double value, const CFrequencyUnit &unit) : CPhysicalQuantity(value, unit) {}

            /*!
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Virtual destructor
             */
            virtual ~CFrequency() {}
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CFrequency)

#endif // BLACKMISC_PQFREQUENCY_H
