// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_TIMESTAMPBASED_H
#define SWIFT_MISC_TIMESTAMPBASED_H

#include <QDateTime>
#include <QString>
#include <QVariant>
#include <QtGlobal>

#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    //! Entity with timestamp
    class SWIFT_MISC_EXPORT ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexUtcTimestamp = CPropertyIndexRef::GlobalIndexITimestampBased,
            IndexUtcTimestampFormattedYmdhms,
            IndexUtcTimestampFormattedYmdhmsz,
            IndexUtcTimestampFormattedMdhms,
            IndexUtcTimestampFormattedMdhmsz,
            IndexUtcTimestampFormattedDhms,
            IndexUtcTimestampFormattedHms,
            IndexUtcTimestampFormattedHm,
            IndexMSecsSinceEpoch // keep this as last item
        };

        //! Get timestamp
        QDateTime getUtcTimestamp() const;

        //! Timestamp as ms value
        qint64 getMSecsSinceEpoch() const { return m_timestampMSecsSinceEpoch; }

        //! Time difference in ms
        qint64 getTimeDifferenceMs(qint64 compareTime) const { return compareTime - this->getMSecsSinceEpoch(); }

        //! Time difference in ms
        qint64 getTimeDifferenceAbsMs(qint64 compareTime) const
        {
            return qAbs(compareTime - this->getMSecsSinceEpoch());
        }

        //! Time difference to now
        qint64 getTimeDifferenceToNowMs() const
        {
            return this->getTimeDifferenceMs(QDateTime::currentMSecsSinceEpoch());
        }

        //! Time difference in ms
        qint64 getTimeDifferenceMs(const ITimestampBased &compare) const
        {
            return compare.getMSecsSinceEpoch() - this->getMSecsSinceEpoch();
        }

        //! Time difference in ms
        qint64 getAbsTimeDifferenceMs(qint64 compareTime) const { return qAbs(this->getTimeDifferenceMs(compareTime)); }

        //! Time difference in ms
        qint64 getAbsTimeDifferenceMs(const ITimestampBased &compare) const
        {
            return qAbs(this->getTimeDifferenceMs(compare));
        }

        //! Set to null
        void setTimestampToNull();

        //! Timestamp as ms value
        void setMSecsSinceEpoch(qint64 mSecsSinceEpoch) { m_timestampMSecsSinceEpoch = mSecsSinceEpoch; }

        //! Set by value such as "20141003231045"
        void setByYearMonthDayHourMinute(const QString &yyyyMMddhhmmss);

        //! Set timestamp
        void setUtcTimestamp(const QDateTime &timestamp);

        //! Is this newer than other?
        bool isNewerThan(const ITimestampBased &otherTimestampObj) const;

        //! Is newer than epoch value?
        bool isNewerThan(qint64 mSecsSinceEpoch) const;

        //! Is this older than other?
        bool isOlderThan(const ITimestampBased &otherTimestampObj) const;

        //! Is this older than other?
        bool isOlderThan(qint64 mSecsSinceEpoch) const;

        //! Older than now-offset
        bool isOlderThanNowMinusOffset(int offsetMs) const;

        //! Same timestamp
        bool isSame(const ITimestampBased &otherTimestampObj) const;

        //! Time difference
        qint64 msecsTo(const ITimestampBased &otherTimestampObj) const;

        //! Time difference
        qint64 absMsecsTo(const ITimestampBased &otherTimestampObj) const;

        //! Milliseconds to now
        qint64 msecsToNow() const;

        //! Set the current time as timestamp
        void setCurrentUtcTime();

        //! Add the given number of milliseconds to the timestamp.
        void addMsecs(qint64 ms);

        //! As hh:mm:ss
        QString getFormattedUtcTimestampHms() const;

        //! As hh:mm:ss.zzz
        QString getFormattedUtcTimestampHmsz() const;

        //! As hh:mm
        QString getFormattedUtcTimestampHm() const;

        //! As dd HH mm ss
        QString getFormattedUtcTimestampDhms() const;

        //! As MM dd HH mm ss
        QString getFormattedUtcTimestampMdhms() const;

        //! As MM dd HH mm ss.zzz
        QString getFormattedUtcTimestampMdhmsz() const;

        //! As yyyy MM dd HH mm ss
        QString getFormattedUtcTimestampYmdhms() const;

        //! As yyyy MM dd HH mm ss.zzz
        QString getFormattedUtcTimestampYmdhmsz() const;

        //! Valid timestamp?
        bool hasValidTimestamp() const;

        //! Any of the timestamp indexes
        static bool isAnyTimestampIndex(int index);

        //! Can given index be handled
        static bool canHandleIndex(CPropertyIndexRef index);

    protected:
        //! Constructor
        ITimestampBased() = default;

        //! Constructor
        ITimestampBased(qint64 msSincePoch);

        //! Constructor
        ITimestampBased(const QDateTime &timestamp);

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const ITimestampBased &compareValue) const;

        //! Update missing parts
        void updateMissingParts(const ITimestampBased &other);

        qint64 m_timestampMSecsSinceEpoch = -1; //!< timestamp value
    };

    //! Entity with timestamp
    class SWIFT_MISC_EXPORT ITimestampWithOffsetBased : public ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexOffsetMs = static_cast<int>(CPropertyIndexRef::GlobalIndexITimestampBased) +
                            ITimestampBased::IndexMSecsSinceEpoch + 1,
            IndexAdjustedMsWithOffset,
            IndexOffsetWithUnit // keep this as last item
        };

        //! Milliseconds to add to timestamp for interpolation
        void setTimeOffsetMs(qint64 offset) { m_timeOffsetMs = offset; }

        //! Milliseconds to add to timestamp for interpolation
        qint64 getTimeOffsetMs() const { return m_timeOffsetMs; }

        //! Having a valid offset time
        bool hasNonZeroOffsetTime() const;

        //! Adds a value to offset time
        void addMsecsToOffsetTime(qint64 msToAdd);

        //! Offset with unit
        QString getTimeOffsetWithUnit() const;

        //! Timestamp with offset added for interpolation
        qint64 getAdjustedMSecsSinceEpoch() const { return this->getMSecsSinceEpoch() + this->getTimeOffsetMs(); }

        //! Set timestamp with offset added for interpolation
        void setAdjustedMSecsSinceEpoch(qint64 adjustedTimeMs)
        {
            this->setMSecsSinceEpoch(adjustedTimeMs - this->getTimeOffsetMs());
        }

        //! Time difference in ms (this -> compare)
        qint64 getAdjustedTimeDifferenceMs(qint64 compareTime) const
        {
            return this->getAdjustedMSecsSinceEpoch() - compareTime;
        }

        //! Timestamp and offset
        QString getTimestampAndOffset(bool formatted) const;

        //! Timestamp and offset
        QString getFormattedTimestampAndOffset(bool includeRawTimestamp) const;

        //! Is this newer than other?
        bool isNewerThanAdjusted(const ITimestampWithOffsetBased &otherTimestampObj) const;

        //! Is newer than epoch value?
        bool isNewerThanAdjusted(qint64 mSecsSinceEpoch) const;

        //! Is this older than other?
        bool isOlderThanAdjusted(const ITimestampWithOffsetBased &otherTimestampObj) const;

        //! Is this older than other?
        bool isOlderThanAdjusted(qint64 mSecsSinceEpoch) const;

    protected:
        //! Can given index be handled
        static bool canHandleIndex(CPropertyIndexRef index);

        //! Constructor
        ITimestampWithOffsetBased() = default;

        //! Constructor
        ITimestampWithOffsetBased(qint64 msSincePoch) : ITimestampBased(msSincePoch) {}

        //! Constructor
        ITimestampWithOffsetBased(const QDateTime &timestamp) : ITimestampBased(timestamp) {}

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const ITimestampWithOffsetBased &compareValue) const;

        qint64 m_timeOffsetMs = 0; //!< offset time in ms
    };
} // namespace swift::misc

#endif // SWIFT_MISC_TIMESTAMPBASED_H
