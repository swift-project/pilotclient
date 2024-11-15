// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_USERLISTMODEL_H
#define SWIFT_GUI_MODELS_USERLISTMODEL_H

#include "gui/swiftguiexport.h"
#include "gui/models/listmodelbase.h"
#include "misc/network/userlist.h"

namespace swift::gui::models
{
    /*!
     * User list model
     */
    class SWIFT_GUI_EXPORT CUserListModel :
        public CListModelBase<swift::misc::network::CUserList, true>
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