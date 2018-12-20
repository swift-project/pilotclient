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

#include "blackgui/blackguiexport.h"
#include "blackgui/models/liverylistmodel.h"
#include "blackgui/views/viewdbobjects.h"

class QWidget;

namespace BlackMisc
{
    namespace Aviation
    {
        class CLivery;
        class CLiveryList;
    }
}

namespace BlackGui
{
    namespace Views
    {
        //! Distributors
        class BLACKGUI_EXPORT CLiveryView :
            public CViewWithDbObjects<Models::CLiveryListModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CLiveryView(QWidget *parent = nullptr);
        };
    } // ns
} // ns
#endif // guard
