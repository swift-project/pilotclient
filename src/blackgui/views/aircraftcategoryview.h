// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_AIRCRAFTCATEGORYVIEW_H
#define BLACKGUI_VIEWS_AIRCRAFTCATEGORYVIEW_H

#include "blackgui/models/aircraftcategorylistmodel.h"
#include "blackgui/views/viewdbobjects.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui::Views
{
    //! Aircraft ICAO codes view
    class BLACKGUI_EXPORT CAircraftCategoryView :
        public CViewWithDbObjects<Models::CAircraftCategoryListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftCategoryView(QWidget *parent = nullptr);
    };
} // ns
#endif // guard
