// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_AIRPORTLISTMODEL_H
#define BLACKGUI_MODELS_AIRPORTLISTMODEL_H

#include "blackgui/models/listmodelbase.h"
#include "blackgui/blackguiexport.h"
#include "misc/aviation/airportlist.h"
#include "misc/aviation/airport.h"

class QObject;

namespace BlackGui::Models
{
    //! Airport list model
    class BLACKGUI_EXPORT CAirportListModel :
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
