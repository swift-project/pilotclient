// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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
