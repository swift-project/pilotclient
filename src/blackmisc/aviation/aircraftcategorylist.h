/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTCATEGORYLIST_H
#define BLACKMISC_AVIATION_AIRCRAFTCATEGORYLIST_H

#include "aircraftcategory.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"

#include <QJsonArray>
#include <QMetaType>
#include <QSet>
#include <QList>
#include <tuple>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of ICAO codes.
        class BLACKMISC_EXPORT CAircraftCategoryList :
            public CSequence<CAircraftCategory>,
            public Db::IDatastoreObjectList<CAircraftCategory, CAircraftCategoryList, int>,
            public Mixin::MetaType<CAircraftCategoryList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftCategoryList)
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
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftCategoryList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftCategory>)

#endif //guard
