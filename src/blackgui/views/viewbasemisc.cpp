// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "viewbase.cpp"

namespace BlackGui::Views
{
    template class CViewBase<BlackGui::Models::CApplicationInfoListModel>;
    template class CViewBase<BlackGui::Models::CCountryListModel>;
    template class CViewBase<BlackGui::Models::CStatusMessageListModel>;
    template class CViewBase<BlackGui::Models::CIdentifierListModel>;
    template class CViewBase<BlackGui::Models::CNameVariantPairModel>;

    template class CViewBase<BlackGui::Models::CAudioDeviceInfoListModel>;

} // namespace
