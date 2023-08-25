// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "viewbase.cpp"

namespace BlackGui::Views
{
    template class CViewBase<BlackGui::Models::CClientListModel>;
    template class CViewBase<BlackGui::Models::CServerListModel>;
    template class CViewBase<BlackGui::Models::CTextMessageListModel>;
    template class CViewBase<BlackGui::Models::CUserListModel>;
} // namespace
