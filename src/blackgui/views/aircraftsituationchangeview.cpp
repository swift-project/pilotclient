// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/views/aircraftsituationchangeview.h"

using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CAircraftSituationChangeView::CAircraftSituationChangeView(QWidget *parent) : CViewWithTimestampWithOffsetObjects(parent)
    {
        this->standardInit(new CAircraftSituationChangeListModel(this));
        this->setMenu(MenuDefault);
    }
} // ns
