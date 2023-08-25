// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_SERVERLISTMODEL_H
#define BLACKGUI_MODELS_SERVERLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/network/serverlist.h"

class QObject;

namespace BlackGui::Models
{
    //! Server list model
    class BLACKGUI_EXPORT CServerListModel : public CListModelBase<BlackMisc::Network::CServerList, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CServerListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CServerListModel() {}
    };
} // ns
#endif // guard
