// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/aircraftcategorylistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/timestampbased.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc::Aviation;

namespace BlackGui::Models
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
} // ns
