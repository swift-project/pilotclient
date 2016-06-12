/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingssimulatorbasicscomponent.h"
#include "ui_settingssimulatorbasicscomponent.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/logmessage.h"
#include "blackconfig/buildconfig.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackConfig;

namespace BlackGui
{
    namespace Components
    {
        CSettingsSimulatorBasicsComponent::CSettingsSimulatorBasicsComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsSimulatorBasicsComponent)
        {
            ui->setupUi(this);
            this->setSmallLayout(true); // no disadvantage, so I always set it
            ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
            ui->comp_SimulatorSelector->setToLastSelection();
            connect(ui->pb_ExcludeFileDialog, &QPushButton::clicked, this, &CSettingsSimulatorBasicsComponent::ps_excludeFileDialog);
            connect(ui->pb_ModelFileDialog, &QPushButton::clicked, this, &CSettingsSimulatorBasicsComponent::ps_modelFileDialog);
            connect(ui->pb_SimulatorFileDialog, &QPushButton::clicked, this, &CSettingsSimulatorBasicsComponent::ps_simulatorFileDialog);
            connect(ui->pb_Save, &QPushButton::clicked, this, &CSettingsSimulatorBasicsComponent::ps_save);
            connect(ui->pb_Reset, &QPushButton::clicked, this, &CSettingsSimulatorBasicsComponent::ps_reset);
            connect(ui->pb_CopyDefaults, &QPushButton::clicked, this, &CSettingsSimulatorBasicsComponent::ps_copyDefaults);
            connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CSettingsSimulatorBasicsComponent::ps_simulatorChanged);
            connect(ui->le_ModelDirectory, &QLineEdit::returnPressed, this, &CSettingsSimulatorBasicsComponent::ps_modelDirectoryEntered);
            connect(ui->le_SimulatorDirectory, &QLineEdit::returnPressed, this, &CSettingsSimulatorBasicsComponent::ps_simulatorDirectoryEntered);

