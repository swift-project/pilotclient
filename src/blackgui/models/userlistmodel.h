/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_USERLISTMODEL_H
#define BLACKGUI_USERLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/network/userlist.h"

namespace BlackGui
{
    namespace Models
    {
        /*!
         * User list model
         */
        class BLACKGUI_EXPORT CUserListModel :
            public CListModelBase<BlackMisc::Network::CUserList, true>
        {
            Q_OBJECT

        public:
            //! What level of detail
            enum UserMode
            {
                NotSet,
                UserDetailed,
                UserShort
            };

            //! Constructor
            explicit CUserListModel(UserMode userMode, QObject *parent = nullptr);

            //! Destructor
            virtual ~CUserListModel() {}

            //! Set station mode
            void setUserMode(UserMode userMode);

        private:
            UserMode m_userMode = NotSet;
        };
    }
}
#endif // guard
