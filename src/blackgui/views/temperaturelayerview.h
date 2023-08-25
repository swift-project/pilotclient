// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_TEMPERATURELAYERVIEW_H
#define BLACKGUI_VIEWS_TEMPERATURELAYERVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/temperaturelayerlistmodel.h"
#include "blackgui/views/viewbase.h"

namespace BlackMisc::Weather
{
    class CTemperatureLayer;
}
namespace BlackGui::Views
{
    //! Airports view
    class BLACKGUI_EXPORT CTemperatureLayerView : public CViewBase<Models::CTemperatureLayerListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CTemperatureLayerView(QWidget *parent = nullptr);
    };
}
#endif // guard
