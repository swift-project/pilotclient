// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "viewbase.cpp"

namespace swift::gui::views
{
    template class CViewBase<swift::gui::models::CApplicationInfoListModel>;
    template class CViewBase<swift::gui::models::CCountryListModel>;
    template class CViewBase<swift::gui::models::CStatusMessageListModel>;
    template class CViewBase<swift::gui::models::CIdentifierListModel>;
    template class CViewBase<swift::gui::models::CNameVariantPairModel>;

    template class CViewBase<swift::gui::models::CAudioDeviceInfoListModel>;

} // namespace
