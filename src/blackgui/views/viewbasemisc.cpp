/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
