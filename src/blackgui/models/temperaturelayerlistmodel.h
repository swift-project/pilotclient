/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_TEMPERATURELAYERLISTMODEL_H
#define BLACKGUI_TEMPERATURELAYERLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/weather/temperaturelayer.h"
#include "blackmisc/weather/temperaturelayerlist.h"

class QObject;

namespace BlackGui::Models
{
    //! Temperature layer list model
    class BLACKGUI_EXPORT CTemperatureLayerListModel :
        public CListModelBase<BlackMisc::Weather::CTemperatureLayerList, false>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTemperatureLayerListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CTemperatureLayerListModel() {}
    };
}
#endif // guard
