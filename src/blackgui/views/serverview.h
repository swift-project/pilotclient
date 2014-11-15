/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SERVERVIEW_H
#define BLACKGUI_SERVERVIEW_H

#include "viewbase.h"
#include "../models/serverlistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! Network servers
        class CServerView : public CViewBase<Models::CServerListModel, BlackMisc::Network::CServerList>
        {

        public:

            //! Constructor
            explicit CServerView(QWidget *parent = nullptr);
        };
    }
}
#endif // guard
