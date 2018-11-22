/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "viewbase.cpp"

namespace BlackGui
{
    namespace Views
    {
        template class CViewBase<BlackGui::Models::CAircraftModelListModel, BlackMisc::Simulation::CAircraftModelList, BlackMisc::Simulation::CAircraftModel>;
        template class CViewBase<BlackGui::Models::CAircraftPartsListModel, BlackMisc::Aviation::CAircraftPartsList, BlackMisc::Aviation::CAircraftParts>;
        template class CViewBase<BlackGui::Models::CAircraftSituationListModel, BlackMisc::Aviation::CAircraftSituationList, BlackMisc::Aviation::CAircraftSituation>;
        template class CViewBase<BlackGui::Models::CAircraftSituationChangeListModel, BlackMisc::Aviation::CAircraftSituationChangeList, BlackMisc::Aviation::CAircraftSituationChange>;
        template class CViewBase<BlackGui::Models::CAirlineIcaoCodeListModel, BlackMisc::Aviation::CAirlineIcaoCodeList, BlackMisc::Aviation::CAirlineIcaoCode>;
        template class CViewBase<BlackGui::Models::CAircraftIcaoCodeListModel, BlackMisc::Aviation::CAircraftIcaoCodeList, BlackMisc::Aviation::CAircraftIcaoCode>;
        template class CViewBase<BlackGui::Models::CAirportListModel, BlackMisc::Aviation::CAirportList, BlackMisc::Aviation::CAirport>;
        template class CViewBase<BlackGui::Models::CAtcStationListModel, BlackMisc::Aviation::CAtcStationList, BlackMisc::Aviation::CAtcStation>;
        template class CViewBase<BlackGui::Models::CLiveryListModel, BlackMisc::Aviation::CLiveryList, BlackMisc::Aviation::CLivery>;
    } // namespace
} // namespace
