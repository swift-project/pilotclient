// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simconnectsettingscomponent.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStringBuilder>

#include "ui_simconnectsettingscomponent.h"

#include "config/buildconfig.h"
#include "core/context/contextapplication.h"
#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "misc/logmessage.h"
#include "misc/network/networkutils.h"
#include "misc/simulation/fsx/simconnectutilities.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fsx;
using namespace swift::misc::network;
using namespace swift::gui;

namespace swift::simplugin::fsxcommon
{
    CSimConnectSettingsComponent::CSimConnectSettingsComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CSimConnectSettingsComponent)
    {
        ui->setupUi(this);

        this->setSimConnectInfo();

        if (m_p3d64bit)
        {
            connect(ui->cb_P3DVersion, &QComboBox::currentTextChanged, this,
                    &CSimConnectSettingsComponent::onP3DVersionChanged);
        }
    }

    CSimConnectSettingsComponent::~CSimConnectSettingsComponent()
    {
        // void
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
            const CStatusMessage m =
                CStatusMessage(this).info(
                    u"Changed P3D version to '%1'. Requires a new start of swift to become effective!")
                << version;
            this->showOverlayMessage(m);
        }
    }
} // namespace swift::simplugin::fsxcommon
