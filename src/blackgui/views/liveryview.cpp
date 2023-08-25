// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/views/liveryview.h"
#include "blackgui/views/viewbase.h"

using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CLiveryView::CLiveryView(QWidget *parent) : CViewWithDbObjects(parent)
    {
        this->standardInit(new CLiveryListModel(this));
        this->setMenu(MenuDefaultDbViews);
    }
} // namespace
