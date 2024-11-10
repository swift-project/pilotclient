// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/liveryview.h"
#include "gui/views/viewbase.h"

using namespace swift::gui::models;

namespace swift::gui::views
{
    CLiveryView::CLiveryView(QWidget *parent) : CViewWithDbObjects(parent)
    {
        this->standardInit(new CLiveryListModel(this));
        this->setMenu(MenuDefaultDbViews);
    }
} // namespace