            const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
            this->displayDefaultValuesAsPlaceholder(simulator);
        }

        CSettingsSimulatorBasicsComponent::~CSettingsSimulatorBasicsComponent()
        { }

        void CSettingsSimulatorBasicsComponent::hideSelector(bool show)
        {
            ui->comp_SimulatorSelector->setVisible(show);
        }

        void CSettingsSimulatorBasicsComponent::setSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            ui->comp_SimulatorSelector->setValue(simulator);
        }

        void CSettingsSimulatorBasicsComponent::setSmallLayout(bool small)
        {
            ui->lbl_ExcludeDirectories->setWordWrap(small);
            ui->lbl_ModelDirectory->setWordWrap(small);
            ui->lbl_SimulatorDirectory->setWordWrap(small);
        }

        void CSettingsSimulatorBasicsComponent::ps_simulatorFileDialog()
        {
            const QString startDirectory = this->getBestCurrentSimulatorDirectory();
            const QString dir = QFileDialog::getExistingDirectory(this, tr("Simulator directory"), startDirectory,
                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            if (dir.isEmpty()) { return; }
            ui->le_SimulatorDirectory->setText(CFileUtils::normalizeFilePathToQtStandard(dir));
        }

        void CSettingsSimulatorBasicsComponent::ps_modelFileDialog()
        {
            const QString startDirectory = this->getBestCurrentModelDirectory();
            const QString dir = QFileDialog::getExistingDirectory(this, tr("Model directory"), startDirectory,
                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            if (dir.isEmpty()) { return; }
            ui->le_ModelDirectory->setText(CFileUtils::normalizeFilePathToQtStandard(dir));
        }

        void CSettingsSimulatorBasicsComponent::ps_excludeFileDialog()
        {
            const QString startDirectory = this->getBestCurrentModelDirectory();
            const QString dir = QFileDialog::getExistingDirectory(this, tr("Exclude directory"), startDirectory,
                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            if (dir.isEmpty()) { return; }
            this->addExcludeDirectoryPattern(dir);
        }

        void CSettingsSimulatorBasicsComponent::ps_simulatorDirectoryEntered()
        {
            const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
            const QString sd = CFileUtils::normalizeFilePathToQtStandard(ui->le_SimulatorDirectory->text().trimmed());
            ui->le_SimulatorDirectory->setText(sd);
            this->displayDefaultValuesAsPlaceholder(simulator);
        }

        void CSettingsSimulatorBasicsComponent::ps_modelDirectoryEntered()
        {
            const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
            const QString md = CFileUtils::normalizeFilePathToQtStandard(ui->le_ModelDirectory->text().trimmed());
            ui->le_ModelDirectory->setText(md);
            this->displayDefaultValuesAsPlaceholder(simulator);
        }

        void CSettingsSimulatorBasicsComponent::ps_save()
        {
            const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
            CSettingsSimulator s = this->getSettings(simulator);
            const QString sd(ui->le_SimulatorDirectory->text().trimmed());
            const QString md(ui->le_ModelDirectory->text().trimmed());
            const QStringList ed(this->parseExcludeDirectories());
            const QStringList red = CFileUtils::makeDirectoriesRelative(ed, this->getBestCurrentModelDirectory(), this->m_fileCaseSensitivity);
            s.setSimulatorDirectory(sd);
            s.setModelDirectory(md);
            s.setModelExcludeDirectories(red);
            const CStatusMessage m = this->m_settings.setAndSaveSettings(s, simulator);
            if (!m.isEmpty())
            {
                CLogMessage::preformatted(m);
            }

            // display as saved
            this->displaySettings(simulator);
        }

        void CSettingsSimulatorBasicsComponent::ps_copyDefaults()
        {
            const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
            const QString sd(this->m_settings.getDefaultSimulatorDirectory(simulator));
            ui->le_SimulatorDirectory->setText(CFileUtils::normalizeFilePathToQtStandard(sd));
            const QString md(this->m_settings.getDefaultModelDirectory(simulator));
            ui->le_ModelDirectory->setText(CFileUtils::normalizeFilePathToQtStandard(md));
            const QStringList excludes(this->m_settings.getDefaultModelExcludeDirectoryPatterns(simulator));
            this->displayExcludeDirectoryPatterns(excludes, md);
        }

        void CSettingsSimulatorBasicsComponent::ps_reset()
        {
            const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
            ui->le_SimulatorDirectory->clear();
            ui->le_ModelDirectory->clear();
            ui->pte_ExcludeDirectories->clear();
            this->displayDefaultValuesAsPlaceholder(simulator);
            this->m_settings.resetToDefaults(simulator);
            CLogMessage(this).info("Reset values for settings of %1") << simulator.toQString(true);
        }

        void CSettingsSimulatorBasicsComponent::ps_simulatorChanged()
        {
            const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
            this->displayDefaultValuesAsPlaceholder(simulator);
            this->displaySettings(simulator);
        }

        QStringList CSettingsSimulatorBasicsComponent::parseExcludeDirectories() const
        {
            const QString raw = ui->pte_ExcludeDirectories->toPlainText().trimmed();
            if (raw.isEmpty()) { return QStringList(); }
            QStringList rawLines = raw.split(QRegExp("\n|\r\n|\r"));
            QStringList dirs;
            for (const QString &l : rawLines)
            {
                const QString normalized = CFileUtils::normalizeFilePathToQtStandard(l);
                if (normalized.isEmpty()) { continue; }
                dirs.push_back(normalized);
            }
            dirs.removeDuplicates();
            dirs.sort(this->m_fileCaseSensitivity);
            return dirs;
        }

        void CSettingsSimulatorBasicsComponent::addExcludeDirectoryPattern(const QString &excludeDirectoryPattern)
        {
            const QString d(CFileUtils::normalizeFilePathToQtStandard(excludeDirectoryPattern));
            if (d.isEmpty()) { return; }
            QStringList dirs = this->parseExcludeDirectories();
            if (!dirs.contains(d, this->m_fileCaseSensitivity))
            {
                dirs.push_back(d);
            }
            dirs.removeDuplicates();
            dirs.sort(this->m_fileCaseSensitivity);
            this->displayExcludeDirectoryPatterns(dirs, ui->le_ModelDirectory->text().trimmed());
        }

        void CSettingsSimulatorBasicsComponent::displayExcludeDirectoryPatterns(const QStringList &dirs, const QString &modelDir)
        {
            const QStringList relativeDirectories = CFileUtils::makeDirectoriesRelative(dirs, modelDir);
            const QString d = relativeDirectories.join("\n");
            ui->pte_ExcludeDirectories->setPlainText(d);
        }

        CSettingsSimulator CSettingsSimulatorBasicsComponent::getSettings(const CSimulatorInfo &simulator) const
        {
            const CSettingsSimulator s = this->m_settings.getSettings(simulator);
            return s;
        }

        void CSettingsSimulatorBasicsComponent::displaySettings(const CSimulatorInfo &simulator)
        {
            const CSettingsSimulator s = this->getSettings(simulator);
            this->displayExcludeDirectoryPatterns(s.getModelExcludeDirectoryPatterns(), s.getModelDirectory());
            ui->le_SimulatorDirectory->setText(s.getSimulatorDirectory());
            ui->le_ModelDirectory->setText(s.getModelDirectory());
        }

        void CSettingsSimulatorBasicsComponent::displayDefaultValuesAsPlaceholder(const CSimulatorInfo &simulator)
        {
            const QString s = this->m_settings.getDefaultSimulatorDirectory(simulator);
            ui->le_SimulatorDirectory->setPlaceholderText(s.isEmpty() ? "Simulator directory" : s);

            const QString m = this->m_settings.getDefaultModelDirectory(simulator);
            ui->le_ModelDirectory->setPlaceholderText(m.isEmpty() ? "Model directory" : m);

            const QStringList e = this->m_settings.getDefaultModelExcludeDirectoryPatterns(simulator);
            if (e.isEmpty())
            {
                ui->pte_ExcludeDirectories->setPlaceholderText("Exclude directories");
            }
            else
            {
                const QString es(e.join('\n'));
                ui->pte_ExcludeDirectories->setPlaceholderText(es);
            }
        }

        QString CSettingsSimulatorBasicsComponent::getBestCurrentModelDirectory() const
        {
            const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
            QString md(ui->le_ModelDirectory->text().trimmed());
            if (md.isEmpty())
            {
                md = this->m_settings.getModelDirectoryOrDefault(simulator);
            }
            if (md.isEmpty())
            {
                md = this->getBestCurrentSimulatorDirectory();
            }
            return CFileUtils::normalizeFilePathToQtStandard(md);
        }

        QString CSettingsSimulatorBasicsComponent::getBestCurrentSimulatorDirectory() const
        {
            const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
            QString sd(ui->le_SimulatorDirectory->text().trimmed());
            if (sd.isEmpty())
            {
                sd = this->m_settings.getSimulatorDirectoryOrDefault(simulator);
            }
            return CFileUtils::normalizeFilePathToQtStandard(sd);
        }
    } // ns
} // ns
