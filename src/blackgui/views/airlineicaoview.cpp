// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/airlineicaolistmodel.h"
#include "blackgui/views/airlineicaoview.h"
#include "blackgui/views/viewbase.h"

using namespace swift::misc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CAirlineIcaoCodeView::CAirlineIcaoCodeView(QWidget *parent) : CViewWithDbObjects(parent)
    {
        this->standardInit(new CAirlineIcaoCodeListModel(this));
        this->setMenu(MenuDefaultDbViews);
    }
} // namespace
