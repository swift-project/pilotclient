// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
