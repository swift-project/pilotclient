// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/aircraftsituationlistmodel.h"
#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/propertyindex.h"

#include <Qt>
#include <QtGlobal>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;

namespace swift::gui::models
{
    CAircraftSituationListModel::CAircraftSituationListModel(QObject *parent) : CListModelTimestampWithOffsetObjects<CAircraftSituationList, true>("ViewAircraftPartsList", parent)
    {
        this->addTimestampOffsetColumns();

        m_columns.addColumn(CColumn("altitude", CAircraftSituation::IndexAltitude, new CAltitudeFormatter()));
        m_columns.addColumn(CColumn("CG", CAircraftSituation::IndexCG, new CPhysiqalQuantiyFormatter<CLengthUnit, CLength>(CLengthUnit::ft(), 1)));
        m_columns.addColumn(CColumn("latitude", CAircraftSituation::IndexLatitude, new CLatLonFormatter()));
        m_columns.addColumn(CColumn("longitude", CAircraftSituation::IndexLongitude, new CLatLonFormatter()));
        m_columns.addColumn(CColumn("gs.", CAircraftSituation::IndexGroundSpeed, new CSpeedKtsFormatter()));
        m_columns.addColumn(CColumn::standardString("PBH", "pitch bank heading", CAircraftSituation::IndexPBHInfo));
        m_columns.addColumn(CColumn::standardString("gnd.elv.", CAircraftSituation::IndexGroundElevationPlusInfo));
        m_columns.addColumn(CColumn::standardString("gnd.elv.alt.", { CAircraftSituation::IndexGroundElevationPlane, CElevationPlane::IndexGeodeticHeightAsString }));
        m_columns.addColumn(CColumn("elv.radius", { CAircraftSituation::IndexGroundElevationPlane, CElevationPlane::IndexRadius }, new CPhysiqalQuantiyFormatter<CLengthUnit, CLength>(CLengthUnit::m(), 1)));
        m_columns.addColumn(CColumn::standardString("cs.", "callsign", { CAircraftSituation::IndexCallsign, CCallsign::IndexCallsignStringAsSet }));

        // default sort order
        this->setSortColumnByPropertyIndex(CAircraftSituation::IndexAdjustedMsWithOffset);
        m_sortOrder = Qt::DescendingOrder;
    }
} // namespace
