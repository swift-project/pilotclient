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

        void CAircraftCategoryList::sortByRank()
        {
            this->sortBy(&CAircraftCategory::getPath);
        }

        CAircraftCategoryList CAircraftCategoryList::fromDatabaseJson(const QJsonArray &array)
        {
            CAircraftCategoryList codes;
            for (const QJsonValue &value : array)
            {
                const CAircraftCategory category(CAircraftCategory::fromDatabaseJson(value.toObject()));
                codes.push_back(category);
            }
            return codes;
        }

    } // namespace
} // namespace
