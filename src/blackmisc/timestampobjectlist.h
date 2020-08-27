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
    public:
        //! Hint if the list is sorted
        enum HintTimestampSort
        {
            NoTimestampSortHint,
            TimestampLatestFirst,
            TimestampLatestLast
        };

        //! List of objects before dateTime (older)
        CONTAINER findBefore(const QDateTime &dateTime) const;

        //! List of objects before msSinceEpoch (older)
        CONTAINER findBefore(qint64 msSinceEpoch) const;

        //! Object before timestamp or default (older)
        OBJ findObjectBeforeOrDefault(qint64 msSinceEpoch) const;

        //! Get objects before msSinceEpoch and remove those
        CONTAINER findBeforeAndRemove(qint64 msSinceEpoch);

        //! List of objects before now - offset
        CONTAINER findBeforeNowMinusOffset(qint64 msOffset) const;

        //! List of objects after dateTime (newer)
        CONTAINER findAfter(const QDateTime &dateTime) const;

        //! List of objects after msSinceEpoch (newer)
        CONTAINER findAfter(qint64 msSinceEpoch) const;

        //! List of objects before now - offset
        CONTAINER findAfterNowMinusOffset(qint64 msOffset) const;

        //! List of objects after msSinceEpoch (newer)
        OBJ findObjectAfterOrDefault(qint64 msSinceEpoch) const;

        //! Objects without valid timestamp
        CONTAINER findInvalidTimestamps() const;

        //! Find closest (or default)
        OBJ findClosestTimeDistance(qint64 msSinceEpoch) const;

        //! Has invalid timestamp
        bool hasInvalidTimestamps() const;

        //! Set all timestamps to now
        void setCurrentUtcTime();

        //! Set all timestamps to given time
        void setUtcTime(qint64 msSinceEpoch);

        //! Set invalid timestamps to now
        void setInvalidTimestampsToCurrentUtcTime();

        //! Latest timestamp
        QDateTime latestTimestamp() const;

        //! Latest timestamp
        qint64 latestTimestampMsecsSinceEpoch() const;

        //! Oldest timestamp
        QDateTime oldestTimestamp() const;

        //! Oldest timestamp
        qint64 oldestTimestampMsecsSinceEpoch() const;

        //! Latest object
        OBJ latestObject() const;

        //! Latest object
        OBJ oldestObject() const;

        //! Remove objects with timestamp before dateTime
        int removeBefore(const QDateTime &dateTime);

        //! Remove objects with timestamp before dateTime
        int removeBefore(qint64 msSinceEpoch);

        //! Remove objects older than seconds
        int removeOlderThanNowMinusOffset(qint64 offsetMs);

        //! Sort by timestamp
        void sortLatestFirst();

        //! Sort by timestamp
        void sortOldestFirst();

        //! Insert as first element by keeping maxElements and the latest first
        void push_frontKeepLatestFirst(const OBJ &value, bool replaceSameTimestamp = true, int maxElements = -1);

        //! Push back and increase the timestamp at least by +1ms if equal to last element
        //! \remark if the timestamp is already greater it does not modifcation
        void push_backIncreaseTimestamp(const OBJ &newObject);

        //! Push back, but set new timestamp
        void push_backOverrideTimestamp(const OBJ &newObject, qint64 newTsMsSinceEpoch);

        //! Set new timestamps starting with the last element
        void setNewTimestampStartingLast(qint64 startTimeStampMs, qint64 deltaTimeMs);

        //! Replace if an object has the same timestamp
        int replaceIfSameTimestamp(const OBJ &newObject);

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedLatestLast() const;

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedLatestFirst() const;

        //! Adds a time to all values
        void addMsecs(qint64 msToAdd);

        //! Set the hint
        void setSortHint(HintTimestampSort hint);

        //! Difference of timestamp values
        //! \cond timestamp list has to be sorted to get meaningful values
        MillisecondsMinMaxMean getTimestampDifferenceMinMaxMean() const;

    protected:
        //! Constructor
        ITimestampObjectList();

        //! Container
        const CONTAINER &container() const;

        //! Container
        CONTAINER &container();

        HintTimestampSort m_tsSortHint = NoTimestampSortHint; //!< sort hint
    };

    //! List of objects with timestamp and offset.
    //! Such objects should implement \sa ITimestampWithOffsetBased
    template<class OBJ, class CONTAINER> class ITimestampWithOffsetObjectList : public ITimestampObjectList<OBJ, CONTAINER>
    {
    public:
        //! Hint if the list is sorted
        enum HintAdjustedTimestampSort
        {
            NoAdjustedTimestampSortHint,
            AdjustedTimestampLatestFirst
        };

        //! Sort by adjusted timestamp
        void sortAdjustedLatestFirst();

        //! As sorted copy
        CONTAINER getSortedAdjustedLatestFirst() const;

        //! Get the latest 2 values
        CONTAINER getLatestAdjustedTwoObjects(bool alreadySortedLatestFirst = false) const;

        //! Sort by adjusted timestamp
        void sortAdjustedOldestFirst();

        //! Any negative or zero offset time?
        bool containsZeroOrNegativeOffsetTime() const;

        //! Any negative offset time?
        bool containsNegativeOffsetTime() const;

        //! Adds a time to all offset values
        void addMsecsToOffset(qint64 msToAdd);

        //! Insert as first element by keeping maxElements and the latest first
        void push_frontKeepLatestAdjustedFirst(const OBJ &value, bool replaceSameTimestamp = true, int maxElements = -1);

        //! Insert as first element by keeping maxElements and the latest first
        //! \remark adjust offset to average offset of two adjacent elements so adjusted values are sorted
        void push_frontKeepLatestFirstAdjustOffset(const OBJ &value, bool replaceSameTimestamp = true, int maxElements = -1);

        //! Add value, but ignore overlapping (past) values
        void push_frontKeepLatestFirstIgnoreOverlapping(const OBJ &value, bool replaceSameTimestamp = true, int maxElements = -1);

        //! Prefill with elements
        void prefillLatestAdjustedFirst(const OBJ &value, int elements, qint64 deltaTimeMs = -1);

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedAdjustedLatestLast() const;

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedAdjustedLatestFirst() const;

        //! List of objects after msSinceEpoch (newer)
        CONTAINER findAfterAdjusted(qint64 msSinceEpoch) const;

        //! List of objects after msSinceEpoch (newer)
        OBJ findObjectAfterAdjustedOrDefault(qint64 msSinceEpoch) const;

        //! List of objects before msSinceEpoch (older)
        CONTAINER findBeforeAdjusted(qint64 msSinceEpoch) const;

        //! Object before timestamp (older)
        OBJ findObjectBeforeAdjustedOrDefault(qint64 msSinceEpoch) const;

        //! Closest adjusted time difference
        OBJ findClosestTimeDistanceAdjusted(qint64 msSinceEpoch) const;

        //! Latest adjusted object
        OBJ latestAdjustedObject() const;

        //! Oldest adjusted object
        OBJ oldestAdjustedObject() const;

        //! Latest adjusted timestamp
        QDateTime latestAdjustedTimestamp() const;

        //! Oldest adjusted timestamp
        QDateTime oldestAdjustedTimestamp() const;

        //! Latest adjusted timestamp
        qint64 latestAdjustedTimestampMsecsSinceEpoch() const;

        //! Oldest adjusted timestamp
        qint64 oldestAdjustedTimestampMsecsSinceEpoch() const;

        //! Set the hint
        void setAdjustedSortHint(HintAdjustedTimestampSort hint);

        //! Difference of timestamp values
        //! \cond timestamp list has to be sorted to get meaningful values
        MillisecondsMinMaxMean getOffsetMinMaxMean() const;

    protected:
        //! Constructor
        ITimestampWithOffsetObjectList();

        HintAdjustedTimestampSort m_tsAdjustedSortHint = NoAdjustedTimestampSortHint; //!< sort hint
    };

    //! \cond PRIVATE
    namespace Aviation
    {
        class CAircraftSituation;
        class CAircraftSituationList;
        class CAircraftSituationChange;
        class CAircraftSituationChangeList;
        class CAircraftParts;
        class CAircraftPartsList;
        class CAirport;
        class CAirportList;
        class CLivery;
        class CLiveryList;
        class CAircraftIcaoCode;
        class CAircraftIcaoCodeList;
        class CAircraftCategory;
        class CAircraftCategoryList;
        class CAirlineIcaoCode;
        class CAirlineIcaoCodeList;
    }

    namespace Network
    {
        class CTextMessage;
        class CTextMessageList;
        class CRawFsdMessage;
        class CRawFsdMessageList;
        class CUrlLog;
        class CUrlLogList;
    }

    namespace Db
    {
        class CDbInfo;
        class CDbInfoList;
        class CArtifact;
        class CArtifactList;
        class CDistribution;
        class CDistributionList;
    }

    namespace Simulation
    {
        class CDistributor;
        class CDistributorList;
        class CAircraftModel;
        class CAircraftModelList;
        class CMatchingStatistics;
        class CMatchingStatisticsEntry;
    }

    class CStatusMessage;
    class CStatusMessageList;
    class CCountry;
    class CCountryList;

    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftCategory, BlackMisc::Aviation::CAircraftCategoryList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Db::CDbInfo, BlackMisc::Db::CDbInfoList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Db::CArtifact, BlackMisc::Db::CArtifactList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Db::CDistribution, BlackMisc::Db::CDistributionList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Simulation::CMatchingStatisticsEntry, BlackMisc::Simulation::CMatchingStatistics>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Network::CTextMessage, BlackMisc::Network::CTextMessageList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Network::CRawFsdMessage, BlackMisc::Network::CRawFsdMessageList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Network::CUrlLog, BlackMisc::Network::CUrlLogList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::CCountry, BlackMisc::CCountryList>;

    // for the derived version both templates are required
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftSituationChange, BlackMisc::Aviation::CAircraftSituationChangeList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampWithOffsetObjectList<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampWithOffsetObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampWithOffsetObjectList<BlackMisc::Aviation::CAircraftSituationChange, BlackMisc::Aviation::CAircraftSituationChangeList>;
    //! \endcond

} //namespace

#endif //guard
