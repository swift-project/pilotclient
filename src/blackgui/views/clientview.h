// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_CLIENTVIEW_H
#define BLACKGUI_VIEWS_CLIENTVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/clientlistmodel.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/network/clientlist.h"

namespace BlackMisc::Network
{
    class CClient;
}
namespace BlackGui::Views
{
    //! Client view
    class BLACKGUI_EXPORT CClientView : public CViewBase<Models::CClientListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CClientView(QWidget *parent = nullptr);
    };
}
#endif // guard
