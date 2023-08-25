// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/views/aircraftpartsview.h"

using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CAircraftPartsView::CAircraftPartsView(QWidget *parent) : CViewWithTimestampWithOffsetObjects(parent)
    {
        this->standardInit(new CAircraftPartsListModel(this));
        this->setMenu(MenuDefault);
    }
} // ns
