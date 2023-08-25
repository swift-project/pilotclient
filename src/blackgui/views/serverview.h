// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_SERVERVIEW_H
#define BLACKGUI_VIEWS_SERVERVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/views/viewbase.h"
#include "blackgui/models/serverlistmodel.h"

class QWidget;

namespace BlackGui::Views
{
    //! Network servers
    class BLACKGUI_EXPORT CServerView : public CViewBase<Models::CServerListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CServerView(QWidget *parent = nullptr);
    };
} // ns
#endif // guard
