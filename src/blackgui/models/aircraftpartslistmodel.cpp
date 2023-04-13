/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
