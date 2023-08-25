// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_USERLISTMODEL_H
#define BLACKGUI_MODELS_USERLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelbase.h"
#include "blackmisc/network/userlist.h"

namespace BlackGui::Models
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
#endif // guard
