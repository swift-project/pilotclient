// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simconnectsettingscomponent.h"
#include "ui_simconnectsettingscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/logmessage.h"
#include "config/buildconfig.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <QStringBuilder>
#include <QDir>

using namespace swift::config;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Fsx;
using namespace BlackMisc::Network;
using namespace BlackGui;

namespace BlackSimPlugin::FsxCommon
{
    CSimConnectSettingsComponent::CSimConnectSettingsComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                                  ui(new Ui::CSimConnectSettingsComponent)
    {
        ui->setupUi(this);

        connect(ui->pb_OpenSwiftSimConnectCfg, &QPushButton::clicked, this, &CSimConnectSettingsComponent::openSwiftSimConnectCfgFile);
        connect(ui->pb_DeleteSwiftSimConnectCfg, &QPushButton::clicked, this, &CSimConnectSettingsComponent::deleteSwiftSimConnectCfgFile);
        connect(ui->pb_ExistsSimConnectCfg, &QPushButton::clicked, this, &CSimConnectSettingsComponent::checkSwiftSimConnectCfgFile);
        connect(ui->pb_SaveSwiftSimConnectCfg, &QPushButton::clicked, this, &CSimConnectSettingsComponent::saveSimConnectCfgFile);
        connect(ui->pb_OpenUserCfgFile, &QPushButton::clicked, this, &CSimConnectSettingsComponent::openUserSimConnectCfgFile);
        connect(ui->pb_TestConnection, &QPushButton::clicked, this, &CSimConnectSettingsComponent::testSwiftSimConnectConnection);
        connect(ui->pb_SaveAsSimConnectIni, &QPushButton::clicked, this, &CSimConnectSettingsComponent::saveSimConnectIniFileDialog);
        this->setSimConnectInfo();

        if (m_p3d64bit)
        {
            connect(ui->cb_P3DVersion, &QComboBox::currentTextChanged, this, &CSimConnectSettingsComponent::onP3DVersionChanged);
        }
    }

    CSimConnectSettingsComponent::~CSimConnectSettingsComponent()
    {
        // void
    }

