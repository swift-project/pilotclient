// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTICAOCODELIST_H
#define SWIFT_MISC_AVIATION_AIRCRAFTICAOCODELIST_H

#include <tuple>

#include <QJsonArray>
#include <QMetaType>
#include <QStringList>

#include "misc/aviation/aircrafticaocode.h"
#include "misc/collection.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CAircraftIcaoCode, CAircraftIcaoCodeList)

namespace swift::misc::aviation
{
    class CAircraftCategoryList;

    //! Value object encapsulating a list of ICAO codes.
    class SWIFT_MISC_EXPORT CAircraftIcaoCodeList :
        public CSequence<CAircraftIcaoCode>,
        public db::IDatastoreObjectList<CAircraftIcaoCode, CAircraftIcaoCodeList, int>,
        public mixin::MetaType<CAircraftIcaoCodeList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAircraftIcaoCodeList)
        using CSequence::CSequence;

        //! Default constructor.
        CAircraftIcaoCodeList();

        //! Construct from a base class object.
        CAircraftIcaoCodeList(const CSequence<CAircraftIcaoCode> &other);

        //! Construct from initializer list.
        CAircraftIcaoCodeList(std::initializer_list<CAircraftIcaoCode> il);

        //! Find by designator
        CAircraftIcaoCodeList findByDesignator(const QString &designator, int fuzzySearch = -1) const;

        //! Find by designator
        CAircraftIcaoCode findBestFuzzyMatchOrDefault(const QString &designator, int cutoff = 50) const;

        //! Find by IATA code
        CAircraftIcaoCodeList findByIataCode(const QString &iata, int fuzzySearch = -1) const;

        //! Find by family
        CAircraftIcaoCodeList findByFamily(const QString &family, int fuzzySearch = -1) const;

        //! Ones with a valid designator
        CAircraftIcaoCodeList findByValidDesignator() const;

        //! Ones with an invalid designator
        CAircraftIcaoCodeList findByInvalidDesignator() const;

        //! Find by ICAO/IATA code
        CAircraftIcaoCodeList findByDesignatorOrIataCode(const QString &icaoOrIata) const;

        //! Find by ICAO/IATA code or family
        CAircraftIcaoCodeList findByDesignatorIataOrFamily(const QString &icaoIataOrFamily) const;

        //! Find code ending with string, e.g. "738" finds "B738"
        //! \remark many users use wrong ICAO designators, one typical mistake is "738" for "B737"
        //! \remark consider to use findBestFuzzyMatchOrDefault
        CAircraftIcaoCodeList findEndingWith(const QString &icaoEnding) const;

        //! Find by manufacturer
        CAircraftIcaoCodeList findByManufacturer(const QString &manufacturer) const;

        //! Find by model description
        CAircraftIcaoCodeList findByDescription(const QString &description) const;

        //! Find matching by any model description
        CAircraftIcaoCodeList findMatchingByAnyDescription(const QString &description) const;

        //! Those with IATA code
        CAircraftIcaoCodeList findWithIataCode(bool removeWhenSameAsDesignator) const;

        //! Those with family
        CAircraftIcaoCodeList findWithFamily(bool removeWhenSameAsDesignator) const;

        //! By military flag
        CAircraftIcaoCodeList findByMilitaryFlag(bool military) const;

        //! Find by designator, then best match by rank
        CAircraftIcaoCode findFirstByDesignatorAndRank(const QString &designator) const;

        //! Contains designator?
        bool containsDesignator(const QString &designator) const;

        //! Best selection by given pattern, also searches IATA and family information
        CAircraftIcaoCode smartAircraftIcaoSelector(const CAircraftIcaoCode &icaoPattern) const;

        //! Group by designator and manufacturer
        CAircraftIcaoCodeList groupByDesignatorAndManufacturer() const;

        //! Sort by rank
        void sortByRank();

        //! Sort by designator first, then by rank
        void sortByDesignatorAndRank();

        //! Sort by designator first, then by manufacturer and rank
        void sortByDesignatorManufacturerAndRank();

        //! Remove invalid combined codes
        void removeInvalidCombinedCodes();

        //! Remove duplicates as marked by CAircraftIcaoCode::isDbDuplicate
        void removeDuplicates();

        //! For selection completion
        QStringList toCompleterStrings(bool withIataCodes = false, bool withFamily = false, bool withCategory = true,
                                       bool sort = true) const;

        //! All ICAO codes, no duplicates
        QSet<QString> allDesignators(bool noUnspecified = true) const;

        //! All ICAO codes and DB key, no duplicates
        QSet<QString> allDesignatorsAndKey(bool noUnspecified = true) const;

        //! All families, no duplicates
        QSet<QString> allFamilies() const;

        //! All manufacturers
        QSet<QString> allManufacturers(bool onlyKnownDesignators = true) const;

        //! Count by manufacturer
        QMap<QString, int> countManufacturers() const;

        //! Uses countManufacturers to find "most important" manufacturer
        QPair<QString, int> maxCountManufacturer() const;

        //! From our database JSON format
        static CAircraftIcaoCodeList fromDatabaseJson(const QJsonArray &array, const CAircraftCategoryList &categories,
                                                      bool ignoreIncompleteAndDuplicates = true,
                                                      CAircraftIcaoCodeList *inconsistent = nullptr);
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftIcaoCodeList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CAircraftIcaoCode>)

#endif // SWIFT_MISC_AVIATION_AIRCRAFTICAOCODELIST_H
