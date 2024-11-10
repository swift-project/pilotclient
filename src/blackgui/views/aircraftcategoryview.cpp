// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/views/aircraftcategoryview.h"
#include "blackgui/views/viewbase.h"

using namespace swift::misc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CAircraftCategoryView::CAircraftCategoryView(QWidget *parent) : CViewWithDbObjects(parent)
    {
        this->standardInit(new CAircraftCategoryListModel(this));
        this->setMenu(MenuDefaultDbViews);
    }
} // namespace
