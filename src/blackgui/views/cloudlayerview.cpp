// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/cloudlayerlistmodel.h"
#include "blackgui/views/cloudlayerview.h"

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CCloudLayerView::CCloudLayerView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CCloudLayerListModel(this));
    }
} // namespace
