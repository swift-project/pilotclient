/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LIVERYVIEW_H
#define BLACKGUI_LIVERYVIEW_H

#include "viewbase.h"
#include "blackgui/blackguiexport.h"
#include "blackgui/models/liverylistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! Distributors
        class BLACKGUI_EXPORT CLiveryView : public CViewBase<Models::CLiveryListModel, BlackMisc::Aviation::CLiveryList, BlackMisc::Aviation::CLivery>
        {

        public:
            //! Constructor
            explicit CLiveryView(QWidget *parent = nullptr);
        };
    }
}
#endif // guard
