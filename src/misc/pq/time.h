// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_TIME_H
#define SWIFT_MISC_PQ_TIME_H

#include <QDateTime>
#include <QList>
#include <QMetaType>
#include <QString>
#include <QTime>

#include "misc/pq/physicalquantity.h"
#include "misc/pq/pqstring.h"
#include "misc/pq/units.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::physical_quantities
{
    /*!
     * Time class, e.g. "ms", "hour", "s", "day"
     */
    class SWIFT_MISC_EXPORT CTime : public CPhysicalQuantity<CTimeUnit, CTime>
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
        CTime(const QString &unitString) : CPhysicalQuantity(0, CTimeUnit::nullUnit())
        {
            this->parseFromString(unitString);
        }

        //! From string hh:mm, or hh:mm:ss, or time units such as s, min
        void parseFromString(const QString &time);

        //! From string like 2211 (hhmm)
        bool parseFromString_hhmm(const QString &hhmm);

        //! From string hh:mm, or hh:mm:ss, or time units such as s, min
        void parseFromString(const QString &time, swift::misc::physical_quantities::CPqString::SeparatorMode mode);

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
} // namespace swift::misc::physical_quantities

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CTime)

#endif // SWIFT_MISC_PQ_TIME_H
