/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorxplaneconfigwindow.h"
#include "blackcore/application.h"
#include "ui_simulatorxplaneconfigwindow.h"
#include "blackmisc/simulation/xplane/xswiftbusconfigwriter.h"

#include <QComboBox>
#include <QDialogButtonBox>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Simulation::XPlane;

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CSimulatorXPlaneConfigWindow::CSimulatorXPlaneConfigWindow(QWidget *parent) :
            CPluginConfigWindow(parent),
            ui(new Ui::CSimulatorXPlaneConfigWindow)
        {
            ui->setupUi(this);
            ui->comp_SettingsXSwiftBus->setDefaultP2PAddress(m_xswiftbusServerSetting.getDefault());
            ui->comp_SettingsXSwiftBus->set(m_xswiftbusServerSetting.getThreadLocal());

            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::onSettingsAccepted);
            connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorXPlaneConfigWindow::close);
        }

        CSimulatorXPlaneConfigWindow::~CSimulatorXPlaneConfigWindow()
        { }

        void CSimulatorXPlaneConfigWindow::onSettingsAccepted()
        {
            const QString currentAddress = m_xswiftbusServerSetting.getThreadLocal();
            const QString updatedAddress = ui->comp_SettingsXSwiftBus->getDBusAddress();
            if (currentAddress != ui->comp_SettingsXSwiftBus->getDBusAddress())
            {
                m_xswiftbusServerSetting.set(updatedAddress);
                CXSwiftBusConfigWriter xswiftbusConfigWriter;
                xswiftbusConfigWriter.setDBusAddress(updatedAddress);
                xswiftbusConfigWriter.updateInAllXPlaneVersions();
            }
            close();
        }
    } // ns
} // ns
