/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftlistmodel.h"
#include "blackmisc/nwuser.h"
#include "blackmisc/avaircraftsituation.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

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
            this->m_columns.addColumn(CColumn("pilotrealname", { CAircraft::IndexPilot, CUser::IndexRealName }));
            this->m_columns.addColumn(CColumn("distance", CAircraft::IndexDistance, Qt::AlignRight | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("frequency", { CAircraft::IndexCom1System, CComSystem::IndexActiveFrequency }, Qt::AlignRight | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("model", { CAircraft::IndexIcao, CAircraftIcao::IndexAsString}));
            this->m_columns.addColumn(CColumn("transponder", { CAircraft::IndexTransponder, CTransponder::IndexTransponderCodeAndModeFormatted }));
            this->m_columns.addColumn(CColumn("latitude", { CAircraft::IndexSituation, CAircraftSituation::IndexLatitude }, Qt::AlignRight | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("longitude", { CAircraft::IndexSituation, CAircraftSituation::IndexLongitude }, Qt::AlignRight  | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("altitude", { CAircraft::IndexSituation, CAircraftSituation::IndexAltitude }, Qt::AlignRight  | Qt::AlignVCenter));
            this->m_columns.addColumn(CColumn("groundspeed", { CAircraft::IndexSituation, CAircraftSituation::IndexGroundspeed }, Qt::AlignRight  | Qt::AlignVCenter));

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
