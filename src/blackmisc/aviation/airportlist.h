/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRPORTLIST_H
#define BLACKMISC_AVIATION_AIRPORTLIST_H

#include "blackmisc/aviation/airport.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/sequence.h"

#include <QMetaType>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for a list of airports.
        class BLACKMISC_EXPORT CAirportList :
            public CSequence<CAirport>,
            public Db::IDatastoreObjectList<CAirport, CAirportList, int>,
            public Geo::IGeoObjectWithRelativePositionList<CAirport, CAirportList>,
            public Mixin::MetaType<CAirportList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAirportList)
            using CSequence::CSequence;

            //! Default constructor.
            CAirportList();

            //! Construct from a base class object.
            CAirportList(const CSequence<CAirport> &other);

            //! Find 0..n airports by ICAO code
            CAirportList findByIcao(const CAirportIcaoCode &icao) const;

            //! Find first station by callsign, if not return given value / default
            CAirport findFirstByIcao(const CAirportIcaoCode &icao, const CAirport &ifNotFound = CAirport()) const;

            //! Find first by name or location, if not return given value / default
            CAirport findFirstByNameOrLocation(const QString &nameOrLocation, const CAirport &ifNotFound = CAirport()) const;

            //! Containing an airport with given ICAO code?
            bool containsAirportWithIcaoCode(const CAirportIcaoCode &icao) const;

            //! Replace or add based on same ICAO code
            void replaceOrAddByIcao(const CAirport &addedOrReplacedAirport);

            //! Update this list from the other list
            void updateMissingParts(const CAirportList &updateFromList);

            //! All ICAO codes
            QStringList allIcaoCodes(bool sorted) const;

            //! All names
            QStringList allDescriptivesNames(bool sorted) const;

            //! All locations
            //! \remark less locations than airports, since a location (e.g. New Yorrk) homes multiple airports
            QStringList allLocations(bool sorted) const;

            //! All locations plus optional description
            QStringList allLocationsPlusOptionalDescription(bool sorted) const;

            //! From our DB JSON
            static CAirportList fromDatabaseJson(const QJsonArray &array, CAirportList *inconsistent = nullptr);
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirportList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAirport>)

#endif //guard
