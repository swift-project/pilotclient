// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/audiodeviceinfolistmodel.h"
#include "gui/views/audiodeviceinfoview.h"

using namespace swift::misc;
using namespace swift::gui::models;

namespace swift::gui::views
{
    CAudioDeviceInfoView::CAudioDeviceInfoView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CAudioDeviceInfoListModel(this));
    }
} // namespace
