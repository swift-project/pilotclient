// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_LIVERYVIEW_H
#define BLACKGUI_VIEWS_LIVERYVIEW_H

#include "blackgui/views/viewdbobjects.h"
#include "blackgui/models/liverylistmodel.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui::Views
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
#endif // guard
