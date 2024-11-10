// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_CLOUDLAYERLISTMODEL_H
#define BLACKGUI_MODELS_CLOUDLAYERLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "misc/weather/cloudlayer.h"
#include "misc/weather/cloudlayerlist.h"

class QObject;

namespace BlackGui::Models
{
    //! Cloud layer list model
    class BLACKGUI_EXPORT CCloudLayerListModel :
        public CListModelBase<swift::misc::weather::CCloudLayerList, false>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCloudLayerListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CCloudLayerListModel() {}
    };
}
#endif // guard
