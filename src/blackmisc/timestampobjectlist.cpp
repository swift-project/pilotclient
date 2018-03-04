/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
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
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/identifierlist.h"
#include "blackmisc/countrylist.h"

#include <QDateTime>
#include <algorithm>
#include <limits>
#include <iterator>
#include <type_traits>

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
        const auto latest = std::max_element(container().begin(), container().end(), [](const OBJ & a, const OBJ & b) { return a.getMSecsSinceEpoch() < b.getMSecsSinceEpoch(); });
        return *latest;
    }

    template <class OBJ, class CONTAINER>
    OBJ ITimestampObjectList<OBJ, CONTAINER>::oldestObject() const
    {
        if (this->container().isEmpty()) { return OBJ(); }
        const auto oldest = std::min_element(container().begin(), container().end(), [](const OBJ & a, const OBJ & b) { return a.getMSecsSinceEpoch() < b.getMSecsSinceEpoch(); });
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
        this->container().sort(BlackMisc::Predicates::MemberLess(&OBJ::getMSecsSinceEpoch));
    }

    template<class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::push_frontKeepLatestFirst(const OBJ &value, int maxElements)
    {
        CONTAINER &c = this->container();
        if (maxElements > 0 && c.size() >= maxElements)
        {
            c.truncate(maxElements - 1);
        }
        const bool needSort = !c.isEmpty() && value.isOlderThan(c.front());
        c.push_front(value);
        if (needSort)
        {
            ITimestampObjectList::sortLatestFirst();
        }
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
            continue;
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
            continue;
        }
        return true;
    }

    template <class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::sortAdjustedLatestFirst()
    {
        this->container().sortAdjustedOldestFirst();
        this->container().reverse();
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
    ITimestampWithOffsetObjectList<OBJ, CONTAINER>::ITimestampWithOffsetObjectList() : ITimestampObjectList<OBJ, CONTAINER>()
    {
        static_assert(std::is_base_of<ITimestampWithOffsetBased, OBJ>::value, "OBJ needs to implement ITimestampBased");
    }

    template<class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::push_frontKeepLatestAdjustedFirst(const OBJ &value, int maxElements)
    {
        Q_ASSERT_X(maxElements < 0 || maxElements > 1, Q_FUNC_INFO, "Max.value wrong range");
        CONTAINER &c = this->container();
        if (maxElements > 0 && c.size() >= maxElements)
        {
            c.truncate(maxElements - 1);
        }
        const bool needSort = !c.isEmpty() && value.isOlderThanAdjusted(c.front());
        c.push_front(value);
        if (needSort)
        {
            ITimestampWithOffsetObjectList::sortAdjustedLatestFirst();
        }
    }

    template<class OBJ, class CONTAINER>
    void ITimestampWithOffsetObjectList<OBJ, CONTAINER>::push_frontKeepLatestFirstAdjustOffset(const OBJ &value, int maxElements)
    {
        ITimestampObjectList<OBJ, CONTAINER>::push_frontKeepLatestFirst(value, maxElements);

        // now sorted by timestamp
        // this reflects normally the incoming order
        CONTAINER &c = this->container();
        if (c.size() < 2) { return; }
        const ITimestampWithOffsetBased &front = c.front();
        ITimestampWithOffsetBased &second = c[1];
        if (!front.isOlderThanAdjusted(second)) { return; }
        const qint64 maxOs = front.getAdjustedMSecsSinceEpoch() - second.getMSecsSinceEpoch();
        const qint64 avgOs = (maxOs + qMax(front.getTimeOffsetMs(), maxOs)) / 2;
        second.setTimeOffsetMs(avgOs);

        // ts
        //  8: os 2 adj 10
        //  6: os 2 adj  8
        //  5: os 5 adj 10 => max os 3
        //  0: os 5 adj  5
    }

    template<class OBJ, class CONTAINER>
    bool ITimestampWithOffsetObjectList<OBJ, CONTAINER>::isSortedAdjustedLatestLast() const
    {
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
    OBJ ITimestampWithOffsetObjectList<OBJ, CONTAINER>::findClosestTimeDistanceAdjusted(qint64 msSinceEpoch) const
    {
        if (this->container().isEmpty()) { return OBJ(); }
        const auto closest = std::min_element(this->container().cbegin(), this->container().cend(), [ = ](const ITimestampWithOffsetBased & a, const ITimestampWithOffsetBased & b)
        {
            return qAbs(a.getAdjustedTimeDifferenceMs(msSinceEpoch)) < qAbs(b.getAdjustedTimeDifferenceMs(msSinceEpoch));
        });
        return *closest;
    }

    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    //! \cond PRIVATE
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::CCountry, BlackMisc::CCountryList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::CIdentifier, BlackMisc::CIdentifierList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList>;
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
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampWithOffsetObjectList<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE ITimestampWithOffsetObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
    //! \endcond

} // namespace
