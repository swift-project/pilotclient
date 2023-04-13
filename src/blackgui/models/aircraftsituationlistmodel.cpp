/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
        m_columns.addColumn(CColumn("sc.os.", "scenery offset", CAircraftSituation::IndexSceneryOffset, new CPhysiqalQuantiyFormatter<CLengthUnit, CLength>(CLengthUnit::ft(), 1)));
        m_columns.addColumn(CColumn("latitude", CAircraftSituation::IndexLatitude, new CLatLonFormatter()));
        m_columns.addColumn(CColumn("longitude", CAircraftSituation::IndexLongitude, new CLatLonFormatter()));
        m_columns.addColumn(CColumn("gs.", CAircraftSituation::IndexGroundSpeed, new CSpeedKtsFormatter()));
        m_columns.addColumn(CColumn::standardString("PBH", "pitch bank heading", CAircraftSituation::IndexPBHInfo));
        m_columns.addColumn(CColumn("on gnd.", "is on gnd.", CAircraftSituation::IndexIsOnGround, new CBoolIconFormatter("yes", "no"), true));
        m_columns.addColumn(CColumn::standardString("reliability", CAircraftSituation::IndexOnGroundReliabilityString));
        m_columns.addColumn(CColumn::standardString("gnd.elv.", CAircraftSituation::IndexGroundElevationPlusInfo));
        m_columns.addColumn(CColumn::standardString("gnd.elv.alt.", { CAircraftSituation::IndexGroundElevationPlane, CElevationPlane::IndexGeodeticHeightAsString }));
        m_columns.addColumn(CColumn("elv.radius", { CAircraftSituation::IndexGroundElevationPlane, CElevationPlane::IndexRadius }, new CPhysiqalQuantiyFormatter<CLengthUnit, CLength>(CLengthUnit::m(), 1)));
        m_columns.addColumn(CColumn::standardString("cs.", "callsign", { CAircraftSituation::IndexCallsign, CCallsign::IndexCallsignStringAsSet }));

        // default sort order
        this->setSortColumnByPropertyIndex(CAircraftSituation::IndexAdjustedMsWithOffset);
        m_sortOrder = Qt::DescendingOrder;
    }
} // namespace
