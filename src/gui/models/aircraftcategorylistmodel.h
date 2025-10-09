// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_AIRCRAFTCATEGORYLISTMODEL_H
#define SWIFT_GUI_MODELS_AIRCRAFTCATEGORYLISTMODEL_H

#include "gui/models/listmodeldbobjects.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/aircraftcategorylist.h"

namespace swift::gui::models
{
    //! Airport list model
    class SWIFT_GUI_EXPORT CAircraftCategoryListModel :
        public CListModelDbObjects<swift::misc::aviation::CAircraftCategoryList, int, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftCategoryListModel(QObject *parent = nullptr);

        //! Destructor
        ~CAircraftCategoryListModel() override = default;
    };
} // namespace swift::gui::models
#endif // SWIFT_GUI_MODELS_AIRCRAFTCATEGORYLISTMODEL_H
