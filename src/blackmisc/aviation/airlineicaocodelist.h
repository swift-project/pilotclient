/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRLINEICAOCODELIST_H
#define BLACKMISC_AVIATION_AIRLINEICAOCODELIST_H

#include "airlineicaocode.h"
#include "callsign.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"

#include <QJsonArray>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <tuple>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of ICAO codes.
        class BLACKMISC_EXPORT CAirlineIcaoCodeList :
            public CSequence<CAirlineIcaoCode>,
            public BlackMisc::Db::IDatastoreObjectList<CAirlineIcaoCode, CAirlineIcaoCodeList, int>,
            public BlackMisc::Mixin::MetaType<CAirlineIcaoCodeList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAirlineIcaoCodeList)
            using CSequence::CSequence;

            //! Categories
            static const CLogCategoryList &getLogCategories();

            //! Default constructor.
            CAirlineIcaoCodeList();

            //! Construct from a base class object.
            CAirlineIcaoCodeList(const CSequence<CAirlineIcaoCode> &other);

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
            CAirlineIcaoCodeList ifPossibleReduceNameTelephonyCountry(const BlackMisc::Aviation::CCallsign &cs, const QString &airlineName, const QString &telephony, const QString &countryIso, bool &reduced, const QString &logInfo, CStatusMessageList *log) const;

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
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirlineIcaoCodeList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAirlineIcaoCode>)

#endif //guard
