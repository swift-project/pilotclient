// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_AIRLINEICAOVIEW_H
#define BLACKGUI_AIRLINEICAOVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/airlineicaolistmodel.h"
#include "blackgui/views/viewdbobjects.h"

class QWidget;

namespace BlackMisc::Aviation
{
    class CAirlineIcaoCode;
    class CAirlineIcaoCodeList;
}

namespace BlackGui::Views
{
    //! Aircraft ICAO codes view
    class BLACKGUI_EXPORT CAirlineIcaoCodeView :
        public CViewWithDbObjects<Models::CAirlineIcaoCodeListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirlineIcaoCodeView(QWidget *parent = nullptr);
    };
}

#endif // guard
