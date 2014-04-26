/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQTIME_H
#define BLACKMISC_PQTIME_H

#include "pqphysicalquantity.h"

namespace BlackMisc
{
    namespace PhysicalQuantities
    {

        /*!
         * \brief Time class, e.g. "ms", "hour", "s", "day"
         */
        class CTime : public CPhysicalQuantity<CTimeUnit, CTime>
        {
        public:
            //! Default constructor
            CTime() : CPhysicalQuantity(0, CTimeUnit::defaultUnit()) {}

            //! Init by double value
            CTime(double value, const CTimeUnit &unit) : CPhysicalQuantity(value, unit) {}

            //! By Qt time
            CTime(const QTime &time);

            //! By string hh:mm or hh:mm:ss
            CTime(const QString &time);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Destructor
            virtual ~CTime() {}
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTime)

#endif // BLACKMISC_PQTIME_H
