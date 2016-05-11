/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/identifierlist.h"
#include "blackmisc/countrylist.h"

#include <QDateTime>
#include <algorithm>
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
    QList<CONTAINER> ITimestampObjectList<OBJ, CONTAINER>::splitByTime(qint64 msSinceEpoch, bool sortedLatestFirst) const
    {
        QList<CONTAINER> result { {}, {} };
        const auto &c = this->container();
        if (sortedLatestFirst)
        {
            // O(log n) comparisons and O(n) copies
            struct Comparator
            {
                bool operator()(const OBJ &a, qint64 b) const { return a.isNewerThan(b); }
                bool operator()(qint64 a, const OBJ &b) const { return b.isOlderThan(a); }
            };
            auto it = std::upper_bound(c.begin(), c.end(), msSinceEpoch, Comparator());
            std::copy(c.begin(), it, std::back_inserter(result[0]));
            std::copy(it, c.end(), std::back_inserter(result[1]));
        }
        else
        {
            // O(n) comparisons and O(n) copies
            std::partition_copy(c.begin(), c.end(), std::back_inserter(result[0]), std::back_inserter(result[1]),
            [msSinceEpoch](const OBJ & obj) { return ! obj.isNewerThan(msSinceEpoch); });
        }
        return result;
    }

    template <class OBJ, class CONTAINER>
    OBJ ITimestampObjectList<OBJ, CONTAINER>::latestObject() const
    {
        if (this->container().isEmpty()) { return OBJ(); }
        auto latest = std::max_element(container().begin(), container().end(), [](const OBJ & a, const OBJ & b) { return a.getMSecsSinceEpoch() < b.getMSecsSinceEpoch(); });
        return *latest;
    }

    template <class OBJ, class CONTAINER>
    OBJ ITimestampObjectList<OBJ, CONTAINER>::oldestObject() const
    {
        if (this->container().isEmpty()) { return OBJ(); }
        auto oldest = std::min_element(container().begin(), container().end(), [](const OBJ & a, const OBJ & b) { return a.getMSecsSinceEpoch() < b.getMSecsSinceEpoch(); });
        return *oldest;
    }

    template <class OBJ, class CONTAINER>
    CONTAINER ITimestampObjectList<OBJ, CONTAINER>::findAfter(const QDateTime &dateTime) const
    {
        return this->findAfter(dateTime.toMSecsSinceEpoch());
    }

    template <class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::removeBefore(const QDateTime &dateTime)
    {
        this->removeBefore(dateTime.toMSecsSinceEpoch());
    }

    template <class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::removeBefore(qint64 msSinceEpoc)
    {
        this->container().removeIf([&](const OBJ & obj)
        {
            return obj.isOlderThan(msSinceEpoc);
        });
    }

    template <class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::removeOlderThanNowMinusOffset(qint64 offsetMs)
    {
        const qint64 epoch = QDateTime::currentMSecsSinceEpoch() - offsetMs;
        this->container().removeIf([&](const OBJ & obj)
        {
            return obj.isOlderThan(epoch);
        });
    }

    template <class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::sortLatestFirst()
    {
        this->container().sortOldestFirst();
        std::reverse(this->container().begin(), this->container().end());
    }

    template <class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::sortOldestFirst()
    {
        this->container().sort(BlackMisc::Predicates::MemberLess(&OBJ::getMSecsSinceEpoch));
    }

    template <class OBJ, class CONTAINER>
    void ITimestampObjectList<OBJ, CONTAINER>::push_frontMaxElements(const OBJ &object, int maxElements)
    {
        Q_ASSERT(maxElements > 1);
        if (this->container().size() >= (maxElements - 1))
        {
            this->container().truncate(maxElements - 1);
        }
        this->container().push_front(object);
    }

    // see here for the reason of thess forward instantiations
    // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
    template class ITimestampObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
    template class ITimestampObjectList<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList>;
    template class ITimestampObjectList<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList>;
    template class ITimestampObjectList<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList>;
    template class ITimestampObjectList<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList>;
    template class ITimestampObjectList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    template class ITimestampObjectList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;
    template class ITimestampObjectList<BlackMisc::Network::CTextMessage, BlackMisc::Network::CTextMessageList>;
    template class ITimestampObjectList<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>;
    template class ITimestampObjectList<BlackMisc::CIdentifier, BlackMisc::CIdentifierList>;
    template class ITimestampObjectList<BlackMisc::CCountry, BlackMisc::CCountryList>;

} // namespace
