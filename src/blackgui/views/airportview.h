/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRPORTVIEW_H
#define BLACKGUI_AIRPORTVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/airportlistmodel.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/aviation/airportlist.h"

class QWidget;

namespace BlackMisc { namespace Aviation { class CAirport; } }

namespace BlackGui
{
    namespace Views
    {
        //! Airports view
        class BLACKGUI_EXPORT CAirportView : public CViewBase<Models::CAirportListModel, BlackMisc::Aviation::CAirportList, BlackMisc::Aviation::CAirport>
        {
        public:

            //! Constructor
            explicit CAirportView(QWidget *parent = nullptr);
        };
    }
} // ns

#endif // guard
