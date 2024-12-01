// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRPORTLIST_H
#define SWIFT_MISC_AVIATION_AIRPORTLIST_H

#include <QMetaType>

#include "misc/aviation/airport.h"
#include "misc/aviation/airporticaocode.h"
#include "misc/collection.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/geo/geoobjectlist.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CAirport, CAirportList)

namespace swift::misc::aviation
{
    //! Value object for a list of airports.
    class SWIFT_MISC_EXPORT CAirportList :
        public CSequence<CAirport>,
        public db::IDatastoreObjectList<CAirport, CAirportList, int>,
        public geo::IGeoObjectWithRelativePositionList<CAirport, CAirportList>,
        public mixin::MetaType<CAirportList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAirportList)
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
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAirportList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CAirport>)

#endif // SWIFT_MISC_AVIATION_AIRPORTLIST_H
