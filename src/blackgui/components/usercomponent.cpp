/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "usercomponent.h"
#include "ui_usercomponent.h"
#include "../guiutility.h"
#include "blackmisc/network/userlist.h"
#include "blackcore/context_network.h"
#include "blackcore/network.h"

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
            CEnableForRuntime(nullptr, false),
            ui(new Ui::CUserComponent), m_updateTimer(nullptr)
        {
            ui->setupUi(this);
            this->tabBar()->setExpanding(false);
            this->m_updateTimer = new CUpdateTimer(&CUserComponent::update, this);

            connect(this->ui->tvp_AllUsers, &CUserView::rowCountChanged, this, &CUserComponent::ps_onCountChanged);
            connect(this->ui->tvp_Clients, &CClientView::rowCountChanged, this, &CUserComponent::ps_onCountChanged);
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
            Q_ASSERT(this->ui->tvp_AllUsers);
            Q_ASSERT(this->ui->tvp_Clients);
            Q_ASSERT(this->getIContextNetwork());

            if (this->getIContextNetwork()->isConnected())
            {
                bool withData = countUsers() > 0 || countClients() > 0;
                if (withData && !isVisibleWidget())
                {
                    // Skip update, invisible
                    return;
                }

                // load data
                this->ui->tvp_Clients->updateContainer(this->getIContextNetwork()->getOtherClients());
                this->ui->tvp_AllUsers->updateContainer(this->getIContextNetwork()->getUsers());
            }
        }

        void CUserComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(this->getIContextNetwork());
            this->connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CUserComponent::ps_connectionStatusChanged);
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

        void CUserComponent::ps_connectionStatusChanged(uint from, uint to)
        {
            INetwork::ConnectionStatus fromStatus = static_cast<INetwork::ConnectionStatus>(from);
            INetwork::ConnectionStatus toStatus = static_cast<INetwork::ConnectionStatus>(to);
            Q_UNUSED(fromStatus);
            if (INetwork::isDisconnectedStatus(toStatus))
            {
                this->ui->tvp_AllUsers->clear();
                this->ui->tvp_Clients->clear();
            }
        }

    } // namespace
} // namespace
