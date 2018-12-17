/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_INTERPOLATIONSETUPMODEL_H
#define BLACKGUI_MODELS_INTERPOLATIONSETUPMODEL_H

#include "blackmisc/simulation/interpolationsetuplist.h"
#include "blackgui/models/listmodelcallsignobjects.h"
#include "blackgui/blackguiexport.h"
#include <QString>

namespace BlackGui
{
    namespace Models
    {
        //! Country list model
        class BLACKGUI_EXPORT CInterpolationSetupListModel :
            public CListModelCallsignObjects<BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign, BlackMisc::Simulation::CInterpolationSetupList, false>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CInterpolationSetupListModel(QObject *parent = nullptr);

            //! Destructor
            virtual ~CInterpolationSetupListModel() {}
        };
    } // ns
} // ns
#endif // guard
