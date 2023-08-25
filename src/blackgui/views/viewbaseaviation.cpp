// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
