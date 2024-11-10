// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_AIRCRAFTPARTSVIEW_H
#define SWIFT_GUI_VIEWS_AIRCRAFTPARTSVIEW_H

#include "gui/views/viewtimestampobjects.h"
#include "gui/models/aircraftpartslistmodel.h"
#include "gui/swiftguiexport.h"

namespace swift::gui::views
{
    //! Aircraft parts
    class SWIFT_GUI_EXPORT CAircraftPartsView : public CViewWithTimestampWithOffsetObjects<models::CAircraftPartsListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftPartsView(QWidget *parent = nullptr);
    };
} // ns

#endif // guard
