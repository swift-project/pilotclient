// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/audiodeviceinfolistmodel.h"
#include "blackgui/views/audiodeviceinfoview.h"

using namespace swift::misc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CAudioDeviceInfoView::CAudioDeviceInfoView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CAudioDeviceInfoListModel(this));
    }
} // namespace
