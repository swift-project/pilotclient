// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "interpolationsetupview.h"

using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CInterpolationSetupView::CInterpolationSetupView(QWidget *parent) : CViewWithCallsignObjects(parent)
    {
        this->standardInit(new CInterpolationSetupListModel(this));
        // this->setMenu(MenuDefaultDbViews);
    }
} // ns
