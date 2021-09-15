/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_AIRPORTVIEW_H
#define BLACKGUI_VIEWS_AIRPORTVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/airportlistmodel.h"
#include "blackgui/views/viewbase.h"

namespace BlackGui::Views
{
    //! Airports view
    class BLACKGUI_EXPORT CAirportView : public CViewBase<Models::CAirportListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirportView(QWidget *parent = nullptr);
    };
} // ns

#endif // guard
