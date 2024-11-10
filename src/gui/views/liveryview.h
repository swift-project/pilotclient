// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_LIVERYVIEW_H
#define SWIFT_GUI_VIEWS_LIVERYVIEW_H

#include "gui/views/viewdbobjects.h"
#include "gui/models/liverylistmodel.h"
#include "gui/swiftguiexport.h"

namespace swift::gui::views
{
    //! Distributors
    class SWIFT_GUI_EXPORT CLiveryView :
        public CViewWithDbObjects<models::CLiveryListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLiveryView(QWidget *parent = nullptr);
    };
} // ns
#endif // guard
