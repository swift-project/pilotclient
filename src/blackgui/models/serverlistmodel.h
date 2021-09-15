/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
