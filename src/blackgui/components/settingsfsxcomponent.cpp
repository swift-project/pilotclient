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
#include "blackcore/context_simulator.h"
#include "blackcore/context_application.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/fsx/fsxsimulatorsetup.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include <QDesktopServices>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QFileInfo>

using namespace BlackMisc;
using namespace BlackMisc::Simulation::Fsx;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CSettingsFsxComponent::CSettingsFsxComponent(QWidget *parent) :
            QFrame(parent), CEnableForRuntime(nullptr, false), ui(new Ui::CSettingsFsxComponent)
        {
            ui->setupUi(this);

            this->connect(this->ui->pb_SettingsFsxTestConnection, &QPushButton::clicked, this, &CSettingsFsxComponent::testSimConnectConnection);
            this->connect(this->ui->pb_SettingsFsxSaveSimconnectCfg, &QPushButton::clicked, this, &CSettingsFsxComponent::saveSimConnectCfg);
            this->connect(this->ui->pb_SettingsFsxOpenSimconnectCfg, &QPushButton::clicked, this, &CSettingsFsxComponent::simConnectCfgFile);
            this->connect(this->ui->pb_SettingsFsxDeleteSimconnectCfg, &QPushButton::clicked, this, &CSettingsFsxComponent::simConnectCfgFile);
            this->connect(this->ui->pb_SettingsFsxExistsSimconncetCfg, &QPushButton::clicked, this, &CSettingsFsxComponent::simConnectCfgFile);
        }

        CSettingsFsxComponent::~CSettingsFsxComponent()
        { }

        /*
         * SimConnect working?
         */
        void CSettingsFsxComponent::testSimConnectConnection()
        {
            QString address = this->ui->le_SettingsFsxAddress->text().trimmed();
            QString port = this->ui->le_SettingsFsxPort->text().trimmed();

            if (address.isEmpty() || port.isEmpty())
            {
                CLogMessage(this).validationWarning("No address or port");
                return;
            }
            if (!CNetworkUtils::isValidIPv4Address(address))
            {
                CLogMessage(this).validationWarning("IPv4 address invalid");
                return;
            }
            if (!CNetworkUtils::isValidPort(port))
            {
                CLogMessage(this).validationWarning("Invalid port");
                return;
            }
            int p = port.toInt();
            QString msg;
            if (!CNetworkUtils::canConnect(address, p, msg))
            {
                CLogMessage(this).validationWarning(msg);
                return;
            }

            CLogMessage(this).validationInfo("Connected to %1:%2") << address << port;
        }

        /*
         * Save simconnect.cfg
         */
        void CSettingsFsxComponent::saveSimConnectCfg()
        {
            if (!this->getIContextSimulator())
            {
                CLogMessage(this).validationError("Simulator driver not available");
                return;
            }
            QString address = this->ui->le_SettingsFsxAddress->text().trimmed();
            QString port = this->ui->le_SettingsFsxPort->text().trimmed();

            if (address.isEmpty() || port.isEmpty())
            {
                CLogMessage(this).validationError("No address or port");
                return;
            }
            if (!CNetworkUtils::isValidIPv4Address(address))
            {
                CLogMessage(this).validationError("IPv4 address invalid");
                return;
            }
            if (!CNetworkUtils::isValidPort(port))
            {
                CLogMessage(this).validationError("Invalid port");
                return;
            }
            int p = port.toInt();

            //! \todo filename is only available if driver has been loaded
            QString fileName = this->getIContextSimulator()->getSimulatorSetup().getStringValue(CFsxSimulatorSetup::KeyLocalSimConnectCfgFilename());
            if (fileName.isEmpty())
            {
                CLogMessage(this).validationError("Invalid or empty filename empty, driver loaded?");
                return;
            }

            // write either local or remote file
            bool localSimulatorObject = this->getIContextSimulator()->isUsingImplementingObject();
            bool success = localSimulatorObject ?
                           BlackMisc::Simulation::Fsx::CSimConnectUtilities::writeSimConnectCfg(fileName, address, p) :
                           this->getIContextApplication()->writeToFile(fileName, CSimConnectUtilities::simConnectCfg(address, p));
            if (success)
            {
                CLogMessage(this).validationInfo(localSimulatorObject ? "Written local %1" : "Written remote %1") << fileName;
            }
            else
            {
                CLogMessage(this).validationError("Cannot write %1") << fileName;
            }
            this->ui->pb_SettingsFsxExistsSimconncetCfg->click(); // update status
        }

        /*
         * simconnect.cfg: open, delete, exists?
         */
        void CSettingsFsxComponent::simConnectCfgFile()
        {
            if (!this->getIContextSimulator())
            {
                CLogMessage(this).validationError("Simulator driver not available");
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
                QString fileName = BlackMisc::Simulation::Fsx::CSimConnectUtilities::getLocalSimConnectCfgFilename();
                if (this->getIContextSimulator()->isUsingImplementingObject())
                {
                    QFile f(fileName);
                    f.remove();
                    CLogMessage(this).info("Deleted locally %1") << fileName;
                }
                else
                {
                    this->getIContextApplication()->removeFile(fileName);
                    CLogMessage(this).info("Deleted remotely %1") << fileName;
                }
                this->ui->pb_SettingsFsxExistsSimconncetCfg->click(); // update status
            }
            else if (sender == this->ui->pb_SettingsFsxExistsSimconncetCfg)
            {
                if (!this->getIContextSimulator()) return;
                QString fileName = BlackMisc::Simulation::Fsx::CSimConnectUtilities::getLocalSimConnectCfgFilename();
                bool exists = this->getIContextSimulator()->isUsingImplementingObject() ?
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
    } // ns
} // ns
