/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "aircraftlistmodel.h"
#include "blackmisc/avaircraftsituation.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc::Aviation;

namespace BlackGui
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
