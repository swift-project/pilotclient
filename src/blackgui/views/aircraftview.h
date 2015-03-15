/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTVIEW_H
#define BLACKGUI_AIRCRAFTVIEW_H

#include "viewbase.h"
#include "../models/aircraftlistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! Aircrafts view
        //! \deprecated use simulated aircraft instead
        class CAircraftView : public CViewBase<Models::CAircraftListModel, BlackMisc::Aviation::CAircraftList, BlackMisc::Aviation::CAircraft>
        {

        public:
            //! Constructor
            explicit CAircraftView(QWidget *parent = nullptr);
        };

    } // namespace
} // namespace
#endif // guard
