/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/listmodeltimestampobjects.h"
#include "blackgui/models/allmodelcontainers.h"

#include <QModelIndex>
#include <type_traits>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui::Models
{
    template <typename T, bool UseCompare>
    CListModelTimestampObjects<T, UseCompare>::CListModelTimestampObjects(const QString &translationContext, QObject *parent) : CListModelBase<ContainerType, UseCompare>(translationContext, parent)
    {}

    template <typename T, bool UseCompare>
    bool CListModelTimestampObjects<T, UseCompare>::isSortedByTimestampProperty() const
    {
        const CPropertyIndex pi = this->getSortProperty();
        return ITimestampBased::canHandleIndex(pi);
    }

    template <typename T, bool UseCompare>
    void CListModelTimestampObjects<T, UseCompare>::addTimestampColumns()
    {
        CListModelBaseNonTemplate::m_columns.addColumn(CColumn::standardString("timestamp", ObjectType::IndexUtcTimestampFormattedMdhmsz));
        CListModelBaseNonTemplate::m_columns.addColumn(CColumn("timestamp ms", "milliseconds since epoch", ObjectType::IndexMSecsSinceEpoch, new CIntegerFormatter()));
    }

    template class CListModelTimestampObjects<BlackMisc::CStatusMessageList, true>;
    template class CListModelTimestampObjects<BlackMisc::Network::CTextMessageList, true>;
    template class CListModelTimestampObjects<BlackMisc::Aviation::CAircraftPartsList, true>;
    template class CListModelTimestampObjects<BlackMisc::Aviation::CAircraftSituationList, true>;
    template class CListModelTimestampObjects<BlackMisc::Aviation::CAircraftSituationChangeList, true>;

    template <typename T, bool UseCompare>
    CListModelTimestampWithOffsetObjects<T, UseCompare>::CListModelTimestampWithOffsetObjects(const QString &translationContext, QObject *parent) : CListModelTimestampObjects<ContainerType, UseCompare>(translationContext, parent)
    {}

    template <typename T, bool UseCompare>
    void CListModelTimestampWithOffsetObjects<T, UseCompare>::push_frontKeepLatestAdjustedFirst(const ObjectType &object, int max)
    {
        this->beginInsertRows(QModelIndex(), 0, 0);
        CListModelBase<ContainerType, UseCompare>::m_container.push_frontKeepLatestAdjustedFirst(object, max);
        this->endInsertRows();
    }

    template <typename T, bool UseCompare>
    void CListModelTimestampWithOffsetObjects<T, UseCompare>::addTimestampOffsetColumns()
    {
        CListModelTimestampObjects<ContainerType, UseCompare>::addTimestampColumns();
        CListModelBaseNonTemplate::m_columns.addColumn(CColumn("ms adj.", "milliseconds adjusted", ObjectType::IndexAdjustedMsWithOffset, new CIntegerFormatter()));
        CListModelBaseNonTemplate::m_columns.addColumn(CColumn("t.os.", "time offset", ObjectType::IndexOffsetMs, new CIntegerFormatter()));
    }

    template class CListModelTimestampWithOffsetObjects<BlackMisc::Aviation::CAircraftPartsList, true>;
    template class CListModelTimestampWithOffsetObjects<BlackMisc::Aviation::CAircraftSituationList, true>;
    template class CListModelTimestampWithOffsetObjects<BlackMisc::Aviation::CAircraftSituationChangeList, true>;
} // namespace
