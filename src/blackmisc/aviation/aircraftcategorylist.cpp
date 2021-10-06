/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackmisc/range.h"
#include "blackmisc/setbuilder.h"

#include <QJsonObject>
#include <QJsonValue>
#include <Qt>
#include <QMap>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Aviation, CAircraftCategory, CAircraftCategoryList)

namespace BlackMisc::Aviation
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
        CSetBuilder<QString> levels;
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
        return levels.values().join(separator);
    }

    QList<int> CAircraftCategoryList::getFirstLevels() const
    {
        CSetBuilder<int> levels;
        for (const CAircraftCategory &category : *this)
        {
            levels.insert(category.getFirstLevel());
        }
        QList<int> ll = levels;
        ll.removeOne(0);
        return ll;
    }

    QList<int> CAircraftCategoryList::getSecondLevels() const
    {
        CSetBuilder<int> levels;
        for (const CAircraftCategory &category : *this)
        {
            levels.insert(category.getSecondLevel());
        }
        QList<int> ll = levels;
        ll.removeOne(0);
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

    CAircraftCategoryList CAircraftCategoryList::findByLevel(const QList<int> &level, bool noRootNode) const
    {
        CAircraftCategoryList categories;
        if (level.isEmpty()) { return categories; }
        const int ls = level.size();
        for (const CAircraftCategory &category : *this)
        {
            if (noRootNode && ls == category.getDepth()) { continue; } // ignore root nodes
            if (category.matchesLevel(level))
            {
                categories.push_back(category);
            }
        }
        return categories;
    }

    CAircraftCategory CAircraftCategoryList::findByFullLevel(const QList<int> &level) const
    {
        if (level.size() != 3) { return {}; }
        for (const CAircraftCategory &category : *this)
        {
            if (category.isLevel(level[0], level[1], level[2])) { return category; }
        }
        return {};
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
            categories = this->findByLevel(levels, true);
        }
        categories.remove(category);
        return categories;
    }

    CAircraftCategoryList CAircraftCategoryList::findInParallelBranch(const CAircraftCategory &category) const
    {
        if (category.isNull() || this->isEmpty()) { return {}; }
        if (category.isFirstLevel()) { return {}; }
        const bool isL2 = category.getDepth() == 2;
        const QList<int> loopLevels = isL2 ? this->getFirstLevels() : this->getSecondLevels();
        if (loopLevels.isEmpty()) { return {}; }

        QList<int> level = category.getLevel();
        CAircraftCategoryList result;
        for (int l = loopLevels.front(); loopLevels.back() >= l; ++l)
        {
            level[isL2 ? 0 : 1] = l;
            if (category.isLevel(level)) { continue; } // ignore category itself
            const CAircraftCategory cat = this->findByFullLevel(level);
            if (!cat.isNull()) { result.push_back(cat); }
        }
        return result;
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
