// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/listmodeltimestampobjects.h"

#include <type_traits>

#include <QModelIndex>

#include "gui/models/allmodelcontainers.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::gui::models
{
    template <typename T, bool UseCompare>
    CListModelTimestampObjects<T, UseCompare>::CListModelTimestampObjects(const QString &translationContext,
                                                                          QObject *parent)
        : CListModelBase<ContainerType, UseCompare>(translationContext, parent)
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
        CListModelBaseNonTemplate::m_columns.addColumn(
            CColumn::standardString("timestamp", ObjectType::IndexUtcTimestampFormattedMdhmsz));
        CListModelBaseNonTemplate::m_columns.addColumn(CColumn(
            "timestamp ms", "milliseconds since epoch", ObjectType::IndexMSecsSinceEpoch, new CIntegerFormatter()));
    }

    template class CListModelTimestampObjects<swift::misc::CStatusMessageList, true>;
    template class CListModelTimestampObjects<swift::misc::network::CTextMessageList, true>;
    template class CListModelTimestampObjects<swift::misc::aviation::CAircraftPartsList, true>;
    template class CListModelTimestampObjects<swift::misc::aviation::CAircraftSituationList, true>;
    template class CListModelTimestampObjects<swift::misc::aviation::CAircraftSituationChangeList, true>;

    template <typename T, bool UseCompare>
    CListModelTimestampWithOffsetObjects<T, UseCompare>::CListModelTimestampWithOffsetObjects(
        const QString &translationContext, QObject *parent)
        : CListModelTimestampObjects<ContainerType, UseCompare>(translationContext, parent)
    {}

    template <typename T, bool UseCompare>
    void
    CListModelTimestampWithOffsetObjects<T, UseCompare>::push_frontKeepLatestAdjustedFirst(const ObjectType &object,
                                                                                           int max)
    {
        this->beginInsertRows(QModelIndex(), 0, 0);
        CListModelBase<ContainerType, UseCompare>::m_container.push_frontKeepLatestAdjustedFirst(object, max);
        this->endInsertRows();
    }

    template <typename T, bool UseCompare>
    void CListModelTimestampWithOffsetObjects<T, UseCompare>::addTimestampOffsetColumns()
    {
        CListModelTimestampObjects<ContainerType, UseCompare>::addTimestampColumns();
        CListModelBaseNonTemplate::m_columns.addColumn(CColumn(
            "ms adj.", "milliseconds adjusted", ObjectType::IndexAdjustedMsWithOffset, new CIntegerFormatter()));
        CListModelBaseNonTemplate::m_columns.addColumn(
            CColumn("t.os.", "time offset", ObjectType::IndexOffsetMs, new CIntegerFormatter()));
    }

    template class CListModelTimestampWithOffsetObjects<swift::misc::aviation::CAircraftPartsList, true>;
    template class CListModelTimestampWithOffsetObjects<swift::misc::aviation::CAircraftSituationList, true>;
    template class CListModelTimestampWithOffsetObjects<swift::misc::aviation::CAircraftSituationChangeList, true>;
} // namespace swift::gui::models
