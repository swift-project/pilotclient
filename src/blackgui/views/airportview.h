/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_AIRPORTVIEW_H
#define BLACKGUI_AIRPORTVIEW_H

//! \file

#include "viewbase.h"
#include "../models/airportlistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! Airports view
        class CAirportView : public CViewBase<Models::CAirportListModel>
        {

        public:

            //! Constructor
            explicit CAirportView(QWidget *parent = nullptr);
        };
    }
}
#endif // guard
