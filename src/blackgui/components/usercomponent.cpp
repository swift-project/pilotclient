/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextnetwork.h"
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
using namespace BlackGui::Settings;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CUserComponent::CUserComponent(QWidget *parent) :
            QTabWidget(parent),
            CEnableForDockWidgetInfoArea(),
            ui(new Ui::CUserComponent)
        {
            ui->setupUi(this);
            this->setCurrentIndex(0);
            this->tabBar()->setExpanding(false);
            this->tabBar()->setUsesScrollButtons(true);
            connect(ui->tvp_AllUsers, &CUserView::modelDataChangedDigest,   this, &CUserComponent::onCountChanged);
            connect(ui->tvp_AllUsers, &CUserView::requestTextMessageWidget, this, &CUserComponent::requestTextMessageWidget);
            connect(ui->tvp_Clients,  &CClientView::modelDataChangedDigest, this, &CUserComponent::onCountChanged);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CUserComponent::onConnectionStatusChanged);
            connect(&m_updateTimer, &QTimer::timeout, this, &CUserComponent::update);
            this->onSettingsChanged();
        }

        CUserComponent::~CUserComponent()
        { }

        int CUserComponent::countClients() const
        {
            Q_ASSERT(ui->tvp_Clients);
            return ui->tvp_Clients->rowCount();
        }

        int CUserComponent::countUsers() const
        {
            Q_ASSERT(ui->tvp_AllUsers);
            return ui->tvp_AllUsers->rowCount();
        }

        void CUserComponent::update()
        {
            if (!sGui || !sGui->getIContextNetwork()) { return; }
            Q_ASSERT(ui->tvp_AllUsers);
            Q_ASSERT(ui->tvp_Clients);

            if (sGui->getIContextNetwork()->isConnected())
            {
                bool withData = countUsers() > 0 || countClients() > 0;
                if (withData && !isVisibleWidget())
                {
                    // Skip update, invisible
                    return;
                }

                // load data
                ui->tvp_Clients->updateContainer(sGui->getIContextNetwork()->getClients());
                ui->tvp_AllUsers->updateContainer(sGui->getIContextNetwork()->getUsers());
            }
        }

        void CUserComponent::onCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int iu = this->indexOf(ui->tb_AllUsers);
            int ic = this->indexOf(ui->tb_Clients);
            QString u = this->tabBar()->tabText(iu);
            QString c = this->tabBar()->tabText(ic);
            u = CGuiUtility::replaceTabCountValue(u, this->countUsers());
            c = CGuiUtility::replaceTabCountValue(c, this->countClients());
            this->tabBar()->setTabText(iu, u);
            this->tabBar()->setTabText(ic, c);
        }

        void CUserComponent::onConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (INetwork::isDisconnectedStatus(to))
            {
                ui->tvp_AllUsers->clear();
                ui->tvp_Clients->clear();
                m_updateTimer.stop();
            }
            else if (INetwork::isConnectedStatus(to))
            {
                m_updateTimer.start();
            }
        }

        void CUserComponent::onSettingsChanged()
        {
            const CViewUpdateSettings settings = m_settings.get();
            const int ms = settings.getAtcUpdateTime().toMs();
            m_updateTimer.setInterval(ms);
        }
    } // namespace
} // namespace
