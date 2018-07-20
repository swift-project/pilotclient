/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "firstmodelsetcomponent.h"
#include "ui_firstmodelsetcomponent.h"
#include "blackgui/views/distributorview.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/directoryutils.h"
#include "dbownmodelsdialog.h"
#include "dbownmodelscomponent.h"
#include "dbownmodelsetdialog.h"
#include "dbownmodelsetcomponent.h"

#include <QStringList>
#include <QFileDialog>
#include <QPointer>
#include <QMessageBox>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;

namespace BlackGui
{
    namespace Components
    {
        const CLogCategoryList &CFirstModelSetComponent::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::modelGui() };
            return cats;
        }

        CFirstModelSetComponent::CFirstModelSetComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
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
        { }

        void CFirstModelSetComponent::onSimulatorChanged(const CSimulatorInfo &simulator)
        {
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
            const int modelsCount = this->modelLoader()->getAircraftModelsCount();
            ui->le_ModelsInfo->setText(modelsCount > 0 ? this->modelLoader()->getModelCacheCountAndTimestamp() : modelsNo);
            ui->pb_CreateModelSet->setEnabled(modelsCount > 0);

            static const QString modelsSetNo("Model set is empty");
            const int modelsSetCount = this->modelSetLoader().getAircraftModelsCount();
            ui->le_ModelSetInfo->setText(modelsSetCount > 0 ? this->modelSetLoader().getModelCacheCountAndTimestamp(simulator) : modelsSetNo);
        }

        void CFirstModelSetComponent::onSettingsChanged(const CSimulatorInfo &simulator)
        {
            const CSimulatorInfo currentSimulator = ui->comp_SimulatorSelector->getValue();
            if (simulator != currentSimulator) { return; } // ignore changes not for my selected simulator
            this->onSimulatorChanged(simulator);
        }

        void CFirstModelSetComponent::onModelsLoaded(const CSimulatorInfo &simulator)
        {
            const CSimulatorInfo currentSimulator = ui->comp_SimulatorSelector->getValue();
            if (simulator != currentSimulator) { return; } // ignore changes not for my selected simulator
            this->onSimulatorChanged(simulator);
        }

        void CFirstModelSetComponent::triggerSettingsChanged(const CSimulatorInfo &simulator)
        {
            if (!sGui || sGui->isShuttingDown()) { return; }
            QPointer<CFirstModelSetComponent> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
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

        const CAircraftModelSetLoader &CFirstModelSetComponent::modelSetLoader() const
        {
            return this->modelSetComponent()->modelSetLoader();
        }

        void CFirstModelSetComponent::openOwnModelsDialog()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
            const bool reload = (QObject::sender() == ui->pb_ModelsTriggerReload);

            if (reload)
            {
                if (!sGui->getWebDataServices()->hasDbModelData())
                {
                    const QMessageBox::StandardButton reply = QMessageBox::warning(this->mainWindow(), "DB data", "No DB data, models cannot be consolidated. Load anyway?", QMessageBox::Yes | QMessageBox::No);
                    if (reply != QMessageBox::Yes) { return; }
                }
                const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
                m_modelsDialog->requestModelsInBackground(simulator, true);
            }

            const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
            m_modelsDialog->setSimulator(simulator);
            m_modelsDialog->exec();

            // force UI update
            this->triggerSettingsChanged(simulator);
        }

        void CFirstModelSetComponent::openOwnModelSetDialog()
        {
            const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
            m_modelSetDialog->setSimulator(simulator);
            m_modelSetDialog->enableButtons(false);
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
            const int modelsCount = this->modelLoader()->getAircraftModelsCount();
            if (modelsCount < 1)
            {
                static const CStatusMessage msg = CStatusMessage(this).validationError("No models indexed so far. Try 'reload'!");
                this->showOverlayMessage(msg, 4000);
                return;
            }

            bool useAllModels = false;
            if (!ui->comp_Distributors->hasSelectedDistributors())
            {
                const QMessageBox::StandardButton reply = QMessageBox::question(this->mainWindow(), "Models", "No distributors selected, use all model?", QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes)
                {
                    useAllModels = true;
                }
                else
                {
                    static const CStatusMessage msg = CStatusMessage(this).validationError("No distributors selected");
                    this->showOverlayMessage(msg, 4000);
                    return;
                }
            }
            CAircraftModelList modelsForSet = this->modelLoader()->getAircraftModels();
            if (!useAllModels)
            {
                const CDistributorList distributors = ui->comp_Distributors->getSelectedDistributors();
                modelsForSet = modelsForSet.findByDistributors(distributors);
            }

            const CSimulatorInfo sim = ui->comp_SimulatorSelector->getValue();
            m_modelSetDialog->modelSetComponent()->setModelSet(modelsForSet, sim);
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
} // ns
