/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TIMESTAMPOBJECTLIST_H
#define BLACKMISC_TIMESTAMPOBJECTLIST_H

#include "blackmisc/timestampbased.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/predicates.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"
#include <QList>
#include <QtGlobal>

class QDateTime;

namespace BlackMisc
{
    //! Milliseconds minimum/maximum/mean
    struct MillisecondsMinMaxMean
    {
        qint64 min;  //!< Minimum
        qint64 max;  //!< Maximum
        double mean; //!< Mean (average)

        //! Valid?
        bool isValid() const { return min >= 0 && max >= 0; }

        //! Reset the values
        void reset() { min = -1; max = -1; mean = -1;}

        //! As string
        QString asString() const { return QStringLiteral("Min: %1ms Max: %2ms Mean: %3ms").arg(min).arg(max).arg(mean, 0, 'f', 2); }
    };

    //! List of objects with timestamp.
    //! Such objects should implement \sa ITimestampBased
    template<class OBJ, class CONTAINER> class ITimestampObjectList
    {
        static_assert(std::is_base_of_v<ITimestampBased, OBJ>, "OBJ needs to implement ITimestampBased");

    public:
        //! Hint if the list is sorted
        enum HintTimestampSort
        {
            NoTimestampSortHint,
            TimestampLatestFirst,
            TimestampLatestLast
        };

        //! List of objects before dateTime (older)
        CONTAINER findBefore(const QDateTime &dateTime) const
        {
            return this->findBefore(dateTime.toMSecsSinceEpoch());
        }

        //! List of objects before msSinceEpoch (older)
        CONTAINER findBefore(qint64 msSinceEpoch) const
        {
            return this->container().findBy([&](const OBJ & obj)
            {
                return obj.isOlderThan(msSinceEpoch);
            });
        }

        //! Object before timestamp or default (older)
        OBJ findObjectBeforeOrDefault(qint64 msSinceEpoch) const
        {
            const CONTAINER before = this->findBefore(msSinceEpoch);
            if (before.isEmpty()) { return OBJ(); }
            return before.latestObject();
        }

        //! Get objects before msSinceEpoch and remove those
        CONTAINER findBeforeAndRemove(qint64 msSinceEpoch)
        {
            CONTAINER result(findBefore(msSinceEpoch));
            this->removeBefore(msSinceEpoch);
            return result;
        }

        //! List of objects before now - offset
        CONTAINER findBeforeNowMinusOffset(qint64 msOffset) const
        {
            return this->findBefore(QDateTime::currentMSecsSinceEpoch() - msOffset);
        }

        //! List of objects after dateTime (newer)
        CONTAINER findAfter(const QDateTime &dateTime) const
        {
            return this->findAfter(dateTime.toMSecsSinceEpoch());
        }

        //! List of objects after msSinceEpoch (newer)
        CONTAINER findAfter(qint64 msSinceEpoch) const
        {
            return this->container().findBy([&](const OBJ & obj)
            {
                return obj.isNewerThan(msSinceEpoch);
            });
        }

        //! List of objects before now - offset
        CONTAINER findAfterNowMinusOffset(qint64 msOffset) const
        {
            return this->findAfter(QDateTime::currentMSecsSinceEpoch() - msOffset);
        }

        //! List of objects after msSinceEpoch (newer)
        OBJ findObjectAfterOrDefault(qint64 msSinceEpoch) const
        {
            const CONTAINER after = this->findAfter(msSinceEpoch);
            if (after.isEmpty()) { return OBJ(); }
            return after.oldestObject();
        }

        //! Objects without valid timestamp
        CONTAINER findInvalidTimestamps() const
        {
            return this->container().findBy([&](const OBJ & obj)
            {
                return !obj.hasValidTimestamp();
            });
        }

        //! Find closest (or default)
        OBJ findClosestTimeDistance(qint64 msSinceEpoch) const
        {
            if (this->container().isEmpty()) { return OBJ(); }
            const auto closest = std::min_element(this->container().cbegin(), this->container().cend(), [=](const ITimestampBased & a, const ITimestampBased & b)
            {
                return qAbs(a.getTimeDifferenceMs(msSinceEpoch)) < qAbs(b.getTimeDifferenceMs(msSinceEpoch));
            });
            return *closest;
        }

        //! Has invalid timestamp
        bool hasInvalidTimestamps() const
        {
            return this->container().contains(&OBJ::hasValidTimestamp, false);
        }

        //! Set all timestamps to now
        void setCurrentUtcTime()
        {
            for (ITimestampBased &tsObj : this->container())
            {
                tsObj.setCurrentUtcTime();
            }
        }

        //! Set all timestamps to given time
        void setUtcTime(qint64 msSinceEpoch)
        {
            for (ITimestampBased &tsObj : this->container())
            {
                tsObj.setMSecsSinceEpoch(msSinceEpoch);
            }
        }

        //! Set invalid timestamps to now
        void setInvalidTimestampsToCurrentUtcTime()
        {
            for (ITimestampBased &tsObj : this->container())
            {
                if (tsObj.hasValidTimestamp()) { continue; }
                tsObj.setCurrentUtcTime();
            }
        }

        //! Latest timestamp
        QDateTime latestTimestamp() const
        {
            if (this->container().isEmpty()) { return QDateTime(); }
            return this->latestObject().getUtcTimestamp();
        }

        //! Latest timestamp
        qint64 latestTimestampMsecsSinceEpoch() const
        {
            const QDateTime dt(latestTimestamp());
            return dt.isValid() ? dt.toMSecsSinceEpoch() : -1;
        }

        //! Oldest timestamp
        QDateTime oldestTimestamp() const
        {
            if (this->container().isEmpty()) { return QDateTime(); }
            return this->oldestObject().getUtcTimestamp();
        }

        //! Oldest timestamp
        qint64 oldestTimestampMsecsSinceEpoch() const
        {
            const QDateTime dt(oldestTimestamp());
            return dt.isValid() ? dt.toMSecsSinceEpoch() : -1;
        }

        //! Latest object
        OBJ latestObject() const
        {
            if (this->container().isEmpty()) { return OBJ(); }
            const auto latest = std::max_element(this->container().begin(), this->container().end(), [](const OBJ & a, const OBJ & b) { return a.getMSecsSinceEpoch() < b.getMSecsSinceEpoch(); });
            return *latest;
        }

        //! Latest object
        OBJ oldestObject() const
        {
            if (this->container().isEmpty()) { return OBJ(); }
            const auto oldest = std::min_element(this->container().begin(), this->container().end(), [](const OBJ & a, const OBJ & b) { return a.getMSecsSinceEpoch() < b.getMSecsSinceEpoch(); });
            return *oldest;
        }

        //! Remove objects with timestamp before dateTime
        int removeBefore(const QDateTime &dateTime)
        {
            return this->removeBefore(dateTime.toMSecsSinceEpoch());
        }

        //! Remove objects with timestamp before dateTime
        int removeBefore(qint64 msSinceEpoch)
        {
            return this->container().removeIf([&](const OBJ & obj)
            {
                return obj.isOlderThan(msSinceEpoch);
            });
        }

        //! Remove objects older than seconds
        int removeOlderThanNowMinusOffset(qint64 offsetMs)
        {
            const qint64 epoch = QDateTime::currentMSecsSinceEpoch() - offsetMs;
            return this->container().removeIf([&](const OBJ & obj)
            {
                return obj.isOlderThan(epoch);
            });
        }

        //! Sort by timestamp
        void sortLatestFirst()
        {
            this->container().sortOldestFirst();
            this->container().reverse();
        }

        //! Sort by timestamp
        void sortOldestFirst()
        {
            this->container().sort(Predicates::MemberLess(&OBJ::getMSecsSinceEpoch));
        }

        //! Insert as first element by keeping maxElements and the latest first
        void push_frontKeepLatestFirst(const OBJ &value, bool replaceSameTimestamp = true, int maxElements = -1)
        {
            Q_ASSERT_X(maxElements < 0 || maxElements > 1, Q_FUNC_INFO, "Max.value wrong range");
            CONTAINER &c = this->container();
            if (replaceSameTimestamp && !c.isEmpty() && c[0].getMSecsSinceEpoch() == value.getMSecsSinceEpoch())
            {
                c[0] = value;
                if (maxElements > 0) { c.truncate(maxElements); }
            }
            else
            {
                if (maxElements > 0) { c.truncate(maxElements - 1); }
                const bool needSort = !c.isEmpty() && value.isOlderThan(c.front());
                c.push_front(value);
                if (needSort)
                {
                    ITimestampObjectList::sortLatestFirst();
                }
            }

            // crosscheck
            if (BlackConfig::CBuildConfig::isLocalDeveloperDebugBuild())
            {
                Q_ASSERT_X(this->isSortedLatestFirst(), Q_FUNC_INFO, "Wrong sort order");
            }
        }

        //! Push back and increase the timestamp at least by +1ms if equal to last element
        //! \remark if the timestamp is already greater it does not modifcation
        void push_backIncreaseTimestamp(const OBJ &newObject)
        {
            if (this->container().isEmpty())
            {
                this->container().push_back(newObject);
                return;
            }
            const qint64 newMs = newObject.getMSecsSinceEpoch();
            const qint64 oldMs = this->container().back().getMSecsSinceEpoch();
            if (newMs > oldMs)
            {
                this->container().push_back(newObject);
                return;
            }
            this->push_backOverrideTimestamp(newObject, oldMs + 1);
        }

        //! Push back, but set new timestamp
        void push_backOverrideTimestamp(const OBJ &newObject, qint64 newTsMsSinceEpoch)
        {
            OBJ newObjectCopy(newObject);
            newObjectCopy.setMSecsSinceEpoch(newTsMsSinceEpoch);
            this->container().push_back(newObjectCopy);
        }

        //! Set new timestamps starting with the last element
        void setNewTimestampStartingLast(qint64 startTimeStampMs, qint64 deltaTimeMs)
        {
            if (this->container().isEmpty()) { return; }
            qint64 currentMs = startTimeStampMs;
            for (auto it = this->container().rbegin(); it != this->container().rend(); ++it)
            {
                it->setMSecsSinceEpoch(currentMs);
                currentMs += deltaTimeMs;
            }
        }

        //! Replace if an object has the same timestamp
        int replaceIfSameTimestamp(const OBJ &newObject)
        {
            int c = 0;
            for (OBJ &obj : this->container())
            {
                if (obj.getMSecsSinceEpoch() == newObject.getMSecsSinceEpoch())
                {
                    obj = newObject;
                    c++;
                }
            }
            return c;
        }

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedLatestLast() const
        {
            if (this->container().size() < 2) { return true; }
            qint64 max = -1;
            for (const ITimestampBased &obj : this->container())
            {
                if (!obj.hasValidTimestamp()) { return false; }
                if (obj.getMSecsSinceEpoch() < max) { return false; }
                max = obj.getMSecsSinceEpoch();
            }
            return true;
        }

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedLatestFirst() const
        {
            if (this->container().size() < 2) { return true; }
            qint64 min = std::numeric_limits <qint64>::max();
            for (const ITimestampBased &obj : this->container())
            {
                if (!obj.hasValidTimestamp()) { return false; }
                if (obj.getMSecsSinceEpoch() > min) { return false; }
                min = obj.getMSecsSinceEpoch();
            }
            return true;
        }

        //! Adds a time to all values
        void addMsecs(qint64 msToAdd)
        {
            if (msToAdd == 0) { return; }
            for (ITimestampBased &obj : this->container())
            {
                obj.addMsecs(msToAdd);
            }
        }

        //! Set the hint
        void setSortHint(HintTimestampSort hint)
        {
            m_tsSortHint = hint;
        }

        //! Difference of timestamp values
        //! \pre timestamp list has to be sorted to get meaningful values
        MillisecondsMinMaxMean getTimestampDifferenceMinMaxMean() const
        {
            MillisecondsMinMaxMean mmm;
            mmm.reset();
            const CONTAINER &container = this->container();
            if (container.size() < 2) { return mmm; }

            // Do not confuse with adjusted sort hint!
            if (container.m_tsSortHint == NoTimestampSortHint)
            {
                CONTAINER copy(container);
                copy.sortLatestFirst();
                copy.m_tsSortHint = TimestampLatestFirst;
                return copy.getTimestampDifferenceMinMaxMean();
            }

            mmm.max = std::numeric_limits<qint64>::min();
            mmm.min = std::numeric_limits<qint64>::max();
            qint64 mean = 0;
            int c = 0;
            OBJ last;

            for (const OBJ &object : container)
            {
                if (c > 0)
                {
                    const ITimestampBased &l = last;
                    const ITimestampBased &o = object;
                    const qint64 diff = l.getAbsTimeDifferenceMs(o);
                    if (diff > mmm.max) { mmm.max = diff; }
                    if (diff < mmm.min) { mmm.min = diff; }
                    mean += diff;
                }
                c++;
                last = object;
            }

            mmm.mean = mean / c;
            return mmm;
        }

    protected:
        //! Constructor
        ITimestampObjectList() = default;

        //! Container
        const CONTAINER &container() const
        {
            return static_cast<const CONTAINER &>(*this);
        }

        //! Container
        CONTAINER &container()
        {
            return static_cast<CONTAINER &>(*this);
        }

        HintTimestampSort m_tsSortHint = NoTimestampSortHint; //!< sort hint
    };

