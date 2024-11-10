// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "viewbase.cpp"

namespace swift::gui::views
{
    template class CViewBase<swift::gui::models::CAircraftPartsListModel>;
    template class CViewBase<swift::gui::models::CAircraftSituationListModel>;
    template class CViewBase<swift::gui::models::CAircraftSituationChangeListModel>;
    template class CViewBase<swift::gui::models::CAirlineIcaoCodeListModel>;
    template class CViewBase<swift::gui::models::CAircraftCategoryListModel>;
    template class CViewBase<swift::gui::models::CAircraftIcaoCodeListModel>;
    template class CViewBase<swift::gui::models::CAirportListModel>;
    template class CViewBase<swift::gui::models::CAtcStationListModel>;
    template class CViewBase<swift::gui::models::CLiveryListModel>;
} // namespace
