// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/clientlistmodel.h"
#include "blackgui/views/clientview.h"

using namespace swift::misc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CClientView::CClientView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CClientListModel(this));
    }
} // namespace
