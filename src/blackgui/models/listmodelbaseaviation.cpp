// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "listmodelbase.cpp"

namespace BlackGui::Models
{
    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    template class CListModelBase<BlackMisc::Aviation::CLiveryList, true>;
    template class CListModelBase<BlackMisc::Aviation::CAtcStationList, true>;
    template class CListModelBase<BlackMisc::Aviation::CAirportList, true>;
    template class CListModelBase<BlackMisc::Aviation::CAircraftIcaoCodeList, true>;
    template class CListModelBase<BlackMisc::Aviation::CAircraftCategoryList, true>;
    template class CListModelBase<BlackMisc::Aviation::CAirlineIcaoCodeList, true>;
    template class CListModelBase<BlackMisc::Aviation::CAircraftPartsList, true>;
    template class CListModelBase<BlackMisc::Aviation::CAircraftSituationList, true>;
    template class CListModelBase<BlackMisc::Aviation::CAircraftSituationChangeList, true>;
} // namespace
