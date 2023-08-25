// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_AIRPORTVIEW_H
#define BLACKGUI_VIEWS_AIRPORTVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/airportlistmodel.h"
#include "blackgui/views/viewbase.h"

namespace BlackGui::Views
{
    //! Airports view
    class BLACKGUI_EXPORT CAirportView : public CViewBase<Models::CAirportListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirportView(QWidget *parent = nullptr);
    };
} // ns

#endif // guard
