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
        this->m_columns.addColumn(CAircraft::IndexCallsign, "callsign");
        this->m_columns.addColumn(CAircraft::IndexPilotRealName, "pilotrealname");
        this->m_columns.addColumn(CAircraft::IndexDistance, "distance", Qt::AlignRight | Qt::AlignVCenter);
        this->m_columns.addColumn(CAircraft::IndexFrequencyCom1, "frequency", Qt::AlignRight | Qt::AlignVCenter);
        this->m_columns.addColumn(CAircraftIcao::IndexAsString, "model");
        this->m_columns.addColumn(CAircraft::IndexTansponderFormatted, "transponder");
        this->m_columns.addColumn(CAircraftSituation::IndexPositionLatitude, "latitude", Qt::AlignRight | Qt::AlignVCenter);
        this->m_columns.addColumn(CAircraftSituation::IndexPositionLongitude, "longitude", Qt::AlignRight  | Qt::AlignVCenter);
        this->m_columns.addColumn(CAircraftSituation::IndexAltitude, "altitude", Qt::AlignRight  | Qt::AlignVCenter);
        this->m_columns.addColumn(CAircraftSituation::IndexGroundspeed, "groundspeed", Qt::AlignRight  | Qt::AlignVCenter);

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
