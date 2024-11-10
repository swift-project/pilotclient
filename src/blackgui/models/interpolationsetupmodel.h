// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_INTERPOLATIONSETUPMODEL_H
#define BLACKGUI_MODELS_INTERPOLATIONSETUPMODEL_H

#include "misc/simulation/interpolation/interpolationsetuplist.h"
#include "blackgui/models/listmodelcallsignobjects.h"
#include "blackgui/blackguiexport.h"
#include <QString>

namespace BlackGui::Models
{
    //! Country list model
    class BLACKGUI_EXPORT CInterpolationSetupListModel :
        public CListModelCallsignObjects<swift::misc::simulation::CInterpolationSetupList, false>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInterpolationSetupListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CInterpolationSetupListModel() override {}
    };
} // ns
#endif // guard
