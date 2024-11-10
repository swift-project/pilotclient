// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "serverview.h"

using namespace swift::misc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CServerView::CServerView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CServerListModel(this));
    }
} // namespace
