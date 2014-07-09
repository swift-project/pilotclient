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

            //! Parts
            enum Parts
            {
                Hours = 0,
                Minutes,
                Seconds
            };

            //! Default constructor
            CTime() : CPhysicalQuantity(0, CTimeUnit::defaultUnit()) {}

            //! Init by double value
            CTime(double value, const CTimeUnit &unit) : CPhysicalQuantity(value, unit) {}

            //! By hours, minutes, seconds
            CTime(int hours, int minutes, int seconds = 0);

            //! By Qt time
            CTime(const QTime &time, bool negative = false);

            //! \copydoc CPhysicalQuantity(const QString &unitString)
            CTime(const QString &unitString) : CPhysicalQuantity(0, CTimeUnit::nullUnit()) { this->parseFromString(unitString); }

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Destructor
            virtual ~CTime() {}

            //! From string hh:mm, or hh:mm:ss, or time units such as s, min
            virtual void parseFromString(const QString &time) override;

            //! To Qt time
            //! \warning sign not considered
            QTime toQTime() const;

            //! Parts hh, mm, ss
            //! \warning sign not considered
            QList<int> getHrsMinSecParts() const;

            //! Formatted as hh:mm:ss
            QString formattedHrsMinSec() const;

            //! Formatted as hh:mm
            QString formattedHrsMin() const;

        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTime)

#endif // guard
