/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_VIEWS_INTERPOLATIONSETUPVIEW_H
#define BLACKGUI_VIEWS_INTERPOLATIONSETUPVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/interpolationsetupmodel.h"
#include "blackgui/views/viewcallsignobjects.h"
#include "blackmisc/countrylist.h"

class QWidget;

namespace BlackGui
{
    namespace Views
    {
        //! Distributors
        class BLACKGUI_EXPORT CInterpolationSetupView : public CViewWithCallsignObjects<Models::CInterpolationSetupListModel, BlackMisc::Simulation::CInterpolationSetupList, BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign>
        {
        public:
            //! Constructor
            explicit CInterpolationSetupView(QWidget *parent = nullptr);
        };
    } // ns
} // ns
#endif // guard
