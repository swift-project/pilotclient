/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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

            //! All levels sorted
            QList<int> getFirstLevels() const;

            //! Find by first level
            CAircraftCategoryList findByFirstLevel(int level) const;

            //! Find first levels
            CAircraftCategoryList findFirstLevels() const;

            //! From our database JSON format
            static CAircraftCategoryList fromDatabaseJson(const QJsonArray &array);
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftCategoryList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftCategory>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraftCategory>)

#endif //guard
