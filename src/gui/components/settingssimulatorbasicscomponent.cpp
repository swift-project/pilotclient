// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingssimulatorbasicscomponent.h"

#include <QMessageBox>
#include <QRegularExpression>

#include "ui_settingssimulatorbasicscomponent.h"

#include "config/buildconfig.h"
#include "misc/fileutils.h"
#include "misc/htmlutils.h"
#include "misc/logmessage.h"
#include "misc/simulation/xplane/xplaneutil.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;
using namespace swift::misc::simulation::settings;
using namespace swift::misc::simulation::xplane;
using namespace swift::config;

namespace swift::gui::components
{
    const QStringList &CSettingsSimulatorBasicsComponent::getLogCategories()
    {
        static const QStringList cats({ CLogCategories::guiComponent(), CLogCategories::wizard() });
        return cats;
    }

    CSettingsSimulatorBasicsComponent::CSettingsSimulatorBasicsComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CSettingsSimulatorBasicsComponent)
    {
        ui->setupUi(this);
        this->setSmallLayout(true); // no disadvantage, so I always set it
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->comp_SimulatorSelector->setRememberSelectionAndSetToLastSelection();

        connect(ui->pb_ExcludeFileDialog, &QPushButton::clicked, this,
                &CSettingsSimulatorBasicsComponent::excludeFileDialog);
        connect(ui->pb_ModelFileDialog, &QPushButton::clicked, this,
                &CSettingsSimulatorBasicsComponent::modelFileDialog);
        connect(ui->pb_SimulatorFileDialog, &QPushButton::clicked, this,
                &CSettingsSimulatorBasicsComponent::simulatorFileDialog);
        connect(ui->pb_Save, &QPushButton::clicked, this, &CSettingsSimulatorBasicsComponent::save);
        connect(ui->pb_Reset, &QPushButton::clicked, this, &CSettingsSimulatorBasicsComponent::reset);
        connect(ui->pb_CopyDefaults, &QPushButton::clicked, this, &CSettingsSimulatorBasicsComponent::copyDefaults);
        connect(ui->pb_AdjustModelDirectory, &QPushButton::clicked, this,
                &CSettingsSimulatorBasicsComponent::adjustModelDirectory);
        connect(ui->le_SimulatorDirectory, &QLineEdit::returnPressed, this,
                &CSettingsSimulatorBasicsComponent::simulatorDirectoryEntered);
        connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this,
                &CSettingsSimulatorBasicsComponent::onSimulatorChanged);
        connect(&m_settings, &CMultiSimulatorSettings::settingsChanged, this,
                &CSettingsSimulatorBasicsComponent::onSimulatorSettingsChanged);

        this->onSimulatorChanged();
    }

    CSettingsSimulatorBasicsComponent::~CSettingsSimulatorBasicsComponent() = default;

    void CSettingsSimulatorBasicsComponent::hideSelector(bool show) { ui->comp_SimulatorSelector->setVisible(show); }

    bool CSettingsSimulatorBasicsComponent::hasAnyValues() const
    {
        return !ui->le_SimulatorDirectory->text().isEmpty() || !ui->pte_ModelDirectories->toPlainText().isEmpty() ||
               !ui->pte_ExcludeDirectories->toPlainText().isEmpty();
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

    void CSettingsSimulatorBasicsComponent::simulatorFileDialog()
    {
        const QString startDirectory = CFileUtils::fixWindowsUncPath(this->getFileBrowserSimulatorDirectory());
        const QString dir =
            QFileDialog::getExistingDirectory(this, tr("Simulator directory"), startDirectory,
                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (dir.isEmpty()) { return; }
        ui->le_SimulatorDirectory->setText(CFileUtils::normalizeFilePathToQtStandard(dir));
        this->adjustModelDirectory();
    }

    void CSettingsSimulatorBasicsComponent::modelFileDialog()
    {
        const QString startDirectory = CFileUtils::fixWindowsUncPath(this->getFileBrowserModelDirectory());
        const QString dir = QFileDialog::getExistingDirectory(
            this, tr("Model directory"), startDirectory, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (dir.isEmpty()) { return; }
        m_unsavedChanges = true;
        const QStringList newDirs = this->addDirectory(dir, this->parseModelDirectories());
        this->displayModelDirectories(newDirs);
    }

    void CSettingsSimulatorBasicsComponent::excludeFileDialog()
    {
        const QString startDirectory = this->getFileBrowserModelDirectory();
        const QString dir =
            QFileDialog::getExistingDirectory(this, tr("Exclude directory"), startDirectory,
                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (dir.isEmpty()) { return; }
        m_unsavedChanges = true;
        const QStringList newDirs =
            CFileUtils::stripLeadingSlashOrDriveLetters(this->addDirectory(dir, this->parseExcludeDirectories()));
        this->displayExcludeDirectoryPatterns(newDirs);
    }

    void CSettingsSimulatorBasicsComponent::simulatorDirectoryEntered()
    {
        const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
        const QString simDir = CFileUtils::normalizeFilePathToQtStandard(ui->le_SimulatorDirectory->text().trimmed());
        ui->le_SimulatorDirectory->setText(simDir);
        this->displayDefaultValuesAsPlaceholder(simulator);
    }

    void CSettingsSimulatorBasicsComponent::save()
    {
        using namespace std::chrono_literals;
        const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
        CSimulatorSettings s = this->getSettings(simulator).getGenericSettings();
        const QString simulatorDir(ui->le_SimulatorDirectory->text().trimmed());
        const QStringList modelDirs(this->parseModelDirectories());
        const QStringList excludeDirs(this->parseDirectories(ui->pte_ExcludeDirectories->toPlainText()));

        const QStringList relativeDirs = CFileUtils::makeDirectoriesRelative(
            excludeDirs, this->getFileBrowserModelDirectory(), m_fileCaseSensitivity);
        s.setSimulatorDirectory(simulatorDir);
        s.setModelDirectories(modelDirs);
        s.setModelExcludeDirectories(relativeDirs);
        s.setPropertyModelSet(ui->cb_LoadNewModelset->isChecked());
        s.setPropertyWithDbEntry(ui->cb_WithDbEntry->isChecked());
        s.setPropertyDistributorFiltered(ui->cb_DistributorFiltered->isChecked());

        const CStatusMessageList msgs = m_settings.setAndValidateSettings(s, simulator);
        if (msgs.isSuccess())
        {
            const CStatusMessage m = m_settings.setAndSaveSettings(s, simulator);
            if (!m.isEmpty()) { CLogMessage::preformatted(m); }
            if (m.isSuccess()) { this->showOverlayHTMLMessage("Saved settings", 2s); }
            else { this->showOverlayMessage(m); }
            m_unsavedChanges = m_unsavedChanges && !m.isSuccess(); // reset if success, but only if there were changes

            // display as it was saved
            this->displaySettings(simulator);
        }
        else { this->showOverlayMessagesOrHTMLMessage(msgs); }
    }

    void CSettingsSimulatorBasicsComponent::copyDefaults()
    {
        const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
        const bool anyValues = this->hasAnyValues();
        if (anyValues)
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Override", "Override existing values?",
                                                                      QMessageBox::Yes | QMessageBox::No);
            if (reply != QMessageBox::Yes) { return; }
        }

        // override if values are not empty
        const CSpecializedSimulatorSettings ss = m_settings.getSpecializedSettings(simulator);
        const QString sd = CFileUtils::fixWindowsUncPath(CFileUtils::normalizeFilePathToQtStandard(
            CSpecializedSimulatorSettings::defaultSimulatorDirectory(simulator)));
        if (!sd.isEmpty())
        {
            ui->le_SimulatorDirectory->setText(sd);
            m_unsavedChanges = true;
        }

        const QStringList md(m_settings.defaultModelDirectories(simulator));
        if (!md.isEmpty())
        {
            this->displayModelDirectories(md);
            m_unsavedChanges = true;
        }

        const QStringList excludes(CSpecializedSimulatorSettings::defaultModelExcludeDirectoryPatterns(simulator));
        if (!excludes.isEmpty())
        {
            this->displayExcludeDirectoryPatterns(excludes);
            m_unsavedChanges = true;
        }
    }

    void CSettingsSimulatorBasicsComponent::adjustModelDirectory()
    {
        const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
        const QString simDir = this->getFileBrowserSimulatorDirectory();
        CSpecializedSimulatorSettings s = m_settings.getSpecializedSettings(simulator);
        s.setSimulatorDirectory(simDir);

        // There is not really a fixed place in the X-Plane install directory where models are put.
        // We just treat the whole X-Plane directory as model directory and search for models in all subdirectories
        // recursively.

        const QStringList parsedDirectories = this->parseModelDirectories();
        const QStringList newDirs =
            parsedDirectories.size() > 1 ?
                parsedDirectories :
                this->removeDirectories(s.getModelDirectoriesFromSimulatorDirectoryOrDefault(), parsedDirectories);
        this->displayModelDirectories(newDirs);
    }

    void CSettingsSimulatorBasicsComponent::reset()
    {
        const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());

        m_settings.resetToDefaults(simulator);
        m_unsavedChanges = true;

        ui->le_SimulatorDirectory->clear();
        ui->pte_ModelDirectories->clear();
        ui->pte_ExcludeDirectories->clear();
        this->displayDefaultValuesAsPlaceholder(simulator);

        CLogMessage(this).info(u"Reset values for settings of %1") << simulator.toQString(true);
    }

    void CSettingsSimulatorBasicsComponent::onSimulatorChanged()
    {
        const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
        this->displaySettings(simulator);

        // special handling for MSFS 2024 only visual changes
        if (simulator == CSimulatorInfo::msfs2024())
        {
            ui->gb_ModelSet->setVisible(true);
            ui->lbl_ExcludeDirectories->setText(QStringLiteral("modelstring exclude patterns:"));
            ui->lbl_ModelDirectory->setText(QStringLiteral("modelstring filter patterns:"));
            ui->lbl_ModelDirectory->setToolTip(
                QStringLiteral("If the field is not empty, these patterns are used as a filter for the model string."));
            ui->pb_AdjustModelDirectory->setVisible(false);
            ui->pb_ExcludeFileDialog->setVisible(false);
            ui->pb_ModelFileDialog->setVisible(false);
            const QString html =
                "<html><head/><body><p><img src=':/diagona/icons/diagona/icons/exclamation--frame.png'/>&quot;These "
                "filter settings are applied to the model string passed from the flight simulator to swift. If the "
                "effect is unclear, start with the default settings!&quot;.</p></body></html>";
            ui->lbl_ModelDirsInfo->setText(html);
        }
        else
        {
            ui->gb_ModelSet->setVisible(false);
            ui->lbl_ExcludeDirectories->setText(QStringLiteral("Exclude directory patterns:"));
            ui->lbl_ModelDirectory->setText(QStringLiteral("Model directories:"));
            ui->lbl_ModelDirectory->setToolTip(QStringLiteral("remove redundant directories, fix file paths .."));
            ui->pb_AdjustModelDirectory->setVisible(true);
            ui->pb_ExcludeFileDialog->setVisible(true);
            ui->pb_ModelFileDialog->setVisible(true);
            const QString html =
                "<html><head/><body><p><img "
                "src=':/diagona/icons/diagona/icons/exclamation--frame.png'/>&quot;If you change the model "
                "directories, you must update your model set. See documentation. &quot;.</p></body></html>";
            ui->lbl_ModelDirsInfo->setText(html);
        }

        this->displayDefaultValuesAsPlaceholder(simulator);
    }

    void CSettingsSimulatorBasicsComponent::onSimulatorSettingsChanged(const CSimulatorInfo &simulator)
    {
        const CSimulatorInfo selectedSimulator(ui->comp_SimulatorSelector->getValue());
        if (selectedSimulator == simulator) { this->displaySettings(simulator); }
    }

    QStringList CSettingsSimulatorBasicsComponent::parseModelDirectories() const
    {
        return this->parseDirectories(ui->pte_ModelDirectories->toPlainText());
    }

    QStringList CSettingsSimulatorBasicsComponent::parseExcludeDirectories() const
    {
        return this->parseDirectories(ui->pte_ExcludeDirectories->toPlainText());
    }

    QStringList CSettingsSimulatorBasicsComponent::parseDirectories(const QString &rawString) const
    {
        const QString raw = rawString.trimmed();
        if (raw.isEmpty()) { return {}; }
        QStringList dirs;
        thread_local const QRegularExpression regExp("\n|\r\n|\r");
        const QStringList rawLines = raw.split(regExp);
        for (const QString &l : rawLines)
        {
            const QString normalized = CFileUtils::normalizeFilePathToQtStandard(l);
            if (normalized.isEmpty()) { continue; }
            dirs.push_back(normalized);
        }
        dirs = CFileUtils::removeSubDirectories(dirs);
        return dirs;
    }

    QStringList CSettingsSimulatorBasicsComponent::addDirectory(const QString &directory,
                                                                const QStringList &existingDirs)
    {
        const QString d(CFileUtils::normalizeFilePathToQtStandard(directory));
        QStringList dirs(existingDirs);
        if (d.isEmpty()) { return existingDirs; }
        if (!dirs.contains(d, m_fileCaseSensitivity)) { dirs.push_back(d); }
        dirs.removeDuplicates();
        dirs.sort(m_fileCaseSensitivity);
        return dirs;
    }

    QStringList CSettingsSimulatorBasicsComponent::removeDirectory(const QString &directory,
                                                                   const QStringList &existingDirs)
    {
        const QString d(CFileUtils::normalizeFilePathToQtStandard(directory));
        return this->removeDirectories(QStringList({ d }), existingDirs);
    }

    QStringList CSettingsSimulatorBasicsComponent::removeDirectories(const QStringList &removeDirectories,
                                                                     const QStringList &existingDirs)
    {
        if (existingDirs.isEmpty() || removeDirectories.isEmpty()) { return existingDirs; }
        const QStringList rDirs = CFileUtils::fixWindowsUncPaths(removeDirectories);
        const QStringList eDirs = CFileUtils::fixWindowsUncPaths(existingDirs);
        QStringList dirs;
        for (const QString &dir : eDirs)
        {
            if (rDirs.contains(dir, m_fileCaseSensitivity)) { continue; }
            dirs.push_back(dir);
        }
        dirs.removeDuplicates();
        dirs.sort(m_fileCaseSensitivity);
        return dirs;
    }

    void CSettingsSimulatorBasicsComponent::displayExcludeDirectoryPatterns(const QStringList &dirs)
    {
        QStringList cleanedDirs(dirs);
        cleanedDirs.removeDuplicates();
        cleanedDirs.sort(m_fileCaseSensitivity);
        const QString d = cleanedDirs.join("\n");
        ui->pte_ExcludeDirectories->setPlainText(d);
    }

    void CSettingsSimulatorBasicsComponent::displayModelDirectories(const QStringList &dirs)
    {
        QStringList cleanedDirs(dirs);
        cleanedDirs.removeDuplicates();
        cleanedDirs.sort(m_fileCaseSensitivity);
        const QString d = cleanedDirs.join("\n");
        ui->pte_ModelDirectories->setPlainText(d);
        this->displayDefaultValuesAsPlaceholder(ui->comp_SimulatorSelector->getValue());
    }

    CSpecializedSimulatorSettings CSettingsSimulatorBasicsComponent::getSettings(const CSimulatorInfo &simulator) const
    {
        const CSpecializedSimulatorSettings s = m_settings.getSpecializedSettings(simulator);
        return s;
    }

    void CSettingsSimulatorBasicsComponent::displaySettings(const CSimulatorInfo &simulator)
    {
        // if (simulator.isMSFS2024())
        //{

        //    //CSpecializedSimulatorSettings settings = this->getSimulatorSettings(simulator.isMSFS2024());
        //    //CSimulatorSettings m_generic = settings.getGenericSettings();
        //    //QStringList excludePatterns = m_generic.getModelExcludeDirectoryPatterns();
        //    //QStringList filterPatterns = m_generic.getModelDirectories();

        //    //const CSimulatorSettings s = m_settings.getSettings(simulator);
        //    //const QString es = s.getSimulatorDirectory();
        //    ui->pte_ModelDirectories->clear();
        //    ui->pte_ExcludeDirectories->clear();

        //    //this->displayModelDirectories(excludePatterns);
        //    //this->displayExcludeDirectoryPatterns(filterPatterns);
        //}
        // else
        //{
        //    this->displayModelDirectories(m_settings.getModelDirectoriesIfNotDefault(simulator));
        //    this->displayExcludeDirectoryPatterns(m_settings.getModelExcludeDirectoryPatternsIfNotDefault(simulator));
        //}

        // ui->le_SimulatorDirectory->setText(m_settings.getSimulatorDirectoryIfNotDefault(simulator));
        // based on discussion here, always display:
        // https://discordapp.com/channels/539048679160676382/594962359441948682/700483609361907842
        const CSimulatorSettings s = m_settings.getSettings(simulator);
        ui->le_SimulatorDirectory->setText(s.getSimulatorDirectory());
    }

    void CSettingsSimulatorBasicsComponent::displayDefaultValuesAsPlaceholder(const CSimulatorInfo &simulator)
    {
        const QString simDir = this->getFileBrowserSimulatorDirectory();

        // only real placeholder shut set as placeholder
        simDir.isEmpty() ? (ui->le_SimulatorDirectory->setPlaceholderText("Simulator directory")) :
                           (ui->le_SimulatorDirectory->setText(simDir));

        // we take the settings and update to latest sim.directory
        CSpecializedSimulatorSettings settings = m_settings.getSpecializedSettings(simulator);
        settings.setSimulatorDirectory(simDir);

        CSimulatorSettings m_generic = settings.getGenericSettings();

        // this setting is only visable for msfs2024 but we set it always to keep code simple. the checkbox is hidden in
        // other simulators this settings are used to load/not load the modelset when starting swiftgui
        ui->cb_WithDbEntry->setChecked(m_generic.getPropertyWithDbEntry());
        ui->cb_DistributorFiltered->setChecked(m_generic.getPropertyDistributorFiltered());
        ui->cb_LoadNewModelset->setChecked(m_generic.getPropertyModelSet());

        // Storeable content should be displayed as planetext so that it is actually saved.
        if (simulator.isMSFS2024())
        {
            QStringList m = m_generic.getModelDirectories();
            if (m.isEmpty()) { ui->pte_ModelDirectories->setPlainText("*"); }
            else
            {
                const QString ms = m.join("\n");
                ui->pte_ModelDirectories->setPlainText(ms);
            }

            QStringList e = m_generic.getModelExcludeDirectoryPatterns();
            if (e.isEmpty()) { ui->pte_ExcludeDirectories->setPlainText("PassiveAircraft\nSTUB\nZZZZ"); }
            else
            {
                const QString es = e.join("\n");
                ui->pte_ExcludeDirectories->setPlainText(es);
            }
        }
        else
        {
            const QStringList m = settings.getModelDirectoriesFromSimulatorDirectoryOrDefault();
            if (m.isEmpty()) { ui->pte_ModelDirectories->setPlaceholderText("Model directories"); }
            else
            {
                const QString ms = m.join("\n");
                ui->pte_ModelDirectories->setPlainText(ms);
            }

            const QStringList e = settings.getDefaultModelExcludePatterns();
            if (e.isEmpty()) { ui->pte_ExcludeDirectories->setPlaceholderText("Exclude directories"); }
            else
            {
                const QString es = e.join("\n");
                ui->pte_ExcludeDirectories->setPlainText(es);
            }
        }
    }

    QString CSettingsSimulatorBasicsComponent::getFileBrowserModelDirectory() const
    {
        const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
        const QStringList modelDirs(this->parseDirectories(ui->pte_ModelDirectories->toPlainText()));
        QString md = modelDirs.isEmpty() ? "" : modelDirs.first();
        if (md.isEmpty()) { md = m_settings.getFirstModelDirectoryOrDefault(simulator); }
        if (md.isEmpty()) { md = this->getFileBrowserSimulatorDirectory(); }
        return CFileUtils::normalizeFilePathToQtStandard(md);
    }

    QString CSettingsSimulatorBasicsComponent::getFileBrowserSimulatorDirectory() const
    {
        const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
        QString sd(ui->le_SimulatorDirectory->text().trimmed());
        if (sd.isEmpty()) { sd = m_settings.getSimulatorDirectoryOrDefault(simulator); }
        return CFileUtils::normalizeFilePathToQtStandard(sd);
    }
} // namespace swift::gui::components
