// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/aircraftsituationchangelistmodel.h"
#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/aviation/aircraftsituationchangelist.h"
#include "misc/propertyindex.h"

#include <Qt>
#include <QtGlobal>

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::gui::models
{
    CAircraftSituationChangeListModel::CAircraftSituationChangeListModel(QObject *parent) : CListModelTimestampWithOffsetObjects<CAircraftSituationChangeList, true>("ViewAircraftPartsList", parent)
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
