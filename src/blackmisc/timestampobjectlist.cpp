/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationchangelist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/db/dbinfolist.h"
#include "blackmisc/db/artifactlist.h"
#include "blackmisc/db/distributionlist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/rawfsdmessagelist.h"
#include "blackmisc/network/urlloglist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/matchingstatistics.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/identifierlist.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

#include <QDateTime>
#include <algorithm>
#include <limits>
#include <iterator>
#include <type_traits>
#include <numeric>

using namespace BlackConfig;

namespace BlackMisc
{
    template <class OBJ, class CONTAINER>
    ITimestampObjectList<OBJ, CONTAINER>::ITimestampObjectList()
    {
        static_assert(std::is_base_of<ITimestampBased, OBJ>::value, "OBJ needs to implement ITimestampBased");
    }

    template <class OBJ, class CONTAINER>
    const CONTAINER &ITimestampObjectList<OBJ, CONTAINER>::container() const
    {
        return static_cast<const CONTAINER &>(*this);
    }

    template <class OBJ, class CONTAINER>
    CONTAINER &ITimestampObjectList<OBJ, CONTAINER>::container()
    {
        return static_cast<CONTAINER &>(*this);
    }

    template <class OBJ, class CONTAINER>
    CONTAINER ITimestampObjectList<OBJ, CONTAINER>::findBefore(qint64 msSinceEpoch) const
    {
        return this->container().findBy([&](const OBJ & obj)
        {
            return obj.isOlderThan(msSinceEpoch);
        });
    }

    template<class OBJ, class CONTAINER>
    OBJ ITimestampObjectList<OBJ, CONTAINER>::findObjectBeforeOrDefault(qint64 msSinceEpoch) const
    {
        const CONTAINER before = this->findBefore(msSinceEpoch);
        if (before.isEmpty()) { return OBJ(); }
        return before.latestObject();
    }

    template <class OBJ, class CONTAINER>
    CONTAINER ITimestampObjectList<OBJ, CONTAINER>::findBeforeAndRemove(qint64 msSinceEpoch)
    {
        CONTAINER result(findBefore(msSinceEpoch));
        this->removeBefore(msSinceEpoch);
        return result;
    }

    template <class OBJ, class CONTAINER>
    CONTAINER ITimestampObjectList<OBJ, CONTAINER>::findBeforeNowMinusOffset(qint64 msOffset) const
    {
        return this->findBefore(QDateTime::currentMSecsSinceEpoch() - msOffset);
    }

    template <class OBJ, class CONTAINER>
    CONTAINER ITimestampObjectList<OBJ, CONTAINER>::findBefore(const QDateTime &dateTime) const
    {
        return this->findBefore(dateTime.toMSecsSinceEpoch());
    }

    template <class OBJ, class CONTAINER>
    CONTAINER ITimestampObjectList<OBJ, CONTAINER>::findAfter(qint64 msSinceEpoc) const
    {
        return this->container().findBy([&](const OBJ & obj)
        {
            return obj.isNewerThan(msSinceEpoc);
        });
    }

    template<class OBJ, class CONTAINER>
    CONTAINER ITimestampObjectList<OBJ, CONTAINER>::findAfterNowMinusOffset(qint64 msOffset) const
    {
        return this->findAfter(QDateTime::currentMSecsSinceEpoch() - msOffset);
    }

    template<class OBJ, class CONTAINER>
    OBJ ITimestampObjectList<OBJ, CONTAINER>::findObjectAfterOrDefault(qint64 msSinceEpoch) const
    {
        const CONTAINER after = this->findAfter(msSinceEpoch);
        if (after.isEmpty()) { return OBJ(); }
        return after.oldestObject();
    }

    template <class OBJ, class CONTAINER>
    CONTAINER ITimestampObjectList<OBJ, CONTAINER>::findInvalidTimestamps() const
    {
        return this->container().findBy([&](const OBJ & obj)
        {
            return !obj.hasValidTimestamp();
        });
    }

    template<class OBJ, class CONTAINER>
    OBJ ITimestampObjectList<OBJ, CONTAINER>::findClosestTimeDistance(qint64 msSinceEpoch) const
    {
        if (this->container().isEmpty()) { return OBJ(); }
        const auto closest = std::min_element(this->container().cbegin(), this->container().cend(), [ = ](const ITimestampBased & a, const ITimestampBased & b)
        {
            return qAbs(a.getTimeDifferenceMs(msSinceEpoch)) < qAbs(b.getTimeDifferenceMs(msSinceEpoch));
        });
        return *closest;
    }

