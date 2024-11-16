// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/clientview.h"

#include "gui/models/clientlistmodel.h"

using namespace swift::misc;
using namespace swift::gui::models;

namespace swift::gui::views
{
    CClientView::CClientView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CClientListModel(this));
    }
} // namespace swift::gui::views
