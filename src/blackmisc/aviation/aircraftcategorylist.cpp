/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackmisc/range.h"

#include <QJsonObject>
#include <QJsonValue>
#include <Qt>
#include <QMap>

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

        QString CAircraftCategoryList::getLevelsString(const QString &separator) const
        {
            const QSet<QString> levels = this->getLevelStrings();
            if (levels.isEmpty()) {return {}; }
            return levels.toList().join(separator);
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

        CAircraftCategoryList CAircraftCategoryList::findHighestLevels(const CAircraftCategoryList &categories)
        {
            if (categories.isEmpty()) { return CAircraftCategoryList(); }
            QMap<int, CAircraftCategory> highestLevels;
            for (const CAircraftCategory &category : *this)
            {
                const int fl = category.getFirstLevel();
                if (highestLevels.contains(fl))
                {
                    if (highestLevels[fl].isHigherLevel(category))
                    {
                        highestLevels[fl] = category;
                    }
                }
                else
                {
                    highestLevels[fl] = category;
                }
            }

            CAircraftCategoryList topLevels;
            for (const CAircraftCategory &category : highestLevels.values())
            {
                topLevels.push_back(category);
            }
            return topLevels;
        }

        CAircraftCategoryList CAircraftCategoryList::findByFirstLevel(int level) const
        {
            CAircraftCategoryList categories;
            for (const CAircraftCategory &category : *this)
            {
                if (category.getFirstLevel() == level)
                {
                    categories.push_back(category);
                }
            }
            return categories;
        }

        CAircraftCategoryList CAircraftCategoryList::findByLevel(const QList<int> &level) const
        {
            CAircraftCategoryList categories;
            if (level.isEmpty()) { return categories; }
            for (const CAircraftCategory &category : *this)
            {
                if (category.matchesLevel(level))
                {
                    categories.push_back(category);
                }
            }
            return categories;
        }

        CAircraftCategoryList CAircraftCategoryList::findFirstLevels() const
        {
            return this->findBy(&CAircraftCategory::isFirstLevel, true);
        }

        CAircraftCategoryList CAircraftCategoryList::findSiblings(const CAircraftCategory &category) const
        {
            QList<int> levels = category.getLevel();
            CAircraftCategoryList categories;
            if (levels.size() < 2)
            {
                categories = this->findFirstLevels();
            }
            else
            {
                levels.removeLast();
                categories = this->findByLevel(levels);
            }
            categories.remove(category);
            return categories;
        }

        int CAircraftCategoryList::removeIfLevel(const QList<int> &level)
        {
            if (level.isEmpty()) { return 0; }
            const int c = this->size();
            const CAircraftCategoryList removed = this->removedLevel(level);
            const int delta = c - removed.size();
            if (delta > 0) { *this = removed; }
            return delta;
        }

        CAircraftCategoryList CAircraftCategoryList::removedLevel(const QList<int> &level) const
        {
            if (level.isEmpty()) { return *this; } // nothing removed
            CAircraftCategoryList removed;
            for (const CAircraftCategory &category : * this)
            {
                if (category.matchesLevel(level)) { continue; }
                removed.push_back(category);
            }
            return removed;
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
