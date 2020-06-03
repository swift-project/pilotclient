/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_LIVERYLIST_H
#define BLACKMISC_AVIATION_LIVERYLIST_H

#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>
#include <QStringList>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for a list of airports.
        class BLACKMISC_EXPORT CLiveryList :
            public CSequence<CLivery>,
            public Db::IDatastoreObjectList<CLivery, CLiveryList, int>,
            public Mixin::MetaType<CLiveryList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CLiveryList)
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
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CLiveryList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CLivery>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CLivery>)

#endif //guard
