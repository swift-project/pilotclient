// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/airportlistmodel.h"
#include "gui/views/airportview.h"

using namespace swift::misc;
using namespace swift::gui::models;

namespace swift::gui::views
{
    CAirportView::CAirportView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CAirportListModel(this));
    }
} // namespace
