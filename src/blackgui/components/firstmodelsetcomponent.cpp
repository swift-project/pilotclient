/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/firstmodelsetcomponent.h"
#include "blackgui/components/dbownmodelsdialog.h"
#include "blackgui/components/dbownmodelscomponent.h"
#include "blackgui/components/dbownmodelsetdialog.h"
#include "blackgui/components/dbownmodelsetcomponent.h"
#include "ui_firstmodelsetcomponent.h"

#include "blackgui/views/distributorview.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

#include <QStringList>
#include <QFileDialog>
#include <QPointer>
#include <QMessageBox>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;

namespace BlackGui::Components
{
    const QStringList &CFirstModelSetComponent::getLogCategories()
    {
        static const QStringList cats { CLogCategories::modelGui() };
        return cats;
    }

    CFirstModelSetComponent::CFirstModelSetComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                        ui(new Ui::CFirstModelSetComponent)
    {
        ui->setupUi(this);
        ui->comp_Distributors->view()->setSelectionMode(QAbstractItemView::MultiSelection);
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->comp_SimulatorSelector->setRememberSelectionAndSetToLastSelection();

        // we use the powerful component to access own models
        m_modelsDialog.reset(new CDbOwnModelsDialog(this));
        m_modelSetDialog.reset(new CDbOwnModelSetDialog(this));

        this->onSimulatorChanged(ui->comp_SimulatorSelector->getValue());

        bool s = connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CFirstModelSetComponent::onSimulatorChanged);
        Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot connect selector signal");
        connect(&m_simulatorSettings, &CMultiSimulatorSettings::settingsChanged, this, &CFirstModelSetComponent::onSettingsChanged, Qt::QueuedConnection);
        Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot connect settings signal");
        connect(m_modelsDialog.data(), &CDbOwnModelsDialog::successfullyLoadedModels, this, &CFirstModelSetComponent::onModelsLoaded, Qt::QueuedConnection);
        Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot connect models signal");

