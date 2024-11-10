// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/airportlistmodel.h"
#include "blackgui/views/airportview.h"

using namespace swift::misc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CAirportView::CAirportView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CAirportListModel(this));
    }
} // namespace
