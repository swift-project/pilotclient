/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_WINDLAYERLISTMODEL_H
#define BLACKGUI_MODELS_WINDLAYERLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/weather/windlayer.h"
#include "blackmisc/weather/windlayerlist.h"

class QObject;

namespace BlackGui::Models
{
    //! Wind layer list model
    class BLACKGUI_EXPORT CWindLayerListModel :
        public CListModelBase<BlackMisc::Weather::CWindLayerList, false>
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
