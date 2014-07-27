/* Copyright (C) 2013
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airportlistmodel.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QMetaProperty>
#include <QBrush>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackGui
{
    namespace Models
    {
        /*
         * Constructor
         */
        CAirportListModel::CAirportListModel(QObject *parent) :
            CListModelBase<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>("ViewAirportList", parent)
        {
            this->m_columns.addColumn(CColumn("ICAO", CAirport::IndexIcao));
            this->m_columns.addColumn(CColumn("distance", CAirport::IndexDistance, Qt::AlignRight | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("bearing", CAirport::IndexBearing, Qt::AlignRight | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("name", CAirport::IndexDescriptiveName));
            this->m_columns.addColumn(CColumn("elevation", CAirport::IndexElevation, Qt::AlignRight | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("latitude", CCoordinateGeodetic::IndexLatitude, Qt::AlignRight | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("longitude", CCoordinateGeodetic::IndexLongitude, Qt::AlignRight  | Qt::AlignVCenter));

            // default sort order
            this->setSortColumnByPropertyIndex(CAirport::IndexDistance);
            this->m_sortOrder = Qt::AscendingOrder;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewAirportList", "ICAO");
            (void)QT_TRANSLATE_NOOP("ViewAirportList", "distance");
            (void)QT_TRANSLATE_NOOP("ViewAirportList", "name");
            (void)QT_TRANSLATE_NOOP("ViewAirportList", "elevation");
            (void)QT_TRANSLATE_NOOP("ViewAirportList", "bearing");
        }
    }
}
