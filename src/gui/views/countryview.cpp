// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/countryview.h"

using namespace swift::gui::models;

namespace swift::gui::views
{
    CCountryView::CCountryView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CCountryListModel(this));
        this->setMenu(MenuDefaultDbViews);
    }
} // namespace swift::gui::views
