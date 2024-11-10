// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "listmodelbase.cpp"

namespace BlackGui::Models
{
    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    template class CListModelBase<swift::misc::aviation::CLiveryList, true>;
    template class CListModelBase<swift::misc::aviation::CAtcStationList, true>;
    template class CListModelBase<swift::misc::aviation::CAirportList, true>;
    template class CListModelBase<swift::misc::aviation::CAircraftIcaoCodeList, true>;
    template class CListModelBase<swift::misc::aviation::CAircraftCategoryList, true>;
    template class CListModelBase<swift::misc::aviation::CAirlineIcaoCodeList, true>;
    template class CListModelBase<swift::misc::aviation::CAircraftPartsList, true>;
    template class CListModelBase<swift::misc::aviation::CAircraftSituationList, true>;
    template class CListModelBase<swift::misc::aviation::CAircraftSituationChangeList, true>;
} // namespace
