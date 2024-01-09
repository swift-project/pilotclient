// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/aircraftsituationlistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/propertyindex.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui::Models
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
