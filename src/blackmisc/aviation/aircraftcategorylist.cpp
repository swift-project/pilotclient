/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackmisc/range.h"

#include <QJsonObject>
#include <QJsonValue>
#include <Qt>

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftCategoryList::CAircraftCategoryList()
        { }

        CAircraftCategoryList::CAircraftCategoryList(const CSequence<CAircraftCategory> &other) :
            CSequence<CAircraftCategory>(other)
        { }

        CAircraftCategoryList CAircraftCategoryList::findByName(const QString &name, Qt::CaseSensitivity cs) const
        {
            return this->findBy([&](const CAircraftCategory & category)
            {
                return category.matchesName(name, cs);
            });
        }

        void CAircraftCategoryList::sortByPath()
        {
            this->sortBy(&CAircraftCategory::getPath);
        }

        void CAircraftCategoryList::sortByLevel()
        {
            this->sort([](const CAircraftCategory & a, const CAircraftCategory & b)
            {
                const int c = a.compareByLevel(b);
                return c < 0;
            });
        }

        QSet<QString> CAircraftCategoryList::getLevelStrings() const
        {
            QSet<QString> levels;
            for (const CAircraftCategory &category : *this)
            {
                levels.insert(category.getLevelString());
            }
            return levels;
        }

        QList<int> CAircraftCategoryList::getFirstLevels() const
        {
            QSet<int> levels;
            for (const CAircraftCategory &category : *this)
            {
                levels.insert(category.getFirstLevel());
            }
            QList<int> ll = levels.toList();
            std::sort(ll.begin(), ll.end());
            return ll;
        }

        CAircraftCategoryList CAircraftCategoryList::findByFirstLevel(int level) const
        {
            CAircraftCategoryList categories;
            for (const CAircraftCategory &category : *this)
            {
                if (category.getFirstLevel() != level) { continue; }
            }
            return categories;
        }

        CAircraftCategoryList CAircraftCategoryList::findFirstLevels() const
        {
            return this->findBy(&CAircraftCategory::isFirstLevel, true);
        }

        CAircraftCategoryList CAircraftCategoryList::fromDatabaseJson(const QJsonArray &array)
        {
            CAircraftCategoryList categories;
            for (const QJsonValue &value : array)
            {
                const CAircraftCategory category(CAircraftCategory::fromDatabaseJson(value.toObject()));
                categories.push_back(category);
            }
            return categories;
        }

    } // namespace
} // namespace
