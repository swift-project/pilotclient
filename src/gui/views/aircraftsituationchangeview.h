// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_AIRCRAFTSITUATIONCHANGEVIEW_H
#define SWIFT_GUI_VIEWS_AIRCRAFTSITUATIONCHANGEVIEW_H

#include "gui/models/aircraftsituationchangelistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewtimestampobjects.h"
#include "misc/aviation/aircraftsituationchangelist.h"

class QWidget;

namespace swift::gui::views
{
    //! Aircraft situation change view
    class SWIFT_GUI_EXPORT CAircraftSituationChangeView :
        public CViewWithTimestampWithOffsetObjects<models::CAircraftSituationChangeListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftSituationChangeView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views

#endif // SWIFT_GUI_VIEWS_AIRCRAFTSITUATIONCHANGEVIEW_H
