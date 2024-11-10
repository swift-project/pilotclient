// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/aircraftcategoryview.h"
#include "gui/views/viewbase.h"

using namespace swift::misc;
using namespace swift::gui::models;

namespace swift::gui::views
{
    CAircraftCategoryView::CAircraftCategoryView(QWidget *parent) : CViewWithDbObjects(parent)
    {
        this->standardInit(new CAircraftCategoryListModel(this));
        this->setMenu(MenuDefaultDbViews);
    }
} // namespace
