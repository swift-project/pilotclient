// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_INTERPOLATIONSETUPMODEL_H
#define SWIFT_GUI_MODELS_INTERPOLATIONSETUPMODEL_H

#include "misc/simulation/interpolation/interpolationsetuplist.h"
#include "gui/models/listmodelcallsignobjects.h"
#include "gui/swiftguiexport.h"
#include <QString>

namespace swift::gui::models
{
    //! Country list model
    class SWIFT_GUI_EXPORT CInterpolationSetupListModel :
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
