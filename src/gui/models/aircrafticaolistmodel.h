// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_AIRCRAFTICAOLISTMODEL_H
#define SWIFT_GUI_MODELS_AIRCRAFTICAOLISTMODEL_H

#include "gui/models/listmodeldbobjects.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/aircrafticaocodelist.h"

namespace swift::gui::models
{
    //! Airport list model
    class SWIFT_GUI_EXPORT CAircraftIcaoCodeListModel :
        public CListModelDbObjects<swift::misc::aviation::CAircraftIcaoCodeList, int, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftIcaoCodeListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftIcaoCodeListModel() {}
    };
} // namespace swift::gui::models

#endif // guard
