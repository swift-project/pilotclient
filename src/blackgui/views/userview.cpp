// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/views/userview.h"
#include "blackgui/models/userlistmodel.h"

#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackGui::Models;
using namespace BlackGui::Menus;

namespace BlackGui::Views
{
    CUserView::CUserView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CUserListModel(CUserListModel::UserDetailed, this));
    }

    void CUserView::setUserMode(CUserListModel::UserMode userMode)
    {
        Q_ASSERT(m_model);
        m_model->setUserMode(userMode);
    }

    void CUserView::customMenu(CMenuActions &menuActions)
    {
        if (this->hasSelection())
        {
            const CUser user(this->selectedObject());
            if (user.hasCallsign())
            {
                menuActions.addMenuCom();
                menuActions.addAction(CIcons::appTextMessages16(), "Show text messages", CMenuAction::pathClientCom(), { this, &CUserView::requestTextMessage });
            }
        }
    }

    void CUserView::requestTextMessage()
    {
        const CUser user(this->selectedObject());
        if (!user.hasCallsign()) { return; }
        emit this->requestTextMessageWidget(user.getCallsign());
    }
} // ns
