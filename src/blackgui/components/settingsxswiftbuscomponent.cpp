/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsxswiftbuscomponent.h"
#include "ui_settingsxswiftbuscomponent.h"
#include "blackmisc/dbusserver.h"

#include <QComboBox>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CSettingsXSwiftBusComponent::CSettingsXSwiftBusComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsXSwiftBusComponent)
        {
            ui->setupUi(this);
            ui->cb_XSwiftBusServer->addItem(CDBusServer::sessionBusAddress());
            ui->cb_XSwiftBusServer->addItem(CDBusServer::systemBusAddress());
            ui->cb_XSwiftBusServer->setCurrentText(m_xSwiftBusServerSetting.getThreadLocal());

            connect(ui->cb_XSwiftBusServer, &QComboBox::currentTextChanged, this, &CSettingsXSwiftBusComponent::saveServer);
        }

        CSettingsXSwiftBusComponent::~CSettingsXSwiftBusComponent()
        { }

        void CSettingsXSwiftBusComponent::saveServer(const QString &dBusAddress)
        {
            if (dBusAddress != m_xSwiftBusServerSetting.getThreadLocal())
            {
                m_xSwiftBusServerSetting.setAndSave(dBusAddress);
            }
        }
    } // ns
} // ns
