/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simconnectsettingscomponent.h"
#include "ui_simconnectsettingscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/logmessage.h"
#include <QFileInfo>
#include <QDesktopServices>
#include <QMessageBox>
#include <QStringBuilder>
#include <QDir>

using namespace BlackMisc;
using namespace BlackMisc::Simulation::Fsx;
using namespace BlackMisc::Network;

namespace BlackSimPlugin
{
    namespace FsxCommon
    {
        CSimConnectSettingsComponent::CSimConnectSettingsComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSimConnectSettingsComponent)
        {
            ui->setupUi(this);

            connect(ui->pb_SettingsFsxOpenSimconnectCfg, &QPushButton::clicked, this, &CSimConnectSettingsComponent::openSimConnectCfgFile);
            connect(ui->pb_SettingsFsxDeleteSimconnectCfg, &QPushButton::clicked, this, &CSimConnectSettingsComponent::deleteSimConnectCfgFile);
            connect(ui->pb_SettingsFsxExistsSimconncetCfg, &QPushButton::clicked, this, &CSimConnectSettingsComponent::checkSimConnectCfgFile);
            connect(ui->pb_SettingsFsxSaveSimconnectCfg, &QPushButton::clicked, this, &CSimConnectSettingsComponent::saveSimConnectCfgFile);
            connect(ui->pb_SettingsFsxTestConnection, &QPushButton::clicked, this, &CSimConnectSettingsComponent::testSimConnectConnection);

            this->setSimConnectInfo();
        }

        CSimConnectSettingsComponent::~CSimConnectSettingsComponent()
        {
            // void
        }

        void CSimConnectSettingsComponent::openSimConnectCfgFile()
        {
            const QFileInfo info(CSimConnectUtilities::getLocalSimConnectCfgFilename());
            QDesktopServices::openUrl(QUrl::fromLocalFile(info.absoluteFilePath()));
        }

        void CSimConnectSettingsComponent::deleteSimConnectCfgFile()
        {
            const QString fileName = CSimConnectUtilities::getLocalSimConnectCfgFilename();
            const bool result = sGui->getIContextApplication()->removeFile(fileName);
            if (result)
            {
                QMessageBox::information(qApp->activeWindow(), tr("File deleted"),
                                         tr("File %1 deleted successfully.").arg(fileName));
            }
            checkSimConnectCfgFile();
        }

        void CSimConnectSettingsComponent::checkSimConnectCfgFile()
        {
            const QString fileName = CSimConnectUtilities::getLocalSimConnectCfgFilename();
            if (sGui->getIContextApplication()->existsFile(fileName))
            {
                ui->le_SettingsFsxExistsSimconncetCfg->setText(fileName);
            }
            else
            {
                ui->le_SettingsFsxExistsSimconncetCfg->setText("no file");
            }
        }

        void CSimConnectSettingsComponent::testSimConnectConnection()
        {
            const QString address = ui->le_SettingsFsxAddress->text().trimmed();
            const QString port = ui->le_SettingsFsxPort->text().trimmed();

            if (address.isEmpty() || port.isEmpty())
            {
                QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"),
                                     tr("Address and/or port not specified!"));
                return;
            }
            if (!CNetworkUtils::isValidIPv4Address(address))
            {
                QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"),
                                     tr("Wrong IPv4 address!"));
                return;
            }
            if (!CNetworkUtils::isValidPort(port))
            {
                QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"),
                                     tr("Invalid port!"));
                return;
            }
            int p = port.toInt();
            QString msg;
            if (!CNetworkUtils::canConnect(address, p, msg))
            {
                QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"), msg);
                return;
            }

            QMessageBox::information(qApp->activeWindow(), tr("Connection successful"),
                                     tr("Connected to %1:%2.").arg(address, port));
        }

        void CSimConnectSettingsComponent::saveSimConnectCfgFile()
        {
            QString address = ui->le_SettingsFsxAddress->text().trimmed();
            QString port = ui->le_SettingsFsxPort->text().trimmed();

            if (address.isEmpty() || port.isEmpty())
            {
                QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"),
                                     tr("Address and/or port not specified!"));
                return;
            }
            if (!CNetworkUtils::isValidIPv4Address(address))
            {
                QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"),
                                     tr("Wrong IPv4 address!"));
                return;
            }
            if (!CNetworkUtils::isValidPort(port))
            {
                QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"),
                                     tr("Invalid port!"));
                return;
            }

            const int p = port.toInt();
            QString fileName;

            if (sGui->getIContextSimulator())
            {
                const BlackMisc::Simulation::CSimulatorInternals internals(sGui->getIContextSimulator()->getSimulatorInternals());
                fileName = internals.getStringValue("fsx/simConnectCfgFilename");
            }

            if (fileName.isEmpty())
            {
                fileName = CSimConnectUtilities::getLocalSimConnectCfgFilename();
            }

            if (fileName.isEmpty())
            {
                QMessageBox::warning(qApp->activeWindow(), tr("Failed writing simConnect.cfg"),
                                     tr("No file name specified!"));
                return;
            }

            if (sGui->getIContextApplication()->writeToFile(fileName, CSimConnectUtilities::simConnectCfg(address, p)))
            {
                QMessageBox::information(qApp->activeWindow(), tr("File saved"),
                                         tr("File '%1' saved.").arg(fileName));
                checkSimConnectCfgFile();
            }
            else
            {
                QMessageBox::warning(qApp->activeWindow(), tr("Failed writing simConnect.cfg"),
                                     tr("Failed writing '%1'!").arg(fileName));
            }
        }

        void CSimConnectSettingsComponent::setSimConnectInfo()
        {
            const CWinDllUtils::DLLInfo simConnectInfo = CSimConnectUtilities::simConnectDllInfo();
            ui->lbl_SimConnectInfo->setText(simConnectInfo.summary());
        }
    } // ns
} // ns
