// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_COUNTRYLIST_H
#define SWIFT_MISC_COUNTRYLIST_H

#include <tuple>

#include <QJsonArray>
#include <QMetaType>
#include <QString>
#include <QStringList>

#include "misc/collection.h"
#include "misc/country.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc, CCountry, CCountryList)

namespace swift::misc
{
    //! Value object encapsulating a list of countries.
    //! \remark: I do not use CCollection as I want to sort per column
    class SWIFT_MISC_EXPORT CCountryList :
        public CSequence<CCountry>,
        public db::IDatastoreObjectList<CCountry, CCountryList, QString>,
        public mixin::MetaType<CCountryList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CCountryList)
        using CSequence::CSequence;

        //! Default constructor.
        CCountryList() = default;

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
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CCountryList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::CCountry>)

#endif // SWIFT_MISC_COUNTRYLIST_H
