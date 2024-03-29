// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackconfig/buildconfig.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/data/globalsetup.h"
#include "blackgui/editors/serverform.h"
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

namespace BlackGui::Components
{
    CSettingsNetworkServersComponent::CSettingsNetworkServersComponent(QWidget *parent) : QFrame(parent),
                                                                                          ui(new Ui::CSettingsNetworkServersComponent)
    {
        ui->setupUi(this);

        // Settings server
        connect(ui->pb_RemoveServer, &QPushButton::pressed, this, &CSettingsNetworkServersComponent::alterTrafficServer);
        connect(ui->pb_SaveServer, &QPushButton::pressed, this, &CSettingsNetworkServersComponent::alterTrafficServer);
        connect(ui->tvp_Servers, &QTableView::clicked, this, &CSettingsNetworkServersComponent::serverSelected);
        this->reloadSettings();
    }

    CSettingsNetworkServersComponent::~CSettingsNetworkServersComponent()
    {}

    void CSettingsNetworkServersComponent::reloadSettings()
    {
        CServerList serverList(m_trafficNetworkServers.get());

        // add swift test servers in case we have no servers:
        // this is debug/bootstrap feature we can continue to test when something goes wrong
        if (serverList.isEmpty() && CBuildConfig::isLocalDeveloperDebugBuild())
        {
            serverList.push_back(sGui->getGlobalSetup().getPredefinedServersPlusHardcodedServers());
        }
        ui->tvp_Servers->updateContainer(serverList);
    }

    void CSettingsNetworkServersComponent::serverSelected(const QModelIndex &index)
    {
        const CServer clickedServer = ui->tvp_Servers->at(index);
        ui->form_Server->setServer(clickedServer);
    }

    void CSettingsNetworkServersComponent::alterTrafficServer()
    {
        const QObject *sender = QObject::sender();
        const CServer server(ui->form_Server->getServer());

        CServerList serverList(m_trafficNetworkServers.getThreadLocal());
        CStatusMessage msg;
        if (sender == ui->pb_RemoveServer)
        {
            // lenient name removal
            serverList.removeByName(server.getName());
        }
        else if (sender == ui->pb_SaveServer)
        {
            CStatusMessageList msgs = server.validate();
            if (!msgs.isEmpty()) { msgs.addCategories(this); }

            // error?
            if (msgs.isFailure())
            {
                CLogMessage::preformatted(msgs);
                return;
            }

            serverList.replaceOrAdd(&CServer::getName, server.getName(), server);
        }
        else
        {
            qFatal("Wrong sender");
            return;
        }

        msg = m_trafficNetworkServers.setAndSave(serverList);
        this->reloadSettings(); // call manually as local object

        if (!msg.isEmpty())
        {
            msg.addCategories(this);
            CLogMessage::preformatted(msg);
        }
    }
} // namespace
