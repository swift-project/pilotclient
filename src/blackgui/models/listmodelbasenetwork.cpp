/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "listmodelbase.cpp"

namespace BlackGui
{
    namespace Models
    {
        // see here for the reason of thess forward instantiations
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        template class CListModelBase<BlackMisc::Network::CServer, BlackMisc::Network::CServerList, true>;
        template class CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList, true>;
        template class CListModelBase<BlackMisc::Network::CTextMessage, BlackMisc::Network::CTextMessageList, false>;
        template class CListModelBase<BlackMisc::Network::CClient, BlackMisc::Network::CClientList, false>;

    } // namespace
} // namespace
