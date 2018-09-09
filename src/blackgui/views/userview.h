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

#include "blackgui/blackguiexport.h"
#include "blackgui/models/userlistmodel.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/network/userlist.h"

namespace BlackMisc { namespace Network { class CUser; } }
namespace BlackGui
{
    namespace Views
    {
        //! User view
        class BLACKGUI_EXPORT CUserView : public CViewBase<Models::CUserListModel, BlackMisc::Network::CUserList, BlackMisc::Network::CUser>
        {
        public:
            //! Constructor
            explicit CUserView(QWidget *parent = nullptr);

            //! Set user mode
            void setUserMode(Models::CUserListModel::UserMode userMode);
        };
    }
}

#endif // guard
