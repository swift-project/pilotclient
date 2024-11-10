// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_AIRCRAFTCATEGORYLISTMODEL_H
#define BLACKGUI_MODELS_AIRCRAFTCATEGORYLISTMODEL_H

#include "misc/aviation/aircraftcategorylist.h"
#include "misc/aviation/aircraftcategory.h"
#include "blackgui/models/listmodeldbobjects.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui::Models
{
    //! Airport list model
    class BLACKGUI_EXPORT CAircraftCategoryListModel :
        public CListModelDbObjects<swift::misc::aviation::CAircraftCategoryList, int, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftCategoryListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftCategoryListModel() {}
    };
}
#endif // guard
