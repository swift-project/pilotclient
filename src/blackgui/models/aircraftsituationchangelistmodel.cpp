/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/aircraftsituationchangelistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/aviation/aircraftsituationchangelist.h"
#include "blackmisc/propertyindex.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui::Models
{
    CAircraftSituationChangeListModel::CAircraftSituationChangeListModel(QObject *parent) :
        CListModelTimestampWithOffsetObjects<CAircraftSituationChangeList, true>("ViewAircraftPartsList", parent)
    {
        this->addTimestampOffsetColumns();

        m_columns.addColumn(CColumn::standardString("s#", "situations", CAircraftSituationChange::IndexSituationsCount));

        m_columns.addColumn(CColumn("alt.mean", CAircraftSituationChange::IndexAltitudeMean, new CAltitudeFormatter()));
        m_columns.addColumn(CColumn("alt.dev.", CAircraftSituationChange::IndexAltitudeStdDev, new CAltitudeFormatter()));
        m_columns.addColumn(CColumn("elv.mean", CAircraftSituationChange::IndexElevationMean, new CAltitudeFormatter()));
        m_columns.addColumn(CColumn("elv.dev.", CAircraftSituationChange::IndexElevationStdDev, new CAltitudeFormatter()));

        m_columns.addColumn(CColumn("null", CAircraftSituationChange::IndexIsNull, new CBoolIconFormatter("null", "not null")));
        m_columns.addColumn(CColumn("gnd.", CAircraftSituationChange::IndexConstOnGround, new CBoolIconFormatter("const on gnd.", "not const on gnd.")));
        m_columns.addColumn(CColumn("n.g.", CAircraftSituationChange::IndexConstNotOnGround, new CBoolIconFormatter("const not on gnd.", "not const not on gnd.")));
        m_columns.addColumn(CColumn("asc.", CAircraftSituationChange::IndexConstAscending, new CBoolIconFormatter("const ascending", "not const ascending")));
        m_columns.addColumn(CColumn("desc", CAircraftSituationChange::IndexConstDescending, new CBoolIconFormatter("const decending", "not const decending")));
        m_columns.addColumn(CColumn("push", CAircraftSituationChange::IndexContainsPushBack, new CBoolIconFormatter("contains pushback", "no pushback")));
        m_columns.addColumn(CColumn("to.", CAircraftSituationChange::IndexJustTakingOff, new CBoolIconFormatter("just takeoff", "no takeoff")));
        m_columns.addColumn(CColumn("td.", CAircraftSituationChange::IndexJustTouchingDown, new CBoolIconFormatter("just touching down", "no touchdown")));

    }
} // namespace
