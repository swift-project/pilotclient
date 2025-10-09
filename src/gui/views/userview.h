// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_USERVIEW_H
#define SWIFT_GUI_USERVIEW_H

#include "gui/models/userlistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"

namespace swift::misc::network
{
    class CUser;
}
namespace swift::gui::views
{
    //! User view
    class SWIFT_GUI_EXPORT CUserView : public CViewBase<models::CUserListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CUserView(QWidget *parent = nullptr);

        //! Set user mode
        void setUserMode(models::CUserListModel::UserMode userMode);

    signals:
        //! Request a text message
        void requestTextMessageWidget(const swift::misc::aviation::CCallsign &callsign);

    protected:
        //! \copydoc CViewBase::customMenu
        void customMenu(menus::CMenuActions &menuActions) override;

    private:
        //! Request text message for selected aircraft
        void requestTextMessage();
    };
} // namespace swift::gui::views

#endif // SWIFT_GUI_USERVIEW_H
