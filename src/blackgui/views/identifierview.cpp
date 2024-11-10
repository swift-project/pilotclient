// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/identifierlistmodel.h"
#include "blackgui/views/identifierview.h"

using namespace swift::misc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CIdentifierView::CIdentifierView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CIdentifierListModel(this));
    }
} // namespace
