/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/contextnetwork.h"
#include "blackcore/network.h"
#include "blackgui/components/usercomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/views/clientview.h"
#include "blackgui/views/userview.h"
#include "ui_usercomponent.h"

#include <QString>
#include <QTabBar>

using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CUserComponent::CUserComponent(QWidget *parent) :
            QTabWidget(parent),
            CEnableForDockWidgetInfoArea(),
            ui(new Ui::CUserComponent),
            m_updateTimer(new CUpdateTimer("CUserComponent", &CUserComponent::update, this))
        {
            ui->setupUi(this);
            this->tabBar()->setExpanding(false);
            connect(this->ui->tvp_AllUsers, &CUserView::modelDataChanged, this, &CUserComponent::ps_onCountChanged);
            connect(this->ui->tvp_Clients, &CClientView::modelDataChanged, this, &CUserComponent::ps_onCountChanged);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CUserComponent::ps_connectionStatusChanged);
        }

        CUserComponent::~CUserComponent()
        { }

        int CUserComponent::countClients() const
        {
            Q_ASSERT(this->ui->tvp_Clients);
            return this->ui->tvp_Clients->rowCount();
        }

        int CUserComponent::countUsers() const
        {
            Q_ASSERT(this->ui->tvp_AllUsers);
            return this->ui->tvp_AllUsers->rowCount();
        }

        void CUserComponent::update()
        {
            if (!sGui || !sGui->getIContextNetwork()) { return; }
            Q_ASSERT(this->ui->tvp_AllUsers);
            Q_ASSERT(this->ui->tvp_Clients);

            if (sGui->getIContextNetwork()->isConnected())
            {
                bool withData = countUsers() > 0 || countClients() > 0;
                if (withData && !isVisibleWidget())
                {
                    // Skip update, invisible
                    return;
                }

                // load data
                this->ui->tvp_Clients->updateContainer(sGui->getIContextNetwork()->getOtherClients());
                this->ui->tvp_AllUsers->updateContainer(sGui->getIContextNetwork()->getUsers());
            }
        }

        void CUserComponent::ps_onCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int iu = this->indexOf(this->ui->tb_AllUsers);
            int ic = this->indexOf(this->ui->tb_Clients);
            QString u = this->tabBar()->tabText(iu);
            QString c = this->tabBar()->tabText(ic);
            u = CGuiUtility::replaceTabCountValue(u, this->countUsers());
            c = CGuiUtility::replaceTabCountValue(c, this->countClients());
            this->tabBar()->setTabText(iu, u);
            this->tabBar()->setTabText(ic, c);
        }

        void CUserComponent::ps_connectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (INetwork::isDisconnectedStatus(to))
            {
                this->ui->tvp_AllUsers->clear();
                this->ui->tvp_Clients->clear();
            }
        }
    } // namespace
} // namespace
