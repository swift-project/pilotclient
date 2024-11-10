// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRLINEICAOCODELIST_H
#define SWIFT_MISC_AVIATION_AIRLINEICAOCODELIST_H

#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/callsign.h"
#include "misc/swiftmiscexport.h"
#include "misc/collection.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/sequence.h"

#include <QJsonArray>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <tuple>

BLACK_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CAirlineIcaoCode, CAirlineIcaoCodeList)

namespace swift::misc::aviation
{
    //! Value object encapsulating a list of ICAO codes.
    class SWIFT_MISC_EXPORT CAirlineIcaoCodeList :
        public CSequence<CAirlineIcaoCode>,
        public swift::misc::db::IDatastoreObjectList<CAirlineIcaoCode, CAirlineIcaoCodeList, int>,
        public swift::misc::mixin::MetaType<CAirlineIcaoCodeList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAirlineIcaoCodeList)
        using CSequence::CSequence;

        //! Categories
        static const QStringList &getLogCategories();

        //! Default constructor.
        CAirlineIcaoCodeList();

        //! Construct from a base class object.
        CAirlineIcaoCodeList(const CSequence<CAirlineIcaoCode> &other);

        //! Construct from initializer list.
        CAirlineIcaoCodeList(std::initializer_list<CAirlineIcaoCode> il);

        //! Find by designator
        //! Not unique because of virtual airlines
        CAirlineIcaoCodeList findByDesignator(const QString &designator) const;

        //! Find by v-designator, this should be unique
        CAirlineIcaoCodeList findByVDesignator(const QString &designator) const;

        //! Find by ICAO code if this is unique, otherwise return default object
        CAirlineIcaoCode findByUniqueVDesignatorOrDefault(const QString &designator, bool preferOperatingAirlines) const;

        //! Find by IATA code
        //! Not unique because of virtual airlines and ceased airlines
        CAirlineIcaoCodeList findByIataCode(const QString &iata) const;

        //! Find by IATA code if this is unique, otherwise return default object
        CAirlineIcaoCode findByUniqueIataCodeOrDefault(const QString &iata) const;

        //! Find by designator or IATA code
        CAirlineIcaoCodeList findByDesignatorOrIataCode(const QString &designatorOrIata) const;

        //! Find by v-designator or IATA code
        CAirlineIcaoCodeList findByVDesignatorOrIataCode(const QString &designatorOrIata) const;

        //! Find by country code
        CAirlineIcaoCodeList findByCountryIsoCode(const QString &isoCode) const;

        //! Find if simplified name contains search string
        CAirlineIcaoCodeList findBySimplifiedNameContaining(const QString &containedString) const;

        //! Find by names or telephony designator (aka callsign, not to be confused with CCallsign)
        CAirlineIcaoCodeList findByTelephonyDesignator(const QString &candidate) const;

        //! Find by names or telephony designator (aka callsign, not to be confused with CCallsign)
        CAirlineIcaoCodeList findByNamesOrTelephonyDesignator(const QString &candidate) const;

        //! Find by military flag
        CAirlineIcaoCodeList findByMilitary(bool military) const;

        //! The ones with a valid designator
        CAirlineIcaoCodeList findByValidDesignator() const;

        //! The ones with an invalid designator
        CAirlineIcaoCodeList findByInvalidDesignator() const;

        //! Use callsign to conclude airline
        CAirlineIcaoCode findBestMatchByCallsign(const CCallsign &callsign) const;

        //! Best selection by given pattern
        CAirlineIcaoCode smartAirlineIcaoSelector(const CAirlineIcaoCode &icaoPattern, const CCallsign &callsign) const;

        //! Reduce by airline name/telephone designator, ISO country
        CAirlineIcaoCodeList ifPossibleReduceNameTelephonyCountry(const swift::misc::aviation::CCallsign &cs, const QString &airlineName, const QString &telephony, const QString &countryIso, bool &reduced, const QString &logInfo, CStatusMessageList *log) const;

        //! Reduce by ISO country
        CAirlineIcaoCodeList ifPossibleReduceByCountry(const QString &countryIso) const;

        //! Reduce by telephony designator
        CAirlineIcaoCodeList ifPossibleReduceByTelephonyDesignator(const QString &telephonyDesignator) const;

        //! String list for completion by ICAO designator
        QStringList toIcaoDesignatorCompleterStrings(bool combinedString = true, bool sort = true) const;

        //! String list for completion by ICAO designator plus Name
        QStringList toIcaoDesignatorNameCountryCompleterStrings(bool sort = true) const;

        //! String list for completion by name
        QStringList toNameCompleterStrings(bool sort = true) const;

        //! All designators
        QSet<QString> allDesignators() const;

        //! All designators
        QSet<QString> allVDesignators() const;

        //! Contains given designator?
        bool containsDesignator(const QString &designator) const;

        //! Contains given designator?
        bool containsVDesignator(const QString &vDesignator) const;

        //! To id map
        AirlineIcaoIdMap toIdMap() const;

        //! From our DB JSON
        static CAirlineIcaoCodeList fromDatabaseJson(const QJsonArray &array, bool ignoreIncomplete = true, CAirlineIcaoCodeList *inconsistent = nullptr);
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::aviation::CAirlineIcaoCodeList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CAirlineIcaoCode>)

#endif // guard
