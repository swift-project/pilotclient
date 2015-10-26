/* Copyright (C) 2013
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airportlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Models
    {
        CAirportListModel::CAirportListModel(QObject *parent) :
            CListModelBase("AirportListModel", parent)
        {
            this->m_columns.addColumn(CColumn::standardValueObject("ICAO", CAirport::IndexIcao));
            this->m_columns.addColumn(CColumn("distance", CAirport::IndexDistanceToOwnAircraft, new CAirspaceDistanceFormatter()));
            this->m_columns.addColumn(CColumn("bearing", CAirport::IndexBearing, new CAngleDegreeFormatter()));
            this->m_columns.addColumn(CColumn::standardString("name", CAirport::IndexDescriptiveName));
            this->m_columns.addColumn(CColumn("elevation", CAirport::IndexElevation, new CPhysiqalQuantiyFormatter<CLengthUnit, CLength>(CLengthUnit::ft(), 0)));
            this->m_columns.addColumn(CColumn("latitude", CAirport::IndexLatitude, new CLatLonFormatter()));
            this->m_columns.addColumn(CColumn("longitude", CAirport::IndexLatitude, new CLatLonFormatter()));

            // default sort order
            this->setSortColumnByPropertyIndex(CAirport::IndexDistanceToOwnAircraft);
            this->m_sortOrder = Qt::AscendingOrder;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ModelAirportList", "ICAO");
            (void)QT_TRANSLATE_NOOP("ModelAirportList", "distance");
            (void)QT_TRANSLATE_NOOP("ModelAirportList", "name");
            (void)QT_TRANSLATE_NOOP("ModelAirportList", "elevation");
            (void)QT_TRANSLATE_NOOP("ModelAirportList", "bearing");
        }
    } // ns
} // ns
