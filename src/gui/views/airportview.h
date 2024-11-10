// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_AIRPORTVIEW_H
#define SWIFT_GUI_VIEWS_AIRPORTVIEW_H

#include "gui/swiftguiexport.h"
#include "gui/models/airportlistmodel.h"
#include "gui/views/viewbase.h"

namespace swift::gui::views
{
    //! Airports view
    class SWIFT_GUI_EXPORT CAirportView : public CViewBase<models::CAirportListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirportView(QWidget *parent = nullptr);
    };
} // ns

#endif // guard
