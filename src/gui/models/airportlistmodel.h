// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_AIRPORTLISTMODEL_H
#define SWIFT_GUI_MODELS_AIRPORTLISTMODEL_H

#include "gui/models/listmodelbase.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/airportlist.h"
#include "misc/aviation/airport.h"

class QObject;

namespace swift::gui::models
{
    //! Airport list model
    class SWIFT_GUI_EXPORT CAirportListModel :
        public CListModelBase<swift::misc::aviation::CAirportList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirportListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CAirportListModel() override {}
    };
}
#endif // guard
