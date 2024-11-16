// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_LIVERYLIST_H
#define SWIFT_MISC_AVIATION_LIVERYLIST_H

#include <QMetaType>
#include <QString>
#include <QStringList>

#include "misc/aviation/airlineicaocodelist.h"
#include "misc/aviation/livery.h"
#include "misc/collection.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CLivery, CLiveryList)

namespace swift::misc::aviation
{
    //! Value object for a list of airports.
    class SWIFT_MISC_EXPORT CLiveryList :
        public CSequence<CLivery>,
        public db::IDatastoreObjectList<CLivery, CLiveryList, int>,
        public mixin::MetaType<CLiveryList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CLiveryList)
        using CSequence::CSequence;

        //! Default constructor.
        CLiveryList();

        //! Construct from a base class object.
        CLiveryList(const CSequence<CLivery> &other);

        //! Find livery by airline
        //! \remark try to use the version with CAirlineIcaoCode as it resolves ambiguities betters
        CLiveryList findByAirlineIcaoDesignator(const QString &icao) const;

        //! Find livery by airline
        //! \remark try to use the version with CAirlineIcaoCode as it resolves ambiguities betters
        CLivery findStdLiveryByAirlineIcaoVDesignator(const QString &icao) const;

        //! Find livery by airline
        CLivery findStdLiveryByAirlineIcaoVDesignator(const CAirlineIcaoCode &icao) const;

        //! Search for colors
        CLivery findColorLiveryOrDefault(const CRgbColor &fuselage, const CRgbColor &tail) const;

        //! Search for colors
        CLivery findClosestColorLiveryOrDefault(const CRgbColor &fuselage, const CRgbColor &tail) const;

        //! By simplified name
        CLiveryList findStdLiveriesBySimplifiedAirlineName(const QString &containedString) const;

        //! By names or telephony designator(aka callsign)
        CLiveryList findStdLiveriesByNamesOrTelephonyDesignator(const QString &candidate) const;

        //! Find livery by combined code
        CLivery findByCombinedCode(const QString &combinedCode) const;

        //! All combined codes
        QStringList getCombinedCodes(bool sort = false) const;

        //! All combined codes plus more info
        QStringList getCombinedCodesPlusInfo(bool sort = false) const;

        //! All combined codes plus more info
        QStringList getCombinedCodesPlusInfoAndId(bool sort = false) const;

        //! All aircraft codes
        CAirlineIcaoCodeList getAirlines() const;

        //! Find by multiple criteria
        CLivery smartLiverySelector(const CLivery &liveryPattern) const;

        //! Caching version from DB data
        //! \param array JSON data
        //! \param relatedAirlines passing the airline can skip the parsing from livery
        //! \remark without passing related airlines there is not much sense using this function, as most airlines/liveries have a 1:1 ratio
        static CLiveryList fromDatabaseJsonCaching(const QJsonArray &array, const CAirlineIcaoCodeList &relatedAirlines = {});
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CLiveryList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CLivery>)

#endif // guard
