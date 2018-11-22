/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "viewbase.cpp"

namespace BlackGui
{
    namespace Views
    {
        template class CViewBase<BlackGui::Models::CClientListModel, BlackMisc::Network::CClientList, BlackMisc::Network::CClient>;
        template class CViewBase<BlackGui::Models::CServerListModel, BlackMisc::Network::CServerList, BlackMisc::Network::CServer>;
        template class CViewBase<BlackGui::Models::CTextMessageListModel, BlackMisc::Network::CTextMessageList, BlackMisc::Network::CTextMessage>;
        template class CViewBase<BlackGui::Models::CUserListModel, BlackMisc::Network::CUserList, BlackMisc::Network::CUser>;
    } // namespace
} // namespace
