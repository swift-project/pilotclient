// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTCATEGORYLIST_H
#define SWIFT_MISC_AVIATION_AIRCRAFTCATEGORYLIST_H

#include "misc/aviation/aircraftcategory.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/collection.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

#include <QJsonArray>
#include <QMetaType>
#include <QSet>
#include <QList>
#include <tuple>

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CAircraftCategory, CAircraftCategoryList)

namespace swift::misc::aviation
{
    //! Value object encapsulating a list of ICAO codes.
    class SWIFT_MISC_EXPORT CAircraftCategoryList :
        public CSequence<CAircraftCategory>,
        public db::IDatastoreObjectList<CAircraftCategory, CAircraftCategoryList, int>,
        public mixin::MetaType<CAircraftCategoryList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAircraftCategoryList)
        using CSequence::CSequence;

        //! Default constructor.
        CAircraftCategoryList();

        //! Construct from a base class object.
        CAircraftCategoryList(const CSequence<CAircraftCategory> &other);

        //! Find by name
        CAircraftCategoryList findByName(const QString &name, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const;

        //! Sort by path
        void sortByPath();

        //! Sort by level
        void sortByLevel();

        //! Get all level strings
        QSet<QString> getLevelStrings() const;

        //! Get all level strings
        QString getLevelsString(const QString &separator = ", ") const;

        //! All levels sorted
        QList<int> getFirstLevels() const;

        //! All levels sorted
        QList<int> getSecondLevels() const;

        //! Find highest (top) level of categories
        CAircraftCategoryList findHighestLevels(const CAircraftCategoryList &categories);

        //! Find by first level
        CAircraftCategoryList findByFirstLevel(int level) const;

        //! Find by levels
        CAircraftCategoryList findByLevel(const QList<int> &level, bool noRootNode = false) const;

        //! Find by exact levels
        CAircraftCategory findByFullLevel(const QList<int> &level) const;

        //! Find first levels
        CAircraftCategoryList findFirstLevels() const;

        //! Find siblings
        //! \remark if level is 3.2, siblings are 3.1 and 3.3
        CAircraftCategoryList findSiblings(const CAircraftCategory &category) const;

        //! Find siblings
        //! \remark if level is 3.2, finds 1.2, 2.2, and 4.2
        CAircraftCategoryList findInParallelBranch(const CAircraftCategory &category) const;

        //! Remove by level
        int removeIfLevel(const QList<int> &level);

        //! With removed categories
        CAircraftCategoryList removedLevel(const QList<int> &level) const;

        //! From our database JSON format
        static CAircraftCategoryList fromDatabaseJson(const QJsonArray &array);
    };
} // namespace

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftCategoryList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CAircraftCategory>)

#endif // guard