    //! List of objects with timestamp and offset.
    //! Such objects should implement \sa ITimestampWithOffsetBased
    template<class OBJ, class CONTAINER> class ITimestampWithOffsetObjectList : public ITimestampObjectList<OBJ, CONTAINER>
    {
        static_assert(std::is_base_of_v<ITimestampWithOffsetBased, OBJ>, "OBJ needs to implement ITimestampBased");

    public:
        //! Hint if the list is sorted
        enum HintAdjustedTimestampSort
        {
            NoAdjustedTimestampSortHint,
            AdjustedTimestampLatestFirst
        };

        //! Sort by adjusted timestamp
        void sortAdjustedLatestFirst()
        {
            this->container().sortAdjustedOldestFirst();
            this->container().reverse();
        }

        //! As sorted copy
        CONTAINER getSortedAdjustedLatestFirst() const
        {
            CONTAINER copy(this->container());
            copy.sortAdjustedLatestFirst();
            return copy;
        }

        //! Get the latest 2 values
        CONTAINER getLatestAdjustedTwoObjects(bool alreadySortedLatestFirst = false) const
        {
            if (this->container().size() < 2) { return CONTAINER(); }
            CONTAINER copy(alreadySortedLatestFirst ? this->container() : this->container().getSortedAdjustedLatestFirst());
            copy.truncate(2);
            return copy;
        }

        //! Sort by adjusted timestamp
        void sortAdjustedOldestFirst()
        {
            this->container().sort(Predicates::MemberLess(&OBJ::getAdjustedMSecsSinceEpoch));
        }

        //! Any negative or zero offset time?
        bool containsZeroOrNegativeOffsetTime() const
        {
            for (const ITimestampWithOffsetBased &obj : this->container())
            {
                if (obj.getTimeOffsetMs() <= 0) { return true; }
            }
            return false;
        }

        //! Any negative offset time?
        bool containsNegativeOffsetTime() const
        {
            for (const ITimestampWithOffsetBased &obj : this->container())
            {
                if (obj.getTimeOffsetMs() < 0) { return true; }
            }
            return false;
        }

        //! Adds a time to all offset values
        void addMsecsToOffset(qint64 msToAdd)
        {
            for (ITimestampWithOffsetBased &obj : this->container())
            {
                obj.addMsecsToOffsetTime(msToAdd);
            }
        }

        //! Insert as first element by keeping maxElements and the latest first
        void push_frontKeepLatestAdjustedFirst(const OBJ &value, bool replaceSameTimestamp = true, int maxElements = -1)
        {
            Q_ASSERT_X(maxElements < 0 || maxElements > 1, Q_FUNC_INFO, "Max.value wrong range");
            CONTAINER &c = this->container();
            if (replaceSameTimestamp && !c.isEmpty() && c[0].getMSecsSinceEpoch() == value.getMSecsSinceEpoch())
            {
                c[0] = value;
                if (maxElements > 0) { c.truncate(maxElements); }
                return;
            }

            if (maxElements > 0) { c.truncate(maxElements - 1); }
            const bool needSort = !c.isEmpty() && value.isOlderThanAdjusted(c.front());
            c.push_front(value);
            if (needSort)
            {
                ITimestampWithOffsetObjectList::sortAdjustedLatestFirst();
            }
        }

        //! Insert as first element by keeping maxElements and the latest first
        //! \remark adjust offset to average offset of two adjacent elements so adjusted values are sorted
        void push_frontKeepLatestFirstAdjustOffset(const OBJ &value, bool replaceSameTimestamp = true, int maxElements = -1)
        {
            ITimestampWithOffsetObjectList<OBJ, CONTAINER>::push_frontKeepLatestFirst(value, replaceSameTimestamp, maxElements);

            // now sorted by timestamp
            // this reflects normally the incoming order
            // ts
            //  8: os 2 adj 12 => min os 4
            //  6: os 2 adj 11 => min os 5
            //  5: os 5 adj 10
            //  0: os 5 adj  5
            CONTAINER &c = this->container();
            if (c.size() < 2) { return; }
            ITimestampWithOffsetBased &front = c.front();
            const ITimestampWithOffsetBased &second = c[1];
            if (!front.isNewerThanAdjusted(second))
            {
                // const qint64 minOs = qMin(front.getTimeOffsetMs(), second.getTimeOffsetMs());
                const qint64 minReqOs = second.getAdjustedMSecsSinceEpoch() - front.getMSecsSinceEpoch(); // minimal required
                const qint64 avgOs = (front.getTimeOffsetMs() + second.getTimeOffsetMs()) / 2;
                const qint64 os = qMax(minReqOs + 1, avgOs); // at least +1, as value must be > (greater)
                front.setTimeOffsetMs(os);
            }

            if (BlackConfig::CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(front.isNewerThanAdjusted(second), Q_FUNC_INFO, "Front/second timestamp");
                BLACK_VERIFY_X(this->isSortedAdjustedLatestFirst(), Q_FUNC_INFO, "Wrong sort order");
            }
        }

        //! Add value, but ignore overlapping (past) values
        void push_frontKeepLatestFirstIgnoreOverlapping(const OBJ &value, bool replaceSameTimestamp = true, int maxElements = -1)
        {
            CONTAINER &c = this->container();
            if (c.size() > 1)
            {
                const ITimestampWithOffsetBased front = c.front();
                if (value.getAdjustedMSecsSinceEpoch() <= front.getAdjustedMSecsSinceEpoch()) { return; }
            }
            ITimestampWithOffsetObjectList<OBJ, CONTAINER>::push_frontKeepLatestFirst(value, replaceSameTimestamp, maxElements);
        }

        //! Prefill with elements
        void prefillLatestAdjustedFirst(const OBJ &value, int elements, qint64 deltaTimeMs = -1)
        {
            this->container().clear();
            const qint64 osTime = value.getTimeOffsetMs();
            const qint64 os = -1 * qAbs(deltaTimeMs < 0 ? osTime : deltaTimeMs);
            if (BlackConfig::CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(os < 0, Q_FUNC_INFO, "Need negative offset time to prefill time");
            }
            this->container().push_front(value);
            for (int i = 1; i < elements; i++)
            {
                OBJ copy(value);
                copy.addMsecs(os * i);
                this->container().push_back(copy);
            }
            this->setAdjustedSortHint(ITimestampWithOffsetObjectList::AdjustedTimestampLatestFirst);
        }

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedAdjustedLatestLast() const
        {
            if (this->container().isEmpty()) { return false; }
            if (this->container().size() < 2) { return true; }
            qint64 max = -1;
            for (const ITimestampWithOffsetBased &obj : this->container())
            {
                if (!obj.hasValidTimestamp()) { return false; }
                if (obj.getAdjustedMSecsSinceEpoch() < max) { return false; }
                max = obj.getAdjustedMSecsSinceEpoch();
            }
            return true;
        }

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedAdjustedLatestFirst() const
        {
            if (this->container().size() < 2) { return true; }
            qint64 min = std::numeric_limits <qint64>::max();
            for (const ITimestampWithOffsetBased &obj : this->container())
            {
                if (!obj.hasValidTimestamp()) { return false; }
                if (obj.getAdjustedMSecsSinceEpoch() > min) { return false; }
                min = obj.getAdjustedMSecsSinceEpoch();
            }
            return true;
        }

        //! List of objects after msSinceEpoch (newer)
        CONTAINER findAfterAdjusted(qint64 msSinceEpoch) const
        {
            return this->container().findBy([&](const ITimestampWithOffsetBased & obj)
            {
                return obj.isNewerThanAdjusted(msSinceEpoch);
            });
        }

        //! List of objects after msSinceEpoch (newer)
        OBJ findObjectAfterAdjustedOrDefault(qint64 msSinceEpoch) const
        {
            const CONTAINER after = this->findAfterAdjusted(msSinceEpoch);
            if (after.isEmpty()) { return OBJ(); }
            return after.oldestAdjustedObject();
        }

        //! List of objects before msSinceEpoch (older)
        CONTAINER findBeforeAdjusted(qint64 msSinceEpoch) const
        {
            return this->container().findBy([&](const ITimestampWithOffsetBased & obj)
            {
                return obj.isOlderThanAdjusted(msSinceEpoch);
            });
        }

        //! Object before timestamp (older)
        OBJ findObjectBeforeAdjustedOrDefault(qint64 msSinceEpoch) const
        {
            const CONTAINER before = this->findBeforeAdjusted(msSinceEpoch);
            if (before.isEmpty()) { return OBJ(); }
            return before.latestAdjustedObject();
        }

        //! Closest adjusted time difference
        OBJ findClosestTimeDistanceAdjusted(qint64 msSinceEpoch) const
        {
            if (this->container().isEmpty()) { return OBJ(); }
            const auto closest = std::min_element(this->container().cbegin(), this->container().cend(), [=](const ITimestampWithOffsetBased & a, const ITimestampWithOffsetBased & b)
            {
                return qAbs(a.getAdjustedTimeDifferenceMs(msSinceEpoch)) < qAbs(b.getAdjustedTimeDifferenceMs(msSinceEpoch));
            });
            return *closest;
        }

        //! Latest adjusted object
        OBJ latestAdjustedObject() const
        {
            if (this->container().isEmpty()) { return OBJ(); }
            if (this->container().m_tsAdjustedSortHint == AdjustedTimestampLatestFirst)
            {
                return this->container().front();
            }
            const auto latest = std::max_element(this->container().begin(), this->container().end(), [](const OBJ & a, const OBJ & b) { return a.getAdjustedMSecsSinceEpoch() < b.getAdjustedMSecsSinceEpoch(); });
            return *latest;
        }

        //! Oldest adjusted object
        OBJ oldestAdjustedObject() const
        {
            if (this->container().isEmpty()) { return OBJ(); }
            if (this->container().m_tsAdjustedSortHint == AdjustedTimestampLatestFirst)
            {
                return this->container().back();
            }
            const auto oldest = std::min_element(this->container().begin(), this->container().end(), [](const OBJ & a, const OBJ & b) { return a.getAdjustedMSecsSinceEpoch() < b.getAdjustedMSecsSinceEpoch(); });
            return *oldest;
        }

        //! Latest adjusted timestamp
        QDateTime latestAdjustedTimestamp() const
        {
            if (this->container().isEmpty()) { return QDateTime(); }
            return this->latestAdjustedObject().getUtcTimestamp();
        }

        //! Oldest adjusted timestamp
        QDateTime oldestAdjustedTimestamp() const
        {
            if (this->container().isEmpty()) { return QDateTime(); }
            return this->oldestAdjustedObject().getUtcTimestamp();
        }

        //! Latest adjusted timestamp
        qint64 latestAdjustedTimestampMsecsSinceEpoch() const
        {
            if (this->container().isEmpty()) { return -1; }
            const QDateTime dt(this->latestAdjustedTimestamp());
            return dt.isValid() ? dt.toMSecsSinceEpoch() : -1;
        }

        //! Oldest adjusted timestamp
        qint64 oldestAdjustedTimestampMsecsSinceEpoch() const
        {
            if (this->container().isEmpty()) { return -1; }
            const QDateTime dt(oldestAdjustedTimestamp());
            return dt.isValid() ? dt.toMSecsSinceEpoch() : -1;
        }

        //! Set the hint
        void setAdjustedSortHint(HintAdjustedTimestampSort hint)
        {
            this->container().m_tsAdjustedSortHint = hint;
        }

        //! Difference of timestamp values
        //! \pre timestamp list has to be sorted to get meaningful values
        MillisecondsMinMaxMean getOffsetMinMaxMean() const
        {
            MillisecondsMinMaxMean mmm;
            mmm.reset();
            const CONTAINER &container = this->container();
            if (container.size() < 1) { return mmm; }

            mmm.max = std::numeric_limits<qint64>::min();
            mmm.min = std::numeric_limits<qint64>::max();
            qint64 mean = 0;
            int c = 0;

            for (const ITimestampWithOffsetBased &object : container)
            {
                if (!object.hasNonZeroOffsetTime()) { continue; }
                const qint64 os = object.getTimeOffsetMs();
                if (os > mmm.max) { mmm.max = os; }
                if (os < mmm.min) { mmm.min = os; }
                mean += os;
                c++;
            }

            if (c > 0) { mmm.mean = mean / c; }
            return mmm;
        }

    protected:
        //! Constructor
        ITimestampWithOffsetObjectList() = default;

        HintAdjustedTimestampSort m_tsAdjustedSortHint = NoAdjustedTimestampSortHint; //!< sort hint
    };
} //namespace

#endif //guard
