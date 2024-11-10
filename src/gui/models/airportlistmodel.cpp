// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/airportlistmodel.h"
#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"

#include <Qt>
#include <QtGlobal>

using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;

namespace swift::gui::models
{
    CAirportListModel::CAirportListModel(QObject *parent) : CListModelBase("AirportListModel", parent)
    {
        m_columns.addColumn(CColumn::standardValueObject("ICAO", CAirport::IndexIcao));
        m_columns.addColumn(CColumn("distance", CAirport::IndexRelativeDistance, new CAirspaceDistanceFormatter()));
        m_columns.addColumn(CColumn("bearing", CAirport::IndexRelativeBearing, new CAngleDegreeFormatter()));
        m_columns.addColumn(CColumn::standardString("name", CAirport::IndexDescriptiveName));
        m_columns.addColumn(CColumn("elevation", CAirport::IndexElevation, new CAltitudeFormatter()));
        m_columns.addColumn(CColumn("latitude", CAirport::IndexLatitude, new CLatLonFormatter()));
        m_columns.addColumn(CColumn("longitude", CAirport::IndexLongitude, new CLatLonFormatter()));

        // default sort order
        this->setSortColumnByPropertyIndex(CAirport::IndexRelativeDistance);
        m_sortOrder = Qt::AscendingOrder;

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelAirportList", "ICAO");
        (void)QT_TRANSLATE_NOOP("ModelAirportList", "distance");
        (void)QT_TRANSLATE_NOOP("ModelAirportList", "name");
        (void)QT_TRANSLATE_NOOP("ModelAirportList", "elevation");
        (void)QT_TRANSLATE_NOOP("ModelAirportList", "bearing");
    }
} // ns
