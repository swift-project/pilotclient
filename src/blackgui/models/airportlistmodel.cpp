// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/airportlistmodel.h"
#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/units.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui::Models
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