    void CSimConnectSettingsComponent::openSwiftSimConnectCfgFile()
    {
        if (!CSimConnectUtilities::hasSwiftLocalSimConnectCfgFile()) { return; }
        const QFileInfo info(CSimConnectUtilities::getSwiftLocalSimConnectCfgFilename());
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.absoluteFilePath()));
    }

    void CSimConnectSettingsComponent::openUserSimConnectCfgFile()
    {
        if (!CSimConnectUtilities::hasUserSimConnectCfgFile()) { return; }
        const QFileInfo info(CSimConnectUtilities::getUserSimConnectCfgFilename());
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.absoluteFilePath()));
    }

    void CSimConnectSettingsComponent::deleteSwiftSimConnectCfgFile()
    {
        const QString fileName = CSimConnectUtilities::getSwiftLocalSimConnectCfgFilename();
        QFile file(fileName);
        const bool result = file.exists() && file.remove();
        if (result)
        {
            QMessageBox::information(qApp->activeWindow(), tr("File deleted"), tr("File %1 deleted successfully.").arg(fileName));
        }
        this->checkSwiftSimConnectCfgFile();
    }

    void CSimConnectSettingsComponent::checkSwiftSimConnectCfgFile()
    {
        // this works for local files only
        const QString fileName = CSimConnectUtilities::getSwiftLocalSimConnectCfgFilename();
        const QFile file(fileName);
        ui->le_ExistsSimConnectCfg->setText(file.exists() ? fileName : "no file");

        // only works for local file (which the SimConnect file normally is)
        if (!CSimConnectUtilities::hasSwiftLocalSimConnectCfgFile()) { return; }
        const QSharedPointer<QSettings> settings = CSimConnectUtilities::simConnectFileAsSettings();
        if (!settings) { return; }
        const QString address = CSimConnectUtilities::ipAddress(settings.data());
        const int port = CSimConnectUtilities::ipPort(settings.data());
        if (!address.isEmpty())
        {
            ui->le_Address->setText(address);
        }
        if (port > 0)
        {
            ui->le_Port->setText(QString::number(port));
        }
    }

    void CSimConnectSettingsComponent::testSwiftSimConnectConnection()
    {
        const QString address = ui->le_Address->text().trimmed();
        const QString port = ui->le_Port->text().trimmed();

        if (address.isEmpty() || port.isEmpty())
        {
            QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"), tr("Address and/or port not specified!"));
            return;
        }
        if (!CNetworkUtils::isValidIPv4Address(address))
        {
            QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"), tr("Wrong IPv4 address!"));
            return;
        }
        if (!CNetworkUtils::isValidPort(port))
        {
            QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"), tr("Invalid port!"));
            return;
        }
        const int p = port.toInt();
        QString msg;
        if (!CNetworkUtils::canConnect(address, p, msg))
        {
            QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"), msg);
            return;
        }

        QMessageBox::information(qApp->activeWindow(), tr("Connection successful"), tr("Connected to '%1:%2'.").arg(address, port));
    }

    void CSimConnectSettingsComponent::saveSimConnectCfgFile()
    {
        const QString address = ui->le_Address->text().trimmed();
        const QString port = ui->le_Port->text().trimmed();

        if (address.isEmpty() || port.isEmpty())
        {
            QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"), tr("Address and/or port not specified!"));
            return;
        }
        if (!CNetworkUtils::isValidIPv4Address(address))
        {
            QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"), tr("Wrong IPv4 address!"));
            return;
        }
        if (!CNetworkUtils::isValidPort(port))
        {
            QMessageBox::warning(qApp->activeWindow(), tr("Connection invalid"), tr("Invalid port!"));
            return;
        }

        const int p = port.toInt();
        QString fileName;

        if (sGui->getIContextSimulator())
        {
            const BlackMisc::Simulation::CSimulatorInternals internals(sGui->getIContextSimulator()->getSimulatorInternals());
            fileName = internals.getStringValue("fsx/SimConnectCfgFilename");
        }

        if (fileName.isEmpty())
        {
            fileName = CSimConnectUtilities::getSwiftLocalSimConnectCfgFilename();
        }

        if (fileName.isEmpty())
        {
            QMessageBox::warning(qApp->activeWindow(), tr("Failed writing SimConnect.cfg"), tr("No file name specified!"));
            return;
        }

        if (CFileUtils::writeStringToFile(CSimConnectUtilities::simConnectCfg(address, p), fileName))
        {
            QMessageBox::information(qApp->activeWindow(), tr("File saved"), tr("File '%1' saved.").arg(fileName));
            this->checkSwiftSimConnectCfgFile();
        }
        else
        {
            QMessageBox::warning(qApp->activeWindow(), tr("Failed writing SimConnect.cfg"), tr("Failed writing '%1'!").arg(fileName));
        }
    }

    void CSimConnectSettingsComponent::saveSimConnectIniFileDialog()
    {
        const QString iniFile = ui->pte_SimConnectIni->toPlainText();
        const QString dir = CSimConnectUtilities::getSimConnectIniFileDirectory(m_simulator);
        bool madeDir = false;
        QDir d(dir);
        if (!d.exists())
        {
            d.mkdir(dir);
            madeDir = true;
        }

        const QString defaultFileName = CFileUtils::appendFilePaths(dir, CSimConnectUtilities::simConnectIniFilename());
        const QString fileName = QFileDialog::getSaveFileName(this, tr("Save SimConnect.ini"),
                                                              defaultFileName, tr("FSX/P3D ini files (*.ini)"));
        if (!fileName.isEmpty())
        {
            const bool written = CFileUtils::writeStringToFile(iniFile, fileName);
            if (!written && madeDir) { d.removeRecursively(); } // clean up own created dir
            if (written) { CLogMessage(this).info(u"Written '%1'") << fileName; }
        }
        // always refresh info, as the dialog can also be used to delete the file
        this->setSimConnectInfo();
    }

    void CSimConnectSettingsComponent::setSimConnectInfo()
    {
        if (CBuildConfig::isCompiledWithP3DSupport() && CBuildConfig::buildWordSize() == 64)
        {
            ui->pte_SimConnectInfo->setPlainText("Static linking P3Dv4 x64");
            m_simulator = CSimulatorInfo(CSimulatorInfo::P3D);
            m_p3d64bit = true;

            const QString v = m_p3dVersion.get();
            this->setComboBox(v);
        }
        else
        {
            const CWinDllUtils::DLLInfo SimConnectInfo = CSimConnectUtilities::simConnectDllInfo();
            ui->pte_SimConnectInfo->setPlainText(SimConnectInfo.summary());
            m_simulator = CSimulatorInfo(CSimulatorInfo::FSX);
            m_p3d64bit = false;
        }

        ui->lbl_P3DVersion->setVisible(m_p3d64bit);
        ui->cb_P3DVersion->setVisible(m_p3d64bit);

        ui->le_UserCfgFile->setText(CSimConnectUtilities::hasUserSimConnectCfgFile() ? CSimConnectUtilities::getUserSimConnectCfgFilename() : "");
        const QString iniFiles = CSimConnectUtilities::getSimConnectIniFiles().join("\n");
        ui->pte_SimConnectIniFiles->setPlainText(iniFiles);
        this->checkSwiftSimConnectCfgFile();
    }

    void CSimConnectSettingsComponent::setComboBox(const QString &value)
    {
        QString v;
        bool found = false;
        for (int index = 0; index < ui->cb_P3DVersion->count(); index++)
        {
            v = ui->cb_P3DVersion->itemText(index);
            if (v.contains(value, Qt::CaseInsensitive))
            {
                found = true;
                break;
            }
        }
        ui->cb_P3DVersion->setCurrentText(found ? v : "");
    }

    void CSimConnectSettingsComponent::onP3DVersionChanged(const QString &version)
    {
        if (m_p3dVersion.get() == version) { return; }
        const CStatusMessage saveMsg = m_p3dVersion.setAndSave(version);
        if (saveMsg.isSuccess())
        {
            const CStatusMessage m = CStatusMessage(this).info(u"Changed P3D version to '%1'. Requires a new start of swift to become effective!") << version;
            this->showOverlayMessage(m);
        }
    }
} // ns
