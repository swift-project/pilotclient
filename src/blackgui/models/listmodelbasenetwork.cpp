// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "listmodelbase.cpp"

namespace BlackGui::Models
{
    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    template class CListModelBase<BlackMisc::Network::CServerList, true>;
    template class CListModelBase<BlackMisc::Network::CUserList, true>;
    template class CListModelBase<BlackMisc::Network::CTextMessageList, true>;
    template class CListModelBase<BlackMisc::Network::CClientList, false>;

} // namespace
