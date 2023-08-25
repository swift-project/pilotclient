// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_AIRCRAFTICAOVIEW_H
#define BLACKGUI_VIEWS_AIRCRAFTICAOVIEW_H

#include "blackgui/models/aircrafticaolistmodel.h"
#include "blackgui/views/viewdbobjects.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui::Views
{
    //! Aircraft ICAO codes view
    class BLACKGUI_EXPORT CAircraftIcaoCodeView :
        public CViewWithDbObjects<Models::CAircraftIcaoCodeListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftIcaoCodeView(QWidget *parent = nullptr);
    };
} // ns
#endif // guard
