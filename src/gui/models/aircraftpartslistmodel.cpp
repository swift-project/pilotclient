// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/aircraftpartslistmodel.h"

#include <Qt>
#include <QtGlobal>

#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/aviation/aircraftpartslist.h"
#include "misc/propertyindex.h"

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::gui::models
{
    CAircraftPartsListModel::CAircraftPartsListModel(QObject *parent)
        : CListModelTimestampWithOffsetObjects<CAircraftPartsList, true>("ViewAircraftPartsList", parent)
    {
        this->addTimestampOffsetColumns();

        m_columns.addColumn(
            CColumn("gnd.", CAircraftParts::IndexIsOnGround, new CBoolIconFormatter("on gnd.", "not on gnd.")));
        m_columns.addColumn(
            CColumn("gear", CAircraftParts::IndexGearDown, new CBoolIconFormatter("gear down", "gear up")));
        m_columns.addColumn(
            CColumn("spoilers", CAircraftParts::IndexSpoilersOut, new CBoolIconFormatter("spoilers", "no spoilers")));
        m_columns.addColumn(CColumn("flaps", CAircraftParts::IndexFlapsPercentage, new CIntegerFormatter()));

        m_columns.addColumn(CColumn::standardString("engines", CAircraftParts::IndexEnginesAsString));
        m_columns.addColumn(
            CColumn::standardString("lights", { CAircraftParts::IndexLights, CAircraftParts::IndexString }));
    }
} // namespace swift::gui::models
