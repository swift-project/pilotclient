// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/aircraftcategorylistmodel.h"

#include <Qt>
#include <QtGlobal>

#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/db/datastore.h"
#include "misc/timestampbased.h"

using namespace swift::misc::aviation;

namespace swift::gui::models
{
    CAircraftCategoryListModel::CAircraftCategoryListModel(QObject *parent) : CListModelDbObjects("AircraftCategoryListModel", parent)
    {
        m_columns.addColumn(CColumn::standardString("id", CAircraftCategory::IndexDbIntegerKey, CDefaultFormatter::alignRightVCenter()));
        m_columns.addColumn(CColumn::standardString("level", CAircraftCategory::IndexLevelString));
        m_columns.addColumn(CColumn::standardString("path", CAircraftCategory::IndexPath));
        m_columns.addColumn(CColumn::standardString("name", CAircraftCategory::IndexName));
        m_columns.addColumn(CColumn::standardString("description", CAircraftCategory::IndexDescription));
        m_columns.addColumn(CColumn::standardString("changed", CAircraftCategory::IndexUtcTimestampFormattedYmdhms));

        // default sort order
        this->setSortColumnByPropertyIndex(CAircraftCategory::IndexLevelString);
        m_sortOrder = Qt::AscendingOrder;
    }
} // namespace swift::gui::models
