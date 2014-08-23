/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsfsxcomponent.h"
#include "ui_settingsfsxcomponent.h"
#include <QDesktopServices>
#include <QFile>
#include <QDir>
#include <QFileInfo>

#include "blackmisc/networkutils.h"
#include "blackmisc/statusmessage.h"
#include "blacksim/fsx/fsxsimulatorsetup.h"
#include "blacksim/fsx/simconnectutilities.h"

using namespace BlackMisc;
using namespace BlackSim::Fsx;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CSettingsFsxComponent::CSettingsFsxComponent(QWidget *parent) :
            QFrame(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CSettingsFsxComponent)
        {
            ui->setupUi(this);

            this->connect(this->ui->pb_SettingsFsxTestConnection, &QPushButton::clicked, this, &CSettingsFsxComponent::testSimConnectConnection);
            this->connect(this->ui->pb_SettingsFsxSaveSimconnectCfg, &QPushButton::clicked, this, &CSettingsFsxComponent::saveSimConnectCfg);
            this->connect(this->ui->pb_SettingsFsxOpenSimconnectCfg, &QPushButton::clicked, this, &CSettingsFsxComponent::simConnectCfgFile);
            this->connect(this->ui->pb_SettingsFsxDeleteSimconnectCfg, &QPushButton::clicked, this, &CSettingsFsxComponent::simConnectCfgFile);
            this->connect(this->ui->pb_SettingsFsxExistsSimconncetCfg, &QPushButton::clicked, this, &CSettingsFsxComponent::simConnectCfgFile);
        }

        CSettingsFsxComponent::~CSettingsFsxComponent()
        {
            delete ui;
        }

        /*
         * SimConnect working?
         */
        void CSettingsFsxComponent::testSimConnectConnection()
        {
            QString address = this->ui->le_SettingsFsxAddress->text().trimmed();
            QString port = this->ui->le_SettingsFsxPort->text().trimmed();

            if (address.isEmpty() || port.isEmpty())
            {
                this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "no address or port"));
                return;
            }
            if (!CNetworkUtils::isValidIPv4Address(address))
            {
                this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "IPv4 address invalid"));
                return;
            }
            if (!CNetworkUtils::isValidPort(port))
            {
                this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "invalid port"));
                return;
            }
            quint16 p = port.toUInt();
            QString msg;
            if (!CNetworkUtils::canConnect(address, p, msg))
            {
                this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, msg));
                return;
            }

            msg = QString("Connected to %1:%2").arg(address).arg(port);
            this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, msg));
        }

        /*
         * Save simconnect.cfg
         */
        void CSettingsFsxComponent::saveSimConnectCfg()
        {
            if (!this->getIContextSimulator() || !this->getIContextSimulator()->isSimulatorAvailable())
            {
                this->sendStatusMessage(CStatusMessage::getErrorMessage("Simulator not available", CStatusMessage::TypeSimulator));
                return;
            }
            QString address = this->ui->le_SettingsFsxAddress->text().trimmed();
            QString port = this->ui->le_SettingsFsxPort->text().trimmed();

            if (address.isEmpty() || port.isEmpty())
            {
                this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "no address or port"));
                return;
            }
            if (!CNetworkUtils::isValidIPv4Address(address))
            {
                this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "IPv4 address invalid"));
                return;
            }
            if (!CNetworkUtils::isValidPort(port))
            {
                this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "invalid port"));
                return;
            }
            quint16 p = port.toUInt();
            QString fileName = this->getIContextSimulator()->getSimulatorInfo().getSimulatorSetupValueAsString(CFsxSimulatorSetup::SetupSimConnectCfgFile);
            Q_ASSERT(!fileName.isEmpty());
            // write either local or remote file
            bool local = this->getIContextSimulator()->usingLocalObjects();
            bool success = local ?
                           BlackSim::Fsx::CSimConnectUtilities::writeSimConnectCfg(fileName, address, p) :
                           this->getIContextApplication()->writeToFile(fileName, CSimConnectUtilities::simConnectCfg(address, p));
            if (success)
            {
                QString m = QString("Written ").append(local ? " local " : "remote ").append(fileName);
                this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, m));
            }
            else
            {
                QString m = QString("Cannot write ").append(fileName);
                this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, m));
            }
            this->ui->pb_SettingsFsxExistsSimconncetCfg->click(); // update status
        }

        /*
         * simconnect.cfg: open, delete, exists?
         */
        void CSettingsFsxComponent::simConnectCfgFile()
        {
            if (!this->getIContextSimulator() || !this->getIContextSimulator()->isSimulatorAvailable())
            {
                this->sendStatusMessage(CStatusMessage::getErrorMessage("Simulator not available", CStatusMessage::TypeSimulator));
                return;
            }

            QObject *sender = QObject::sender();
            if (sender == this->ui->pb_SettingsFsxOpenSimconnectCfg)
            {
                QFileInfo fi(CSimConnectUtilities::getLocalSimConnectCfgFilename());
                QString path = QDir::toNativeSeparators(fi.absolutePath());
                QDesktopServices::openUrl(QUrl("file:///" + path));
            }
            else if (sender == this->ui->pb_SettingsFsxDeleteSimconnectCfg)
            {
                if (!this->getIContextSimulator()) return;
                QString fileName = BlackSim::Fsx::CSimConnectUtilities::getLocalSimConnectCfgFilename();
                QString m = QString("Deleted %1 ").append(fileName);
                if (this->getIContextSimulator()->usingLocalObjects())
                {
                    QFile f(fileName);
                    f.remove();
                    m = m.arg("locally");
                }
                else
                {
                    this->getIContextApplication()->removeFile(fileName);
                    m = m.arg("remotely");
                }
                this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeSimulator, CStatusMessage::SeverityInfo, m));
                this->ui->pb_SettingsFsxExistsSimconncetCfg->click(); // update status
            }
            else if (sender == this->ui->pb_SettingsFsxExistsSimconncetCfg)
            {
                if (!this->getIContextSimulator()) return;
                QString fileName = BlackSim::Fsx::CSimConnectUtilities::getLocalSimConnectCfgFilename();
                bool exists = this->getIContextSimulator()->usingLocalObjects() ?
                              QFile::exists(fileName) :
                              this->getIContextApplication()->existsFile(fileName);
                if (exists)
                {
                    this->ui->le_SettingsFsxExistsSimconncetCfg->setText(fileName);
                }
                else
                {
                    this->ui->le_SettingsFsxExistsSimconncetCfg->setText("no file");
                }
            }
        }
    }
}
