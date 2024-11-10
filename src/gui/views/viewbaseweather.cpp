// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "viewbase.cpp"

namespace swift::gui::views
{
    template class CViewBase<swift::gui::models::CCloudLayerListModel>;
    template class CViewBase<swift::gui::models::CWindLayerListModel>;
} // namespace
