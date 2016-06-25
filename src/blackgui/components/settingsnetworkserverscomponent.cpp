/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackcore/contextapplication.h"
#include "blackcore/data/globalsetup.h"
#include "blackgui/components/serverform.h"
#include "blackgui/components/settingsnetworkserverscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/serverview.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "ui_settingsnetworkserverscomponent.h"

#include <QPushButton>
#include <QString>
#include <QTableView>

using namespace BlackConfig;
using namespace BlackCore;
using namespace BlackCore::Data;
using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;

namespace BlackGui
{
    namespace Components
    {
        CSettingsNetworkServersComponent::CSettingsNetworkServersComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsNetworkServersComponent)
        {
            ui->setupUi(this);

            // Settings server
            this->connect(this->ui->pb_RemoveServer, &QPushButton::pressed, this, &CSettingsNetworkServersComponent::ps_alterTrafficServer);
            this->connect(this->ui->pb_SaveServer, &QPushButton::pressed, this, &CSettingsNetworkServersComponent::ps_alterTrafficServer);
            this->connect(this->ui->pb_AddServer, &QPushButton::pressed, this, &CSettingsNetworkServersComponent::ps_alterTrafficServer);
            this->connect(this->ui->tvp_Servers, &QTableView::clicked, this, &CSettingsNetworkServersComponent::ps_serverSelected);
            this->ps_reloadSettings();
        }

        CSettingsNetworkServersComponent::~CSettingsNetworkServersComponent()
        { }

        void CSettingsNetworkServersComponent::ps_reloadSettings()
        {
            CServerList serverList(m_trafficNetworkServers.get());

            // add swift test servers in case we have no servers:
            // this is debug/bootstrap feature we can continue to test when something goes wrong
            if (serverList.isEmpty() && (CBuildConfig::isBetaTest() || sGui->isRunningInDeveloperEnvironment()))
            {
                serverList.push_back(sGui->getGlobalSetup().getFsdTestServersPlusHardcodedServers());
            }
            this->ui->tvp_Servers->updateContainer(serverList);
        }

        void CSettingsNetworkServersComponent::ps_serverSelected(const QModelIndex &index)
        {
            const CServer clickedServer = this->ui->tvp_Servers->at(index);
            this->ui->frp_ServerForm->setServer(clickedServer);
        }

        void CSettingsNetworkServersComponent::ps_alterTrafficServer()
        {
            CServer server(this->ui->frp_ServerForm->getServer());
            CStatusMessageList  msgs = server.validate();
            if (!msgs.isEmpty()) { msgs.addCategories(this); }

            CServerList serverList(m_trafficNetworkServers.getThreadLocal());
            QObject *sender = QObject::sender();
            CStatusMessage msg;
            bool changed = false;
            bool save = false;
            if (sender == this->ui->pb_RemoveServer)
            {
                // lenient name removal
                serverList.removeByName(server.getName());
                changed = true;
            }
            else if (sender == this->ui->pb_AddServer)
            {
                if (!msgs.isEmpty())
                {
                    CLogMessage::preformatted(msgs);
                    return;
                }
                serverList.replaceOrAdd(&CServer::getName, server.getName(), server);
                changed = true;
            }
            else if (sender == this->ui->pb_SaveServer)
            {
                save = true;
                if (msgs.isEmpty() && server.hasAddressAndPort())
                {
                    // update in any case to list before saving if we have a valid form
                    serverList.replaceOrAdd(&CServer::getName, server.getName(), server);
                    changed = true;
                }
            }

            if (changed)
            {
                msg = save ? m_trafficNetworkServers.setAndSave(serverList) : m_trafficNetworkServers.set(serverList);
                this->ps_reloadSettings(); // call manually as local object
            }

            if (!msg.isEmpty())
            {
                msg.addCategories(this);
                CLogMessage::preformatted(msg);
            }
        }
    } // namespace
} // namespace
