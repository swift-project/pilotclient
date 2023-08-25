// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "viewbase.cpp"

namespace BlackGui::Views
{
    template class CViewBase<BlackGui::Models::CAircraftModelListModel>;
    template class CViewBase<BlackGui::Models::CDistributorListModel>;
    template class CViewBase<BlackGui::Models::CInterpolationSetupListModel>;
    template class CViewBase<BlackGui::Models::CMatchingStatisticsModel>;
    template class CViewBase<BlackGui::Models::CSimulatedAircraftListModel>;
} // namespace
