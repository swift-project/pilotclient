// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_WINDLAYERLISTMODEL_H
#define BLACKGUI_MODELS_WINDLAYERLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "misc/weather/windlayer.h"
#include "misc/weather/windlayerlist.h"

class QObject;

namespace BlackGui::Models
{
    //! Wind layer list model
    class BLACKGUI_EXPORT CWindLayerListModel :
        public CListModelBase<swift::misc::weather::CWindLayerList, false>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CWindLayerListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CWindLayerListModel() {}
    };
}
#endif // guard
