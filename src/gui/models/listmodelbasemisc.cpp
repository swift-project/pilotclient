// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "listmodelbase.cpp"

namespace swift::gui::models
{
    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    template class CListModelBase<swift::misc::CIdentifierList, false>;
    template class CListModelBase<swift::misc::CApplicationInfoList, true>;
    template class CListModelBase<swift::misc::CStatusMessageList, true>;
    template class CListModelBase<swift::misc::CNameVariantPairList, false>;
    template class CListModelBase<swift::misc::CCountryList, true>;

    template class CListModelBase<swift::misc::audio::CAudioDeviceInfoList, true>;

} // namespace
