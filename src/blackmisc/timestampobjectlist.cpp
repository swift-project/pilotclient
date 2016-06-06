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
#include "blackmisc/db/dbinfolist.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/timestampobjectlist.h"
#include "blackmisc/predicates.h"
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
    CONTAINER ITimestampObjectList<OBJ, CONTAINER>::findInvalidTimestamps() const
    {
        return this->container().findBy([&](const OBJ & obj)
        {
            return !obj.hasValidTimestamp();
        });
    }

    template <class OBJ, class CONTAINER>
    bool ITimestampObjectList<OBJ, CONTAINER>::hasInvalidTimestamps() const
    {
        return this->container().contains(&OBJ::hasValidTimestamp, false);
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
    template class ITimestampObjectList<BlackMisc::Db::CDbInfo, BlackMisc::Db::CDbInfoList>;
    template class ITimestampObjectList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    template class ITimestampObjectList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;
    template class ITimestampObjectList<BlackMisc::Network::CTextMessage, BlackMisc::Network::CTextMessageList>;
    template class ITimestampObjectList<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>;
    template class ITimestampObjectList<BlackMisc::CIdentifier, BlackMisc::CIdentifierList>;
    template class ITimestampObjectList<BlackMisc::CCountry, BlackMisc::CCountryList>;

} // namespace
