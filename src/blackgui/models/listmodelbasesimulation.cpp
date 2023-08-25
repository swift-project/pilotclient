// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "listmodelbase.cpp"

namespace BlackGui::Models
{
    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    template class CListModelBase<BlackMisc::Simulation::CAircraftModelList, true>;
    template class CListModelBase<BlackMisc::Simulation::CSimulatedAircraftList, true>;
    template class CListModelBase<BlackMisc::Simulation::CDistributorList, true>;
    template class CListModelBase<BlackMisc::Simulation::CInterpolationSetupList, false>;
    template class CListModelBase<BlackMisc::Simulation::CMatchingStatistics, true>;

} // namespace