        connect(ui->pb_ModelSet, &QPushButton::clicked, this, &CFirstModelSetComponent::openOwnModelSetDialog);
        connect(ui->pb_Models, &QPushButton::clicked, this, &CFirstModelSetComponent::openOwnModelsDialog);
        connect(ui->pb_ModelsTriggerReload, &QPushButton::clicked, this, &CFirstModelSetComponent::openOwnModelsDialog);
        connect(ui->pb_ChangeModelDir, &QPushButton::clicked, this, &CFirstModelSetComponent::changeModelDirectory);
        connect(ui->pb_ClearModelDir, &QPushButton::clicked, this, &CFirstModelSetComponent::changeModelDirectory);
        connect(ui->pb_CreateModelSet, &QPushButton::clicked, this, &CFirstModelSetComponent::createModelSet);
    }

    CFirstModelSetComponent::~CFirstModelSetComponent()
    {}

    void CFirstModelSetComponent::onSimulatorChanged(const CSimulatorInfo &simulator)
    {
        if (!simulator.isSingleSimulator())
        {
            //! \fixme KB 2019-01 reported by RR/crash dump sometimes happening and leading to ASSERT/CTD avoiding the "crash" for better infos
            if (CBuildConfig::isLocalDeveloperDebugBuild()) { BLACK_VERIFY_X(false, Q_FUNC_INFO, "Need single simulator"); }
            CLogMessage(this).error(u"Changing to non-single simulator %1 ignored") << simulator.toQString();
            return;
        }

        Q_ASSERT_X(m_modelsDialog, Q_FUNC_INFO, "No models dialog");
        m_modelsDialog->setSimulator(simulator);

        Q_ASSERT_X(m_modelSetDialog, Q_FUNC_INFO, "No model set dialog");
        m_modelSetDialog->setSimulator(simulator);

        // distributor component
        ui->comp_Distributors->filterBySimulator(simulator);

        const QStringList dirs = m_simulatorSettings.getModelDirectoriesOrDefault(simulator);
        ui->le_ModelDirectories->setText(dirs.join(", "));

        // kind of hack, but simplest solution
        // we us the loader of the components directly,
        // avoid to fully init a loader logic here
        static const QString modelsNo("No models so far");
        const int modelsCount = this->modelLoader()->getCachedModelsCount(simulator);
        if (modelsCount > 0)
        {
            static const QString modelsInfo("%1 included %2 DB key %3");
            const CAircraftModelList modelsInCache = this->modelLoader()->getCachedModels(simulator);
            const int modelsIncluded = modelsInCache.countByMode(CAircraftModel::Include);
            const int modelsDbKey = modelsInCache.countWithValidDbKey(true);
            ui->le_ModelsInfo->setText(modelsInfo.arg(this->modelLoader()->getCacheCountAndTimestamp(simulator)).arg(modelsIncluded).arg(modelsDbKey));
        }
        else
        {
            ui->le_ModelsInfo->setText(modelsNo);
        }

        ui->pb_CreateModelSet->setEnabled(modelsCount > 0);

        static const QString modelsSetNo("Model set is empty");
        const int modelsSetCount = m_modelSetDialog->modelSetComponent()->getModelSetCount();
        ui->le_ModelSetInfo->setText(modelsSetCount > 0 ? m_modelSetDialog->modelSetComponent()->getModelCacheCountAndTimestamp() : modelsSetNo);
    }

    void CFirstModelSetComponent::onSettingsChanged(const CSimulatorInfo &simulator)
    {
        const CSimulatorInfo currentSimulator = ui->comp_SimulatorSelector->getValue();
        if (simulator != currentSimulator) { return; } // ignore changes not for my selected simulator
        this->onSimulatorChanged(simulator);
    }

    void CFirstModelSetComponent::onModelsLoaded(const CSimulatorInfo &simulator, int count)
    {
        Q_UNUSED(count);
        const CSimulatorInfo currentSimulator = ui->comp_SimulatorSelector->getValue();
        if (simulator != currentSimulator) { return; } // ignore changes not for my selected simulator
        this->onSimulatorChanged(simulator);
    }

    void CFirstModelSetComponent::triggerSettingsChanged(const CSimulatorInfo &simulator)
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        QPointer<CFirstModelSetComponent> myself(this);
        QTimer::singleShot(0, this, [=] {
            if (!myself || !sGui || sGui->isShuttingDown()) { return; }
            myself->onSettingsChanged(simulator);
        });
    }

    const CDbOwnModelsComponent *CFirstModelSetComponent::modelsComponent() const
    {
        Q_ASSERT_X(m_modelsDialog, Q_FUNC_INFO, "No models dialog");
        Q_ASSERT_X(m_modelsDialog->modelsComponent(), Q_FUNC_INFO, "No models component");
        return m_modelsDialog->modelsComponent();
    }

    const CDbOwnModelSetComponent *CFirstModelSetComponent::modelSetComponent() const
    {
        Q_ASSERT_X(m_modelSetDialog, Q_FUNC_INFO, "No model set dialog");
        Q_ASSERT_X(m_modelSetDialog->modelSetComponent(), Q_FUNC_INFO, "No model set component");
        return m_modelSetDialog->modelSetComponent();
    }

    IAircraftModelLoader *CFirstModelSetComponent::modelLoader() const
    {
        Q_ASSERT_X(m_modelsDialog->modelsComponent()->modelLoader(), Q_FUNC_INFO, "No model loader");
        return m_modelsDialog->modelsComponent()->modelLoader();
    }

    void CFirstModelSetComponent::openOwnModelsDialog()
    {
        if (!m_modelsDialog) { return; }
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        const bool reload = (QObject::sender() == ui->pb_ModelsTriggerReload);

        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        m_modelsDialog->setSimulator(simulator);

        if (reload)
        {
            if (!sGui->getWebDataServices()->hasDbModelData())
            {
                const QMessageBox::StandardButton reply = QMessageBox::warning(this->mainWindow(), "DB data", "No DB data, models cannot be consolidated. Load anyway?", QMessageBox::Yes | QMessageBox::No);
                if (reply != QMessageBox::Yes) { return; }
            }

            bool loadOnlyIfNotEmpty = true;
            if (m_modelsDialog->getOwnModelsCount() > 0)
            {
                const QMessageBox::StandardButton reply = QMessageBox::warning(this->mainWindow(), "Model loading", "Reload the models?\nThe existing cache data will we overridden.", QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) { loadOnlyIfNotEmpty = false; }
            }
            m_modelsDialog->requestModelsInBackground(simulator, loadOnlyIfNotEmpty);
        }
        m_modelsDialog->exec();

        // force UI update
        this->triggerSettingsChanged(simulator);
    }

    void CFirstModelSetComponent::openOwnModelSetDialog()
    {
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        m_modelSetDialog->setSimulator(simulator);
        m_modelSetDialog->enableButtons(false, false);
        m_modelSetDialog->exec();

        // force UI update
        this->triggerSettingsChanged(simulator);
    }

    void CFirstModelSetComponent::changeModelDirectory()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        CSpecializedSimulatorSettings settings = m_simulatorSettings.getSpecializedSettings(simulator);
        const bool clear = (QObject::sender() == ui->pb_ClearModelDir);

        if (clear)
        {
            settings.clearModelDirectories();
        }
        else
        {
            const QString dirOld = settings.getFirstModelDirectoryOrDefault();
            const QString newDir = QFileDialog::getExistingDirectory(this->mainWindow(), tr("Open model directory"), dirOld, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            if (newDir.isEmpty() || CDirectoryUtils::isSameExistingDirectory(dirOld, newDir)) { return; }
            settings.addModelDirectory(newDir);
        }

        const CStatusMessage msg = m_simulatorSettings.setAndSaveSettings(settings, simulator);
        if (msg.isSuccess())
        {
            this->triggerSettingsChanged(simulator);
        }
        else
        {
            this->showOverlayMessage(msg, 4000);
        }
    }

    void CFirstModelSetComponent::createModelSet()
    {
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        const int modelsCount = this->modelLoader()->getCachedModelsCount(simulator);
        if (modelsCount < 1)
        {
            static const CStatusMessage msg = CStatusMessage(this).validationError(u"No models indexed so far. Try 'reload'!");
            this->showOverlayMessage(msg, 4000);
            return;
        }

        bool useAllModels = false;
        if (!ui->comp_Distributors->hasSelectedDistributors())
        {
            const QMessageBox::StandardButton reply = QMessageBox::question(this->mainWindow(), "Models", "No distributors selected, use all models?", QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes)
            {
                useAllModels = true;
            }
            else
            {
                static const CStatusMessage msg = CStatusMessage(this).validationError(u"No distributors selected");
                this->showOverlayMessage(msg, 4000);
                return;
            }
        }
        CAircraftModelList modelsForSet = this->modelLoader()->getCachedModels(simulator);
        if (!useAllModels)
        {
            const CDistributorList distributors = ui->comp_Distributors->getSelectedDistributors();
            modelsForSet = modelsForSet.findByDistributors(distributors);
        }

        if (ui->cb_DbDataOnly->isChecked()) { modelsForSet.removeObjectsWithoutDbKey(); }
        if (modelsForSet.isEmpty())
        {
            this->showOverlayHTMLMessage("Selection yielded no result!");
            return;
        }

        // just in case, paranoia
        if (!m_modelSetDialog || !m_modelSetDialog->modelSetComponent())
        {
            this->showOverlayHTMLMessage("No model set dialog, cannot continue");
            return;
        }

        const int modelsSetCount = m_modelSetDialog->modelSetComponent()->getModelSetCount();
        if (modelsSetCount > 0)
        {
            QMessageBox::StandardButton override = QMessageBox::question(this, "Override", "Override existing model set?", QMessageBox::Yes | QMessageBox::No);
            if (override != QMessageBox::Yes) { return; }
        }

        m_modelSetDialog->modelSetComponent()->setModelSet(modelsForSet, simulator);
        ui->pb_ModelSet->click();
    }

    QWidget *CFirstModelSetComponent::mainWindow()
    {
        QWidget *pw = sGui->mainApplicationWidget();
        return pw ? pw : this;
    }

    bool CFirstModelSetWizardPage::validatePage()
    {
        return true;
    }
} // ns
