// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/windlayerlistmodel.h"
#include "blackgui/views/windlayerview.h"

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CWindLayerView::CWindLayerView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CWindLayerListModel(this));
    }
} // namespace
