/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/audiodeviceinfolistmodel.h"
#include "blackgui/views/audiodeviceinfoview.h"

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CAudioDeviceInfoView::CAudioDeviceInfoView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CAudioDeviceInfoListModel(this));
    }
} // namespace
