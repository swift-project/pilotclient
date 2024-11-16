// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "viewbase.cpp"

namespace swift::gui::views
{
    template class CViewBase<swift::gui::models::CAircraftModelListModel>;
    template class CViewBase<swift::gui::models::CDistributorListModel>;
    template class CViewBase<swift::gui::models::CInterpolationSetupListModel>;
    template class CViewBase<swift::gui::models::CMatchingStatisticsModel>;
    template class CViewBase<swift::gui::models::CSimulatedAircraftListModel>;
} // namespace swift::gui::views
