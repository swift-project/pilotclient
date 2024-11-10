// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_INTERPOLATIONSETUPVIEW_H
#define BLACKGUI_VIEWS_INTERPOLATIONSETUPVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/interpolationsetupmodel.h"
#include "blackgui/views/viewcallsignobjects.h"
#include "misc/countrylist.h"

class QWidget;

namespace BlackGui::Views
{
    //! Distributors
    class BLACKGUI_EXPORT CInterpolationSetupView : public CViewWithCallsignObjects<Models::CInterpolationSetupListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInterpolationSetupView(QWidget *parent = nullptr);
    };
} // ns
#endif // guard
