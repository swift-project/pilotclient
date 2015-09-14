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

#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QList>
#include <QMap>

namespace BlackMisc
{
    //! List of objects with timestamp.
    //! Such objects should implement \sa ITimestampBased
    template<class OBJ, class CONTAINER>
    class ITimestampObjectList
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

        //! Partition into two containers, first [0,msSinceEpoch] and second (msSinceEpoch,LLONG_MAX].
        //! Within each of the two parts, the original relative ordering of the elements is preserved.
        QList<CONTAINER> splitByTime(qint64 msSinceEpoch, bool sortedLatestFirst = false) const;

        //! Latest value
        OBJ latestValue() const;

        //! Latest value
        OBJ oldestValue() const;

        //! Remove objects with timestamp before dateTime
        void removeBefore(const QDateTime &dateTime);

        //! Remove objects with timestamp before dateTime
        void removeBefore(qint64 msSinceEpoch);

        //! Remove objects older than seconds
        void removeOlderThanNowMinusOffset(qint64 offsetMs);

        //! Sort by timestamp
        void sortLatestFirst();

        //! Sort by timestamp
        void sortOldestFirst();

        //! Inserts as first object by keeping max. elements
        void push_frontMaxElements(const OBJ &object, int maxElements);

    protected:
        //! Constructor
        ITimestampObjectList();

        //! Container
        const CONTAINER &container() const;

        //! Container
        CONTAINER &container();
    };

    //! \cond PRIVATE
    namespace Aviation
    {
        class CAircraftSituation;
        class CAircraftSituationList;
        class CAircraftParts;
        class CAircraftPartsList;
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

    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList>;

    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;

    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::Network::CTextMessage, BlackMisc::Network::CTextMessageList>;

    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::CIdentifier, BlackMisc::CIdentifierList>;
    extern template class BLACKMISC_EXPORT_TEMPLATE ITimestampObjectList<BlackMisc::CCountry, BlackMisc::CCountryList>;
    //! \endcond

} //namespace

#endif //guard
