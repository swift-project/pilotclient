// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/aircraftsituationchangeview.h"

using namespace swift::gui::models;

namespace swift::gui::views
{
    CAircraftSituationChangeView::CAircraftSituationChangeView(QWidget *parent) : CViewWithTimestampWithOffsetObjects(parent)
    {
        this->standardInit(new CAircraftSituationChangeListModel(this));
        this->setMenu(MenuDefault);
    }
} // namespace swift::gui::views
