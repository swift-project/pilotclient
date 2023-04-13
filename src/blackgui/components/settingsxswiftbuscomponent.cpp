/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "settingsxswiftbuscomponent.h"
#include "ui_settingsxswiftbuscomponent.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/xplane/xswiftbusconfigwriter.h"

#include <QComboBox>

using namespace BlackMisc;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Simulation::XPlane;

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
