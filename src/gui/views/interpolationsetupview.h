// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_INTERPOLATIONSETUPVIEW_H
#define SWIFT_GUI_VIEWS_INTERPOLATIONSETUPVIEW_H

#include "gui/models/interpolationsetupmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewcallsignobjects.h"
#include "misc/countrylist.h"

class QWidget;

namespace swift::gui::views
{
    //! Distributors
    class SWIFT_GUI_EXPORT CInterpolationSetupView :
        public CViewWithCallsignObjects<models::CInterpolationSetupListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInterpolationSetupView(QWidget *parent = nullptr);
    };
} // namespace swift::gui::views
#endif // SWIFT_GUI_VIEWS_INTERPOLATIONSETUPVIEW_H
