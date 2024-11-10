// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "installfsxterrainprobecomponent.h"
#include "ui_installfsxterrainprobecomponent.h"
#include "gui/overlaymessagesframe.h"
#include "gui/guiutility.h"
#include "misc/simulation/fscommon/fscommonutil.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/swiftdirectories.h"
#include "misc/directoryutils.h"
#include "misc/fileutils.h"

#include <QPushButton>
#include <QFileDialog>
#include <QPointer>
#include <QTimer>

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;

namespace swift::gui::components
{
    CInstallFsxTerrainProbeComponent::CInstallFsxTerrainProbeComponent(QWidget *parent) : QFrame(parent),
                                                                                          ui(new Ui::CInstallFsxTerrainProbeComponent)
    {
        ui->setupUi(this);
        ui->le_Source->setText(CSwiftDirectories::shareTerrainProbeDirectory());

        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->comp_SimulatorSelector->setFsxP3DOnly();
        ui->comp_SimulatorSelector->setValue(CSimulatorInfo::fsx());

        connect(ui->pb_Copy, &QPushButton::released, this, &CInstallFsxTerrainProbeComponent::copyProbe);
        connect(ui->pb_DirectoryBrowser, &QPushButton::released, this, &CInstallFsxTerrainProbeComponent::selectSimObjectsDir);
        connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CInstallFsxTerrainProbeComponent::onSimulatorChanged);

        QPointer<CInstallFsxTerrainProbeComponent> myself(this);
        QTimer::singleShot(500, this, [=] {
            if (!myself) { return; }
            this->onSimulatorChanged(ui->comp_SimulatorSelector->getValue());
        });
    }

    CInstallFsxTerrainProbeComponent::~CInstallFsxTerrainProbeComponent()
    {}

    void CInstallFsxTerrainProbeComponent::copyProbe()
    {
        if (ui->le_Target->text().isEmpty()) { return; }
        CStatusMessageList msgs;
        const int copied = CFsCommonUtil::copyFsxTerrainProbeFiles(ui->le_Target->text(), msgs);
        COverlayMessagesWizardPage *mf = CGuiUtility::nextOverlayMessageWizardPage(this);
        if (mf) { mf->showOverlayMessages(msgs, false, 5000); }
        Q_UNUSED(copied);
    }

    void CInstallFsxTerrainProbeComponent::onSimulatorChanged(const CSimulatorInfo &simulator)
    {
        // avoid invalid simulators
        CSimulatorInfo sim = simulator;
        if (!sim.isFsxP3DFamily()) { sim = CSimulatorInfo::p3d(); }

        // model directories
        const QStringList modelDirs = m_simulatorSettings.getModelDirectoriesOrDefault(sim);
        if (!modelDirs.isEmpty() && !modelDirs.front().isEmpty())
        {
            ui->le_Target->setText(modelDirs.front());
        }
        else
        {
            ui->le_Target->clear();
        }
    }

    void CInstallFsxTerrainProbeComponent::selectSimObjectsDir()
    {
        const QString startDirectory = CFileUtils::fixWindowsUncPath(ui->le_Target->text());
        const QString dir =
            CFileUtils::fixWindowsUncPath(
                QFileDialog::getExistingDirectory(
                    this, "SimObjects directory", startDirectory,
                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
        ui->le_Target->setText(dir);
    }
} // ns
