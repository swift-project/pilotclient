/* Copyright (C) 2019
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
    CAircraftCategoryListModel::CAircraftCategoryListModel(QObject *parent) :
        CListModelDbObjects("AircraftCategoryListModel", parent)
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
