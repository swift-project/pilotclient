// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Aviation, CAirport, CAirportList)

namespace BlackMisc::Aviation
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
        CAirportList() = default;

        //! Construct from a base class object.
        CAirportList(const CSequence<CAirport> &other);

        //! Find 0..n airports by ICAO code
        CAirportList findByIcao(const CAirportIcaoCode &icao) const;

        //! Find first station by callsign, if not return default
        CAirport findFirstByIcao(const CAirportIcaoCode &icao) const;

        //! Find first by name or location, if not return default
        CAirport findFirstByNameOrLocation(const QString &nameOrLocation) const;

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
        QStringList allLocations() const;

        //! All locations plus optional description
        QStringList allLocationsPlusOptionalDescription(bool sorted) const;

        //! From our DB JSON
        static CAirportList fromDatabaseJson(const QJsonArray &array, CAirportList *inconsistent = nullptr);
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirportList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAirport>)

#endif // guard
