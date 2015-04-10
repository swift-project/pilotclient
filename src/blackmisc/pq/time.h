/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_TIME_H
#define BLACKMISC_PQ_TIME_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc
{

    //! \private
    template <> struct CValueObjectPolicy<PhysicalQuantities::CTime> : public CValueObjectPolicy<>
    {
        using MetaType = Policy::MetaType::DefaultAndQList;
    };

    namespace PhysicalQuantities
    {

        /*!
         * Time class, e.g. "ms", "hour", "s", "day"
         */
        class BLACKMISC_EXPORT CTime : public CValueObject<CTime, CPhysicalQuantity<CTimeUnit, CTime>>
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
            CTime() : CValueObject(0, CTimeUnit::defaultUnit()) {}

            //! Init by double value
            CTime(double value, const CTimeUnit &unit) : CValueObject(value, unit) {}

            //! By hours, minutes, seconds
            CTime(int hours, int minutes, int seconds = 0);

            //! By Qt time
            CTime(const QTime &time, bool negative = false);

            //! \copydoc CPhysicalQuantity(const QString &unitString)
            CTime(const QString &unitString) : CValueObject(0, CTimeUnit::nullUnit()) { this->parseFromString(unitString); }

            //! From string hh:mm, or hh:mm:ss, or time units such as s, min
            virtual void parseFromString(const QString &time) override;

            //! From string hh:mm, or hh:mm:ss, or time units such as s, min
            virtual void parseFromString(const QString &time, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode) override;

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

    }
}

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTime)

#endif // guard
