/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_TIME_H
#define BLACKMISC_PQ_TIME_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/pq/units.h"

#include <QList>
#include <QMetaType>
#include <QString>
#include <QTime>
#include <QDateTime>

namespace BlackMisc::PhysicalQuantities
{
    /*!
     * Time class, e.g. "ms", "hour", "s", "day"
     */
    class BLACKMISC_EXPORT CTime : public CPhysicalQuantity<CTimeUnit, CTime>
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
        CTime(QTime time, bool negative = false);

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CTime(const QString &unitString) : CPhysicalQuantity(0, CTimeUnit::nullUnit()) { this->parseFromString(unitString); }

        //! From string hh:mm, or hh:mm:ss, or time units such as s, min
        void parseFromString(const QString &time);

        //! From string like 2211 (hhmm)
        bool parseFromString_hhmm(const QString &hhmm);

        //! From string hh:mm, or hh:mm:ss, or time units such as s, min
        void parseFromString(const QString &time, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode);

        //! Parts hh, mm, ss
        //! \warning sign not considered
        QList<int> getHrsMinSecParts() const;

        //! Formatted as hh:mm:ss
        QString formattedHrsMinSec() const;

        //! Convenience functions when used with timers etc.
        int toMs() const;

        //! Convenience functions when used with timers etc.
        int toSeconds() const;

        //! Formatted as hh:mm
        QString formattedHrsMin() const;

        //! To Qt date time
        QDateTime toQDateTime() const;

        //! To Qt time
        QTime toQTime() const;
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTime)

#endif // guard
