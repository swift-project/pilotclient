/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_AIRCRAFTSITUATIONVIEW_H
#define BLACKGUI_VIEWS_AIRCRAFTSITUATIONVIEW_H

#include "blackgui/views/viewtimestampobjects.h"
#include "blackgui/models/aircraftsituationlistmodel.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/aircraftsituationlist.h"

class QWidget;

namespace BlackGui
{
    namespace Views
    {
        //! Aircraft situations view
        class BLACKGUI_EXPORT CAircraftSituationView : public CViewWithTimestampWithOffsetObjects<Models::CAircraftSituationListModel, BlackMisc::Aviation::CAircraftSituationList, BlackMisc::Aviation::CAircraftSituation>
        {
        public:
            //! Constructor
            explicit CAircraftSituationView(QWidget *parent = nullptr);
        };
    } // ns
} // ns
#endif // guard
