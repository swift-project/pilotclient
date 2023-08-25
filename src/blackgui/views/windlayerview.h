// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_WINDLAYERVIEW_H
#define BLACKGUI_WINDLAYERVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/windlayerlistmodel.h"
#include "blackgui/views/viewbase.h"

class QWidget;

namespace BlackMisc::Weather
{
    class CWindLayer;
}
namespace BlackGui::Views
{
    //! Wind layer view
    class BLACKGUI_EXPORT CWindLayerView : public CViewBase<Models::CWindLayerListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CWindLayerView(QWidget *parent = nullptr);
    };
}
#endif // guard