    template <class OBJ, class CONTAINER>
    bool ITimestampObjectList<OBJ, CONTAINER>::hasInvalidTimestamps() const
    {
        return this->container().contains(&OBJ::hasValidTimestamp, false);
    }

    template <class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::setCurrentUtcTime()
    {
        for (ITimestampBased &tsObj : this->container())
        {
            tsObj.setCurrentUtcTime();
        }
    }

    template<class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::setUtcTime(qint64 msSinceEpoch)
    {
        for (ITimestampBased &tsObj : this->container())
        {
            tsObj.setMSecsSinceEpoch(msSinceEpoch);
        }
    }

    template <class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::setInvalidTimestampsToCurrentUtcTime()
    {
        for (ITimestampBased &tsObj : this->container())
        {
            if (tsObj.hasValidTimestamp()) { continue; }
            tsObj.setCurrentUtcTime();
        }
    }

    template <class OBJ, class CONTAINER>
    QDateTime ITimestampObjectList<OBJ, CONTAINER>::latestTimestamp() const
    {
        if (this->container().isEmpty()) { return QDateTime(); }
        return this->latestObject().getUtcTimestamp();
    }

    template <class OBJ, class CONTAINER>
    qint64 ITimestampObjectList<OBJ, CONTAINER>::latestTimestampMsecsSinceEpoch() const
    {
        const QDateTime dt(latestTimestamp());
        return dt.isValid() ? dt.toMSecsSinceEpoch() : -1;
    }

    template <class OBJ, class CONTAINER>
    QDateTime ITimestampObjectList<OBJ, CONTAINER>::oldestTimestamp() const
    {
        if (this->container().isEmpty()) { return QDateTime(); }
        return this->oldestObject().getUtcTimestamp();
    }

    template <class OBJ, class CONTAINER>
    qint64 ITimestampObjectList<OBJ, CONTAINER>::oldestTimestampMsecsSinceEpoch() const
    {
        const QDateTime dt(oldestTimestamp());
        return dt.isValid() ? dt.toMSecsSinceEpoch() : -1;
    }

    template <class OBJ, class CONTAINER>
    OBJ ITimestampObjectList<OBJ, CONTAINER>::latestObject() const
    {
        if (this->container().isEmpty()) { return OBJ(); }
        const auto latest = std::max_element(this->container().begin(), this->container().end(), [](const OBJ & a, const OBJ & b) { return a.getMSecsSinceEpoch() < b.getMSecsSinceEpoch(); });
        return *latest;
    }

    template <class OBJ, class CONTAINER>
    OBJ ITimestampObjectList<OBJ, CONTAINER>::oldestObject() const
    {
        if (this->container().isEmpty()) { return OBJ(); }
        const auto oldest = std::min_element(this->container().begin(), this->container().end(), [](const OBJ & a, const OBJ & b) { return a.getMSecsSinceEpoch() < b.getMSecsSinceEpoch(); });
        return *oldest;
    }

    template <class OBJ, class CONTAINER>
    CONTAINER ITimestampObjectList<OBJ, CONTAINER>::findAfter(const QDateTime &dateTime) const
    {
        return this->findAfter(dateTime.toMSecsSinceEpoch());
    }

    template <class OBJ, class CONTAINER>
    int ITimestampObjectList<OBJ, CONTAINER>::removeBefore(const QDateTime &dateTime)
    {
        return this->removeBefore(dateTime.toMSecsSinceEpoch());
    }

    template <class OBJ, class CONTAINER>
    int ITimestampObjectList<OBJ, CONTAINER>::removeBefore(qint64 msSinceEpoc)
    {
        return this->container().removeIf([&](const OBJ & obj)
        {
            return obj.isOlderThan(msSinceEpoc);
        });
    }

    template <class OBJ, class CONTAINER>
    int ITimestampObjectList<OBJ, CONTAINER>::removeOlderThanNowMinusOffset(qint64 offsetMs)
    {
        const qint64 epoch = QDateTime::currentMSecsSinceEpoch() - offsetMs;
        return this->container().removeIf([&](const OBJ & obj)
        {
            return obj.isOlderThan(epoch);
        });
    }

