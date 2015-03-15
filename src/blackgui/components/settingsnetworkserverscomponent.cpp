/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsnetworkserverscomponent.h"
#include "ui_settingsnetworkserverscomponent.h"
#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/settingsblackmiscclasses.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Settings;

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
            this->connect(this->ui->pb_SettingsTnServersRemoveServer, &QPushButton::pressed, this, &CSettingsNetworkServersComponent::ps_alterTrafficServer);
            this->connect(this->ui->pb_SettingsTnServersSaveServer, &QPushButton::pressed, this, &CSettingsNetworkServersComponent::ps_alterTrafficServer);
            this->connect(this->ui->tvp_SettingsTnServers, &QTableView::clicked, this, &CSettingsNetworkServersComponent::ps_networkServerSelected);
        }

        CSettingsNetworkServersComponent::~CSettingsNetworkServersComponent()
        { }

        void CSettingsNetworkServersComponent::runtimeHasBeenSet()
        {
            Q_ASSERT_X(this->getIContextSettings(), "runtimeHasBeenSet", "Missing settings");
            this->connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &CSettingsNetworkServersComponent::ps_changedSettings);
        }

        void CSettingsNetworkServersComponent::reloadSettings()
        {
            // local copy
            CSettingsNetwork nws = this->getIContextSettings()->getNetworkSettings();

            // update servers
            this->ui->tvp_SettingsTnServers->updateContainer(nws.getTrafficNetworkServers());
        }

        void CSettingsNetworkServersComponent::ps_networkServerSelected(QModelIndex index)
        {
            const CServer clickedServer = this->ui->tvp_SettingsTnServers->at(index);
            this->ui->frp_ServerForm->setServer(clickedServer);
        }

        void CSettingsNetworkServersComponent::ps_alterTrafficServer()
        {
            CServer server = this->ui->frp_ServerForm->getServer();
            CStatusMessageList  msgs = server.validate();
            if (!msgs.isEmpty())
            {
                msgs.addCategories(this);
                msgs.addCategory(CLogCategory::validation());
                CLogMessage::preformatted(msgs);
                return;
            }

            const QString path = CSettingUtilities::appendPaths(IContextSettings::PathNetworkSettings(), CSettingsNetwork::ValueTrafficServers());
            QObject *sender = QObject::sender();
            if (sender == this->ui->pb_SettingsTnServersRemoveServer)
            {
                this->getIContextSettings()->value(path, CSettingUtilities::CmdRemove(), server.toCVariant());
            }
            else if (sender == this->ui->pb_SettingsTnServersSaveServer)
            {
                this->getIContextSettings()->value(path, CSettingUtilities::CmdUpdate(), server.toCVariant());
            }
        }

        void CSettingsNetworkServersComponent::ps_changedSettings(uint typeValue)
        {
            IContextSettings::SettingsType type = static_cast<IContextSettings::SettingsType>(typeValue);
            Q_UNUSED(type);
        }


    } // namespace
} // namespace
