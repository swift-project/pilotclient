// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/cloudlayerview.h"

#include "gui/models/cloudlayerlistmodel.h"

using namespace swift::misc;
using namespace swift::gui::models;

namespace swift::gui::views
{
    CCloudLayerView::CCloudLayerView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CCloudLayerListModel(this));
    }
} // namespace swift::gui::views