    template <class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::sortLatestFirst()
    {
        this->container().sortOldestFirst();
        this->container().reverse();
    }

    template <class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::sortOldestFirst()
    {
        this->container().sort(Predicates::MemberLess(&OBJ::getMSecsSinceEpoch));
    }

    template<class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::push_frontKeepLatestFirst(const OBJ &value, bool replaceSameTimestamp, int maxElements)
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
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            Q_ASSERT_X(this->isSortedLatestFirst(), Q_FUNC_INFO, "Wrong sort order");
        }
    }

    template<class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::push_backIncreaseTimestamp(const OBJ &newObject)
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

    template<class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::push_backOverrideTimestamp(const OBJ &newObject, qint64 newTsMsSinceEpoch)
    {
        OBJ newObjectCopy(newObject);
        newObjectCopy.setMSecsSinceEpoch(newTsMsSinceEpoch);
        this->container().push_back(newObjectCopy);
    }

    template<class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::setNewTimestampStartingLast(qint64 startTimeStampMs, qint64 deltaTimeMs)
    {
        if (this->container().isEmpty()) { return; }
        qint64 currentMs = startTimeStampMs;
        for (auto it = this->container().rbegin(); it != this->container().rend(); ++it)
        {
            it->setMSecsSinceEpoch(currentMs);
            currentMs += deltaTimeMs;
        }
    }

    template<class OBJ, class CONTAINER>
    int ITimestampObjectList<OBJ, CONTAINER>::replaceIfSameTimestamp(const OBJ &newObject)
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

    template<class OBJ, class CONTAINER>
    bool ITimestampObjectList<OBJ, CONTAINER>::isSortedLatestLast() const
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

    template<class OBJ, class CONTAINER>
    bool ITimestampObjectList<OBJ, CONTAINER>::isSortedLatestFirst() const
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

    template<class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::addMsecs(qint64 msToAdd)
    {
        if (msToAdd == 0) { return; }
        for (ITimestampBased &obj : this->container())
        {
            obj.addMsecs(msToAdd);
        }
    }

    template<class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::setSortHint(HintTimestampSort hint)
    {
        m_tsSortHint = hint;
    }

    template<class OBJ, class CONTAINER>
    MillisecondsMinMaxMean ITimestampObjectList<OBJ, CONTAINER>::getTimestampDifferenceMinMaxMean() const
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

    template<class OBJ, class CONTAINER>
    MillisecondsMinMaxMean ITimestampWithOffsetObjectList<OBJ, CONTAINER>::getOffsetMinMaxMean() const
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
            if (os > mmm.max) { mmm.max  = os; }
            if (os < mmm.min) { mmm.min  = os; }
            mean += os;
            c++;
        }

        if (c > 0) { mmm.mean = mean / c; }
        return mmm;
    }


    template <class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::sortAdjustedLatestFirst()
    {
        this->container().sortAdjustedOldestFirst();
        this->container().reverse();
    }

    template<class OBJ, class CONTAINER>
    CONTAINER ITimestampWithOffsetObjectList<OBJ, CONTAINER>::getSortedAdjustedLatestFirst() const
    {
        CONTAINER copy(this->container());
        copy.sortAdjustedLatestFirst();
        return copy;
    }

    template<class OBJ, class CONTAINER>
    CONTAINER ITimestampWithOffsetObjectList<OBJ, CONTAINER>::getLatestAdjustedTwoObjects(bool alreadySortedLatestFirst) const
    {
        if (this->container().size() < 2) { return CONTAINER(); }
        CONTAINER copy(alreadySortedLatestFirst ? this->container() : this->container().getSortedAdjustedLatestFirst());
        copy.truncate(2);
        return copy;
    }

    template<class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::sortAdjustedOldestFirst()
    {
        this->container().sort(Predicates::MemberLess(&OBJ::getAdjustedMSecsSinceEpoch));
    }

    template<class OBJ, class CONTAINER>
    bool ITimestampWithOffsetObjectList<OBJ, CONTAINER>::containsZeroOrNegativeOffsetTime() const
    {
        for (const ITimestampWithOffsetBased &obj : this->container())
        {
            if (obj.getTimeOffsetMs() <= 0) { return true; }
        }
        return false;
    }

    template<class OBJ, class CONTAINER>
    bool ITimestampWithOffsetObjectList<OBJ, CONTAINER>::containsNegativeOffsetTime() const
    {
        for (const ITimestampWithOffsetBased &obj : this->container())
        {
            if (obj.getTimeOffsetMs() < 0) { return true; }
        }
        return false;
    }

    template<class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::addMsecsToOffset(qint64 msToAdd)
    {
        for (ITimestampWithOffsetBased &obj : this->container())
        {
            obj.addMsecsToOffsetTime(msToAdd);
        }
    }

    template<class OBJ, class CONTAINER>
    ITimestampWithOffsetObjectList<OBJ, CONTAINER>::ITimestampWithOffsetObjectList() : ITimestampObjectList<OBJ, CONTAINER>()
    {
        static_assert(std::is_base_of<ITimestampWithOffsetBased, OBJ>::value, "OBJ needs to implement ITimestampBased");
    }

    template<class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::push_frontKeepLatestAdjustedFirst(const OBJ &value, bool replaceSameTimestamp, int maxElements)
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

    template<class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::push_frontKeepLatestFirstAdjustOffset(const OBJ &value, bool replaceSameTimestamp, int maxElements)
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

        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            BLACK_VERIFY_X(front.isNewerThanAdjusted(second), Q_FUNC_INFO, "Front/second timestamp");
            BLACK_VERIFY_X(this->isSortedAdjustedLatestFirst(), Q_FUNC_INFO, "Wrong sort order");
        }
    }

    template<class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::push_frontKeepLatestFirstIgnoreOverlapping(const OBJ &value, bool replaceSameTimestamp, int maxElements)
    {
        CONTAINER &c = this->container();
        if (c.size() > 1)
        {
            const ITimestampWithOffsetBased front = c.front();
            if (value.getAdjustedMSecsSinceEpoch() <= front.getAdjustedMSecsSinceEpoch()) { return; }
        }
        ITimestampWithOffsetObjectList<OBJ, CONTAINER>::push_frontKeepLatestFirst(value, replaceSameTimestamp, maxElements);
    }

    template<class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::prefillLatestAdjustedFirst(const OBJ &value, int elements, qint64 deltaTimeMs)
    {
        this->container().clear();
        const qint64 osTime = value.getTimeOffsetMs();
        const qint64 os = -1 * qAbs(deltaTimeMs < 0 ? osTime : deltaTimeMs);
        if (CBuildConfig::isLocalDeveloperDebugBuild())
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

    template<class OBJ, class CONTAINER>
    bool ITimestampWithOffsetObjectList<OBJ, CONTAINER>::isSortedAdjustedLatestLast() const
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

    template<class OBJ, class CONTAINER>
    bool ITimestampWithOffsetObjectList<OBJ, CONTAINER>::isSortedAdjustedLatestFirst() const
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

    template<class OBJ, class CONTAINER>
    CONTAINER ITimestampWithOffsetObjectList<OBJ, CONTAINER>::findAfterAdjusted(qint64 msSinceEpoch) const
    {
        return this->container().findBy([&](const ITimestampWithOffsetBased & obj)
        {
            return obj.isNewerThanAdjusted(msSinceEpoch);
        });
    }

    template<class OBJ, class CONTAINER>
    OBJ ITimestampWithOffsetObjectList<OBJ, CONTAINER>::findObjectAfterAdjustedOrDefault(qint64 msSinceEpoch) const
    {
        const CONTAINER after = this->findAfterAdjusted(msSinceEpoch);
        if (after.isEmpty()) { return OBJ(); }
        return after.oldestAdjustedObject();
    }

    template<class OBJ, class CONTAINER>
    CONTAINER ITimestampWithOffsetObjectList<OBJ, CONTAINER>::findBeforeAdjusted(qint64 msSinceEpoch) const
    {
        return this->container().findBy([&](const ITimestampWithOffsetBased & obj)
        {
            return obj.isOlderThanAdjusted(msSinceEpoch);
        });
    }

    template<class OBJ, class CONTAINER>
    OBJ ITimestampWithOffsetObjectList<OBJ, CONTAINER>::findObjectBeforeAdjustedOrDefault(qint64 msSinceEpoch) const
    {
        const CONTAINER before = this->findBeforeAdjusted(msSinceEpoch);
        if (before.isEmpty()) { return OBJ(); }
        return before.latestAdjustedObject();
    }

    template<class OBJ, class CONTAINER>
    OBJ ITimestampWithOffsetObjectList<OBJ, CONTAINER>::findClosestTimeDistanceAdjusted(qint64 msSinceEpoch) const
    {
        if (this->container().isEmpty()) { return OBJ(); }
        const auto closest = std::min_element(this->container().cbegin(), this->container().cend(), [ = ](const ITimestampWithOffsetBased & a, const ITimestampWithOffsetBased & b)
        {
            return qAbs(a.getAdjustedTimeDifferenceMs(msSinceEpoch)) < qAbs(b.getAdjustedTimeDifferenceMs(msSinceEpoch));
        });
        return *closest;
    }

    template <class OBJ, class CONTAINER>
    OBJ ITimestampWithOffsetObjectList<OBJ, CONTAINER>::latestAdjustedObject() const
    {
        if (this->container().isEmpty()) { return OBJ(); }
        if (this->container().m_tsAdjustedSortHint == AdjustedTimestampLatestFirst)
        {
            return this->container().front();
        }
        const auto latest = std::max_element(this->container().begin(), this->container().end(), [](const OBJ & a, const OBJ & b) { return a.getAdjustedMSecsSinceEpoch() < b.getAdjustedMSecsSinceEpoch(); });
        return *latest;
    }

    template <class OBJ, class CONTAINER>
    OBJ ITimestampWithOffsetObjectList<OBJ, CONTAINER>::oldestAdjustedObject() const
    {
        if (this->container().isEmpty()) { return OBJ(); }
        if (this->container().m_tsAdjustedSortHint == AdjustedTimestampLatestFirst)
        {
            return this->container().back();
        }
        const auto oldest = std::min_element(this->container().begin(), this->container().end(), [](const OBJ & a, const OBJ & b) { return a.getAdjustedMSecsSinceEpoch() < b.getAdjustedMSecsSinceEpoch(); });
        return *oldest;
    }

    template <class OBJ, class CONTAINER>
    QDateTime ITimestampWithOffsetObjectList<OBJ, CONTAINER>::latestAdjustedTimestamp() const
    {
        if (this->container().isEmpty()) { return QDateTime(); }
        return this->latestAdjustedObject().getUtcTimestamp();
    }

    template <class OBJ, class CONTAINER>
    qint64 ITimestampWithOffsetObjectList<OBJ, CONTAINER>::latestAdjustedTimestampMsecsSinceEpoch() const
    {
        if (this->container().isEmpty()) { return -1; }
        const QDateTime dt(this->latestAdjustedTimestamp());
        return dt.isValid() ? dt.toMSecsSinceEpoch() : -1;
    }

    template <class OBJ, class CONTAINER>
    QDateTime ITimestampWithOffsetObjectList<OBJ, CONTAINER>::oldestAdjustedTimestamp() const
    {
        if (this->container().isEmpty()) { return QDateTime(); }
        return this->oldestAdjustedObject().getUtcTimestamp();
    }

    template <class OBJ, class CONTAINER>
    qint64 ITimestampWithOffsetObjectList<OBJ, CONTAINER>::oldestAdjustedTimestampMsecsSinceEpoch() const
    {
        if (this->container().isEmpty()) { return -1; }
        const QDateTime dt(oldestAdjustedTimestamp());
        return dt.isValid() ? dt.toMSecsSinceEpoch() : -1;
    }

    template <class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::setAdjustedSortHint(HintAdjustedTimestampSort hint)
    {
        this->container().m_tsAdjustedSortHint = hint;
    }

    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::CCountry, BlackMisc::CCountryList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftCategory, BlackMisc::Aviation::CAircraftCategoryList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Db::CDbInfo, BlackMisc::Db::CDbInfoList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Db::CArtifact, BlackMisc::Db::CArtifactList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Db::CDistribution, BlackMisc::Db::CDistributionList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Network::CTextMessage, BlackMisc::Network::CTextMessageList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Network::CUrlLog, BlackMisc::Network::CUrlLogList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Network::CRawFsdMessage, BlackMisc::Network::CRawFsdMessageList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Simulation::CMatchingStatisticsEntry, BlackMisc::Simulation::CMatchingStatistics>;

    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftSituationChange, BlackMisc::Aviation::CAircraftSituationChangeList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampWithOffsetObjectList<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampWithOffsetObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampWithOffsetObjectList<BlackMisc::Aviation::CAircraftSituationChange, BlackMisc::Aviation::CAircraftSituationChangeList>;

} // namespace

//! \endcond
