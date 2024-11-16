// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_AIRCRAFTCATEGORYVIEW_H
#define SWIFT_GUI_VIEWS_AIRCRAFTCATEGORYVIEW_H

#include "gui/models/aircraftcategorylistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewdbobjects.h"

namespace swift::gui::views
{
    //! Aircraft ICAO codes view
    class SWIFT_GUI_EXPORT CAircraftCategoryView : public CViewWithDbObjects<models::CAircraftCategoryListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftCategoryView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views
#endif // guard
