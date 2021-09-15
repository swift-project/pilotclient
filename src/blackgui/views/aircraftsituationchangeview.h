/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_AIRCRAFTSITUATIONCHANGEVIEW_H
#define BLACKGUI_VIEWS_AIRCRAFTSITUATIONCHANGEVIEW_H

#include "blackgui/views/viewtimestampobjects.h"
#include "blackgui/models/aircraftsituationchangelistmodel.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/aircraftsituationchangelist.h"

class QWidget;

namespace BlackGui::Views
{
    //! Aircraft situation change view
    class BLACKGUI_EXPORT CAircraftSituationChangeView : public CViewWithTimestampWithOffsetObjects<Models::CAircraftSituationChangeListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftSituationChangeView(QWidget *parent = nullptr);
    };
} // ns

#endif // guard
