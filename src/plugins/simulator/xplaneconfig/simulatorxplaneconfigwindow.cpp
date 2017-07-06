/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorxplaneconfigwindow.h"
#include "blackcore/application.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/simulation/xplane/xplaneutil.h"
#include "ui_simulatorxplaneconfigwindow.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFlags>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QStringBuilder>

class QWidget;

using namespace BlackGui;
using namespace BlackMisc;

namespace
{
    QString xSwiftBusOriginDir()
    {
        return CDirectoryUtils::getBinDir() % QStringLiteral("/../xswiftbus");
    }
}

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CSimulatorXPlaneConfigWindow::CSimulatorXPlaneConfigWindow(QWidget *parent) :
            CPluginConfigWindow(parent),
            ui(new Ui::CSimulatorXPlaneConfigWindow)
        {
            ui->setupUi(this);

            ui->cp_XSwiftBusServer->addItem(BlackMisc::CDBusServer::sessionBusAddress());
            ui->cp_XSwiftBusServer->addItem(BlackMisc::CDBusServer::systemBusAddress());

            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::ps_storeSettings);
            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::close);
            connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorXPlaneConfigWindow::close);

            ui->cp_XSwiftBusServer->setCurrentText(m_xswiftbusServerSetting.getThreadLocal());

            if (xSwiftBusAvailable())
                connect(ui->pb_InstallXSwiftBus, &QPushButton::clicked, this, &CSimulatorXPlaneConfigWindow::ps_installXSwiftBus);
            else
                ui->pb_InstallXSwiftBus->setEnabled(false);
        }

        CSimulatorXPlaneConfigWindow::~CSimulatorXPlaneConfigWindow()
        {

        }

        bool CSimulatorXPlaneConfigWindow::xSwiftBusAvailable()
        {
            return QDir(xSwiftBusOriginDir()).exists();
        }

        void CSimulatorXPlaneConfigWindow::ps_storeSettings()
        {
            if (ui->cp_XSwiftBusServer->currentText() != m_xswiftbusServerSetting.getThreadLocal())
            {
                m_xswiftbusServerSetting.set(ui->cp_XSwiftBusServer->currentText());
            }
        }

        void CSimulatorXPlaneConfigWindow::ps_installXSwiftBus()
        {
            const QString xPlaneLocation = BlackMisc::Simulation::XPlane::CXPlaneUtil::xplaneRootDir();
            QString path = QFileDialog::getExistingDirectory(parentWidget(),
                           tr("Choose your X-Plane install directory"),
                           xPlaneLocation,
                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks | QFileDialog::DontUseNativeDialog);

            if (path.isEmpty()) // canceled
                return;

            path.append("/Resources/plugins");
            if (!QDir(path).exists())
            {
                QMessageBox::warning(this, tr("Invalid X-Plane directory"), tr("%1 is not a valid X-Plane installation.").arg(path));
                return;
            }

            path.append("/xswiftbus");

            // TODO Use QtConcurrent here, maybe?
            const bool result = BlackMisc::CFileUtils::copyRecursively(xSwiftBusOriginDir(), path);
            if (result)
            {
                QMessageBox::information(this, tr("XSwiftBus installed"), tr("You may now launch your X-Plane and start using XSwiftBus!"));
            }
            else
            {
                QMessageBox::warning(this, tr("Failed installing XSwiftBus"), tr("Failed installing the XSwiftBus plugin in your X-Plane installation directory; try installing it manually."));
            }
        }
    }
}
