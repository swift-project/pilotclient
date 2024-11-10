// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_LIVERYLISTMODEL_H
#define BLACKGUI_LIVERYLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodeldbobjects.h"

class QObject;

namespace swift::misc::aviation
{
    class CLivery;
    class CLiveryList;
}

namespace BlackGui::Models
{
    //! Distributor list model
    class BLACKGUI_EXPORT CLiveryListModel :
        public CListModelDbObjects<swift::misc::aviation::CLiveryList, int, true>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CLiveryListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CLiveryListModel() override {}
    };
} // ns

#endif // guard
