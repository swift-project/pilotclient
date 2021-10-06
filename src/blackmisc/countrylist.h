/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COUNTRYLIST_H
#define BLACKMISC_COUNTRYLIST_H

#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/country.h"
#include "blackmisc/sequence.h"

#include <QJsonArray>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <tuple>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc, CCountry, CCountryList)

namespace BlackMisc
{
    //! Value object encapsulating a list of countries.
    //! \remark: I do not use CCollection as I want to sort per column
    class BLACKMISC_EXPORT CCountryList :
        public CSequence<CCountry>,
        public BlackMisc::Db::IDatastoreObjectList<CCountry, CCountryList, QString>,
        public BlackMisc::Mixin::MetaType<CCountryList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CCountryList)
        using CSequence::CSequence;

        //! Default constructor.
        CCountryList();

        //! Construct from a base class object.
        CCountryList(const CSequence<CCountry> &other);

        //! Find by ISO code
        CCountry findByIsoCode(const QString &isoCode) const;

        //! Find "best match" by country
        CCountry findBestMatchByCountryName(const QString &candidate) const;

        //! Find first by alias
        CCountry findFirstByAlias(const QString &alias) const;

        //! ISO/name string list
        QStringList toIsoNameList(bool sorted = false) const;

        //! Name/ISO string list
        QStringList toNameIsoList(bool sorted = false) const;

        //! Name string list
        QStringList toNameList(bool sorted = false) const;

        //! All ISO codes
        QStringList toIsoList(bool sorted = false) const;

        //! All ISO 3 codes
        QStringList toIso3List(bool sorted = false) const;

        //! From our database JSON format
        static CCountryList fromDatabaseJson(const QJsonArray &array);
    };
} //namespace

Q_DECLARE_METATYPE(BlackMisc::CCountryList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CCountry>)

#endif //guard
