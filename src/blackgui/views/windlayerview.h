/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_WINDLAYERVIEW_H
#define BLACKGUI_WINDLAYERVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/windlayerlistmodel.h"
#include "blackgui/views/viewbase.h"

class QWidget;

namespace BlackMisc::Weather { class CWindLayer; }
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
