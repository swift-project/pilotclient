// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_AIRCRAFTPARTSVIEW_H
#define BLACKGUI_VIEWS_AIRCRAFTPARTSVIEW_H

#include "blackgui/views/viewtimestampobjects.h"
#include "blackgui/models/aircraftpartslistmodel.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui::Views
{
    //! Aircraft parts
    class BLACKGUI_EXPORT CAircraftPartsView : public CViewWithTimestampWithOffsetObjects<Models::CAircraftPartsListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftPartsView(QWidget *parent = nullptr);
    };
} // ns

#endif // guard
