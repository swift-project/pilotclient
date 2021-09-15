/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "viewbase.cpp"

namespace BlackGui::Views
{
    template class CViewBase<BlackGui::Models::CAircraftPartsListModel>;
    template class CViewBase<BlackGui::Models::CAircraftSituationListModel>;
    template class CViewBase<BlackGui::Models::CAircraftSituationChangeListModel>;
    template class CViewBase<BlackGui::Models::CAirlineIcaoCodeListModel>;
    template class CViewBase<BlackGui::Models::CAircraftCategoryListModel>;
    template class CViewBase<BlackGui::Models::CAircraftIcaoCodeListModel>;
    template class CViewBase<BlackGui::Models::CAirportListModel>;
    template class CViewBase<BlackGui::Models::CAtcStationListModel>;
    template class CViewBase<BlackGui::Models::CLiveryListModel>;
} // namespace
