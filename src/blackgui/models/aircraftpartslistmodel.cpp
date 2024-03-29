// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/aircraftpartslistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/propertyindex.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui::Models
{
    CAircraftPartsListModel::CAircraftPartsListModel(QObject *parent) : CListModelTimestampWithOffsetObjects<CAircraftPartsList, true>("ViewAircraftPartsList", parent)
    {
        this->addTimestampOffsetColumns();

        m_columns.addColumn(CColumn("gnd.", CAircraftParts::IndexIsOnGround, new CBoolIconFormatter("on gnd.", "not on gnd.")));
        m_columns.addColumn(CColumn("gear", CAircraftParts::IndexGearDown, new CBoolIconFormatter("gear down", "gear up")));
        m_columns.addColumn(CColumn("spoilers", CAircraftParts::IndexSpoilersOut, new CBoolIconFormatter("spoilers", "no spoilers")));
        m_columns.addColumn(CColumn("flaps", CAircraftParts::IndexFlapsPercentage, new CIntegerFormatter()));

        m_columns.addColumn(CColumn::standardString("engines", CAircraftParts::IndexEnginesAsString));
        m_columns.addColumn(CColumn::standardString("lights", { CAircraftParts::IndexLights, CAircraftParts::IndexString }));
    }
} // namespace
