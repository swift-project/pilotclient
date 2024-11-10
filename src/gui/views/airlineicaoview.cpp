// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/airlineicaolistmodel.h"
#include "gui/views/airlineicaoview.h"
#include "gui/views/viewbase.h"

using namespace swift::misc;
using namespace swift::gui::models;

namespace swift::gui::views
{
    CAirlineIcaoCodeView::CAirlineIcaoCodeView(QWidget *parent) : CViewWithDbObjects(parent)
    {
        this->standardInit(new CAirlineIcaoCodeListModel(this));
        this->setMenu(MenuDefaultDbViews);
    }
} // namespace
