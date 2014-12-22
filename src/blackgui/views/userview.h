/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_USERVIEW_H
#define BLACKGUI_USERVIEW_H

#include "viewbase.h"
#include "../models/userlistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! User view
        class CUserView : public CViewBase<Models::CUserListModel, BlackMisc::Network::CUserList, BlackMisc::Network::CUser>
        {

        public:

            //! Constructor
            explicit CUserView(QWidget *parent = nullptr);

            //! Set station mode
            void setUserMode(Models::CUserListModel::UserMode userMode);
        };
    }
}
#endif // guard
