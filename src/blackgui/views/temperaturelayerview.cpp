// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/temperaturelayerlistmodel.h"
#include "blackgui/views/temperaturelayerview.h"

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CTemperatureLayerView::CTemperatureLayerView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CTemperatureLayerListModel(this));
    }
} // namespace
