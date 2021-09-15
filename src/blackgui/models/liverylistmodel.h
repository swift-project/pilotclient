/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LIVERYLISTMODEL_H
#define BLACKGUI_LIVERYLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodeldbobjects.h"

class QObject;

namespace BlackMisc::Aviation
{
    class CLivery;
    class CLiveryList;
}

namespace BlackGui::Models
{
    //! Distributor list model
    class BLACKGUI_EXPORT CLiveryListModel :
        public CListModelDbObjects<BlackMisc::Aviation::CLiveryList, int, true>
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
