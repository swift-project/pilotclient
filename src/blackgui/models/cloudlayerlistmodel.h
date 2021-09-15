/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_CLOUDLAYERLISTMODEL_H
#define BLACKGUI_MODELS_CLOUDLAYERLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/weather/cloudlayer.h"
#include "blackmisc/weather/cloudlayerlist.h"

class QObject;

namespace BlackGui::Models
{
    //! Cloud layer list model
    class BLACKGUI_EXPORT CCloudLayerListModel :
        public CListModelBase<BlackMisc::Weather::CCloudLayerList, false>
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
