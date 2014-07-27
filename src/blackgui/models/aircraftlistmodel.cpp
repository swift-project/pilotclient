/* Copyright (C) 2013
 * Swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftlistmodel.h"
#include "blackmisc/avaircraftsituation.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        /*
         * Constructor
         */
        CAircraftListModel::CAircraftListModel(QObject *parent) : CListModelBase("ViewAircraftList", parent)
        {
            this->m_columns.addColumn(CColumn("callsign", CAircraft::IndexCallsign));
            this->m_columns.addColumn(CColumn("pilotrealname", CAircraft::IndexPilotRealName));
            this->m_columns.addColumn(CColumn("distance", CAircraft::IndexDistance, Qt::AlignRight | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("frequency", CAircraft::IndexFrequencyCom1, Qt::AlignRight | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("model", CAircraftIcao::IndexAsString));
            this->m_columns.addColumn(CColumn("transponder", CAircraft::IndexTansponderFormatted));
            this->m_columns.addColumn(CColumn("latitude", CAircraftSituation::IndexPositionLatitude, Qt::AlignRight | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("longitude", CAircraftSituation::IndexPositionLongitude, Qt::AlignRight  | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("altitude", CAircraftSituation::IndexAltitude, Qt::AlignRight  | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("groundspeed", CAircraftSituation::IndexGroundspeed, Qt::AlignRight  | Qt::AlignVCenter));

            // default sort order
            this->setSortColumnByPropertyIndex(CAircraft::IndexDistance);
            this->m_sortOrder = Qt::AscendingOrder;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ViewAircraftList", "callsign");
            (void)QT_TRANSLATE_NOOP("ViewAircraftList", "pilotrealname");
            (void)QT_TRANSLATE_NOOP("ViewAircraftList", "latitude");
            (void)QT_TRANSLATE_NOOP("ViewAircraftList", "longitude");
            (void)QT_TRANSLATE_NOOP("ViewAircraftList", "altitude");
            (void)QT_TRANSLATE_NOOP("ViewAircraftList", "distance");
            (void)QT_TRANSLATE_NOOP("ViewAircraftList", "height");
            (void)QT_TRANSLATE_NOOP("ViewAircraftList", "transponder");
            (void)QT_TRANSLATE_NOOP("ViewAircraftList", "groundspeed");
            (void)QT_TRANSLATE_NOOP("ViewAircraftList", "model");
        }
    }
}
