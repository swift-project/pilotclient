/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_USERVIEW_H
#define BLACKGUI_USERVIEW_H

#include "blackgui/views/viewbase.h"
#include "blackgui/models/userlistmodel.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/network/userlist.h"

namespace BlackMisc::Network
{
    class CUser;
}
namespace BlackGui::Views
{
    //! User view
    class BLACKGUI_EXPORT CUserView : public CViewBase<Models::CUserListModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CUserView(QWidget *parent = nullptr);

        //! Set user mode
        void setUserMode(Models::CUserListModel::UserMode userMode);

    signals:
        //! Request a text message
        void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

    protected:
        //! \copydoc CViewBase::customMenu
        virtual void customMenu(Menus::CMenuActions &menuActions) override;

    private:
        //! Request text message for selected aircraft
        void requestTextMessage();
    };
} // ns

#endif // guard
