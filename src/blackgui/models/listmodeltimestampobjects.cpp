/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "listmodeltimestampobjects.h"
#include "allmodelcontainers.h"

#include <QModelIndex>
#include <type_traits>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        template <typename ObjectType, typename ContainerType, bool UseCompare>
        CListModelTimestampObjects<ObjectType, ContainerType, UseCompare>::CListModelTimestampObjects(const QString &translationContext, QObject *parent) :
            CListModelBase<ObjectType, ContainerType, UseCompare>(translationContext, parent)
        { }

        template<typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelTimestampObjects<ObjectType, ContainerType, UseCompare>::push_frontKeepLatestAdjustedFirst(const ObjectType &object, int max)
        {
            this->beginInsertRows(QModelIndex(), 0, 0);
            CListModelBase<ObjectType, ContainerType, UseCompare>::m_container.push_frontKeepLatestAdjustedFirst(object, max);
            this->endInsertRows();
        }

        template<typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelTimestampObjects<ObjectType, ContainerType, UseCompare>::addTimestampOffsetColumns()
        {
            CListModelBaseNonTemplate::m_columns.addColumn(CColumn::standardString("timestamp", ObjectType::IndexUtcTimestampFormattedMdhmsz));
            CListModelBaseNonTemplate::m_columns.addColumn(CColumn("ms", ObjectType::IndexMSecsSinceEpoch, new CIntegerFormatter()));
            CListModelBaseNonTemplate::m_columns.addColumn(CColumn("ms adj.", ObjectType::IndexAdjustedMsWithOffset, new CIntegerFormatter()));
            CListModelBaseNonTemplate::m_columns.addColumn(CColumn("offset", ObjectType::IndexOffsetMs, new CIntegerFormatter()));
        }

        // see here for the reason of thess forward instantiations
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        template class CListModelTimestampObjects<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList, true>;
        template class CListModelTimestampObjects<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList, true>;

    } // namespace
} // namespace
