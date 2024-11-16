// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "listmodelbase.cpp"

namespace swift::gui::models
{
    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    template class CListModelBase<swift::misc::network::CServerList, true>;
    template class CListModelBase<swift::misc::network::CUserList, true>;
    template class CListModelBase<swift::misc::network::CTextMessageList, true>;
    template class CListModelBase<swift::misc::network::CClientList, false>;

} // namespace swift::gui::models
