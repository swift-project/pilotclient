// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/windlayerview.h"

#include "gui/models/windlayerlistmodel.h"

using namespace swift::misc;
using namespace swift::gui::models;

namespace swift::gui::views
{
    CWindLayerView::CWindLayerView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CWindLayerListModel(this));
    }
} // namespace swift::gui::views
