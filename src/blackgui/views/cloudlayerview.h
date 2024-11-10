// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_CLOUDLAYERVIEW_H
#define BLACKGUI_CLOUDLAYERVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/cloudlayerlistmodel.h"
#include "blackgui/views/viewbase.h"

class QWidget;

namespace swift::misc::weather
{
    class CCloudLayer;
}

namespace BlackGui::Views
{
    //! Airports view
    class BLACKGUI_EXPORT CCloudLayerView : public CViewBase<Models::CCloudLayerListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCloudLayerView(QWidget *parent = nullptr);
    };
}
#endif // guard
