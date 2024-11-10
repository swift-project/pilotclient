// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingsxswiftbuscomponent.h"
#include "ui_settingsxswiftbuscomponent.h"
#include "misc/dbusserver.h"
#include "misc/logmessage.h"
#include "misc/simulation/xplane/xswiftbusconfigwriter.h"

#include <QComboBox>

using namespace swift::misc;
using namespace swift::misc::simulation::settings;
using namespace swift::misc::simulation::xplane;

namespace BlackGui::Components
{
    const QStringList &CSettingsXSwiftBusComponent::getLogCategories()
    {
        static const QStringList cats { CLogCategories::guiComponent() };
        return cats;
    }

    CSettingsXSwiftBusComponent::CSettingsXSwiftBusComponent(QWidget *parent) : QFrame(parent),
                                                                                ui(new Ui::CSettingsXSwiftBusComponent)
    {
        ui->setupUi(this);

        connect(ui->pb_Save, &QPushButton::released, this, &CSettingsXSwiftBusComponent::saveServer);
        connect(ui->pb_Reset, &QPushButton::released, this, &CSettingsXSwiftBusComponent::resetServer);

        const QString dBusAddress = m_xSwiftBusSettings.get().getDBusServerAddressQt();
        ui->comp_DBusServer->setForXSwiftBus();
        ui->comp_DBusServer->set(dBusAddress);
    }

    CSettingsXSwiftBusComponent::~CSettingsXSwiftBusComponent()
    {}

    void CSettingsXSwiftBusComponent::resetServer()
    {
        const QString s = TXSwiftBusSettings::defaultValue().getDBusServerAddressQt();
        ui->comp_DBusServer->set(s);
    }

    void CSettingsXSwiftBusComponent::saveServer()
    {
        const QString dBusAddress = ui->comp_DBusServer->getDBusAddress();
        if (dBusAddress.isEmpty()) { return; }
        CXSwiftBusSettings s = m_xSwiftBusSettings.getThreadLocal();

        if (dBusAddress != s.getDBusServerAddressQt())
        {
            s.setDBusServerAddressQt(dBusAddress);
            const CStatusMessage msg = m_xSwiftBusSettings.setAndSave(dBusAddress);
            CXSwiftBusConfigWriter xswiftbusConfigWriter;
            xswiftbusConfigWriter.setDBusAddress(dBusAddress);
            xswiftbusConfigWriter.updateInAllXPlaneVersions();
            CLogMessage::preformatted(msg);
        }
    }
} // ns
