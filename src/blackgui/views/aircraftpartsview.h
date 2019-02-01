/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_AIRCRAFTPARTSVIEW_H
#define BLACKGUI_VIEWS_AIRCRAFTPARTSVIEW_H

#include "blackgui/views/viewtimestampobjects.h"
#include "blackgui/models/aircraftpartslistmodel.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui
{
    namespace Views
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
} // ns

#endif // guard
