/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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
    //! List of objects with timestamp.
    //! Such objects should implement \sa ITimestampBased
    template<class OBJ, class CONTAINER> class ITimestampObjectList
    {
    public:
        //! List of objects before dateTime
        CONTAINER findBefore(const QDateTime &dateTime) const;

        //! List of objects before msSinceEpoch
        CONTAINER findBefore(qint64 msSinceEpoch) const;

        //! Get objects before msSinceEpoch and remove those
        CONTAINER findBeforeAndRemove(qint64 msSinceEpoch);

        //! List of objects before now - offset
        CONTAINER findBeforeNowMinusOffset(qint64 msOffset) const;

        //! List of objects after dateTime
        CONTAINER findAfter(const QDateTime &dateTime) const;

        //! List of objects after msSinceEpoch
        CONTAINER findAfter(qint64 msSinceEpoch) const;

        //! Objects without valid timestamp
        CONTAINER findInvalidTimestamps() const;

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

        //! Latest value
        OBJ latestObject() const;

        //! Latest value
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
        void push_frontKeepLatestFirst(const OBJ &value, int maxElements = -1);

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedLatestLast() const;

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedLatestFirst() const;

    protected:
        //! Constructor
        ITimestampObjectList();

        //! Container
        const CONTAINER &container() const;

        //! Container
        CONTAINER &container();
    };

    //! List of objects with timestamp and offset.
    //! Such objects should implement \sa ITimestampWithOffsetBased
    template<class OBJ, class CONTAINER> class ITimestampWithOffsetObjectList : public ITimestampObjectList<OBJ, CONTAINER>
    {
    public:
        //! Sort by adjusted timestamp
        void sortAdjustedLatestFirst();

        //! Sort by adjusted timestamp
        void sortAdjustedOldestFirst();

        //! Insert as first element by keeping maxElements and the latest first
        void push_frontKeepLatestAdjustedFirst(const OBJ &value, int maxElements = -1);

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedAdjustedLatestLast() const;

        //! Is completely sorted: latest last
        //! \remark all object must have a valid timestamp
        bool isSortedAdjustedLatestFirst() const;

    protected:
        //! Constructor
        ITimestampWithOffsetObjectList();
    };

    //! \cond PRIVATE
    namespace Aviation
    {
        class CAircraftSituation;
        class CAircraftSituationList;
        class CAircraftParts;
        class CAircraftPartsList;
        class CAirport;
        class CAirportList;
        class CLivery;
        class CLiveryList;
        class CAircraftIcaoCode;
        class CAircraftIcaoCodeList;
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
    }

    class CStatusMessage;
    class CStatusMessageList;
    class CIdentifier;
    class CIdentifierList;
    class CCountry;
    class CCountryList;

    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Db::CDbInfo, BlackMisc::Db::CDbInfoList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Db::CArtifact, BlackMisc::Db::CArtifactList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Db::CDistribution, BlackMisc::Db::CDistributionList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Network::CTextMessage, BlackMisc::Network::CTextMessageList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Network::CRawFsdMessage, BlackMisc::Network::CRawFsdMessageList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Network::CUrlLog, BlackMisc::Network::CUrlLogList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::CIdentifier, BlackMisc::CIdentifierList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::CCountry, BlackMisc::CCountryList>;

    // for the derived version both templates are required
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampWithOffsetObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE ITimestampWithOffsetObjectList<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList>;
    //! \endcond

} //namespace

#endif //guard
