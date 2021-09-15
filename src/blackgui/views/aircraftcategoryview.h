/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
