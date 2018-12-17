/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_TEMPERATURELAYERVIEW_H
#define BLACKGUI_TEMPERATURELAYERVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/temperaturelayerlistmodel.h"
#include "blackgui/views/viewbase.h"

class QWidget;

namespace BlackMisc { namespace Weather { class CTemperatureLayer; } }

namespace BlackGui
{
    namespace Views
    {
        //! Airports view
        class BLACKGUI_EXPORT CTemperatureLayerView : public CViewBase<Models::CTemperatureLayerListModel, BlackMisc::Weather::CTemperatureLayerList, BlackMisc::Weather::CTemperatureLayer>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CTemperatureLayerView(QWidget *parent = nullptr);
        };
    }
}
#endif // guard
