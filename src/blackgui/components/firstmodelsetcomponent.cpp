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
#include "dbownmodelsdialog.h"
#include "dbownmodelscomponent.h"
#include "dbownmodelsetdialog.h"
#include "dbownmodelsetcomponent.h"
#include <QStringList>

using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Settings;

namespace BlackGui
{
    namespace Components
    {
        CFirstModelSetComponent::CFirstModelSetComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CFirstModelSetComponent)
        {
            ui->setupUi(this);
            ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
            ui->comp_SimulatorSelector->setRememberSelection(true);

            // we use the powerful component to access own models
            m_modelsDialog.reset(new CDbOwnModelsDialog(this));
            m_modelSetDialog.reset(new CDbOwnModelSetDialog(this));

            this->onSimulatorChanged(ui->comp_SimulatorSelector->getValue());

            connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CFirstModelSetComponent::onSimulatorChanged);
            connect(ui->pb_ModelSet, &QPushButton::clicked, this, &CFirstModelSetComponent::openOwnModelSetDialog);
            connect(ui->pb_Models, &QPushButton::clicked, this, &CFirstModelSetComponent::openOwnModelsDialog);
            connect(ui->pb_ModelsTriggerReload, &QPushButton::clicked, this, &CFirstModelSetComponent::openOwnModelsDialog);
        }

        CFirstModelSetComponent::~CFirstModelSetComponent()
        { }

        void CFirstModelSetComponent::onSimulatorChanged(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(m_modelsDialog, Q_FUNC_INFO, "No models dialog");
            m_modelsDialog->setSimulator(simulator);

            Q_ASSERT_X(m_modelSetDialog, Q_FUNC_INFO, "No model set dialog");
            m_modelSetDialog->setSimulator(simulator);

            ui->comp_Distributors->filterBySimulator(simulator);

            // kind of hack, but simplest solution
            // we us the loader of the components directly,
            // avoid to fully init a loader logic here
            const QStringList dirs = this->simulatorSettings().getModelDirectoriesOrDefault(simulator);
            ui->le_ModelDirectories->setText(dirs.join(", "));

            static const QString modelInfo("Models already indexed: %1");
            static const QString modelsNo("No models so far");
            const int models = this->modelLoader()->getAircraftModelsCount();
            ui->le_ModelsInfo->setText(models > 0 ? modelInfo.arg(models) : modelsNo);

            static const QString modelSetInfo("Models in set: %1");
            static const QString modelsSetNo("Model set is empty");
            const int modelSet = this->modelSetLoader().getAircraftModelsCount();
            ui->le_ModelSetInfo->setText(models > 0 ? modelSetInfo.arg(modelSet) : modelsSetNo);
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

        BlackMisc::Simulation::IAircraftModelLoader *CFirstModelSetComponent::modelLoader() const
        {
            Q_ASSERT_X(m_modelsDialog->modelsComponent()->modelLoader(), Q_FUNC_INFO, "No model loader");
            return m_modelsDialog->modelsComponent()->modelLoader();
        }

        const CAircraftModelSetLoader &CFirstModelSetComponent::modelSetLoader() const
        {
            return this->modelSetComponent()->modelSetLoader();
        }

        const CMultiSimulatorSettings &CFirstModelSetComponent::simulatorSettings() const
        {
            return this->modelLoader()->multiSimulatorSettings();
        }

        void CFirstModelSetComponent::openOwnModelsDialog()
        {
            m_modelsDialog->setSimulator(ui->comp_SimulatorSelector->getValue());
            m_modelsDialog->show();
            bool const reload = QObject::sender() == ui->pb_ModelsTriggerReload;
            if (reload) { m_modelsDialog->requestModelsInBackground(ui->comp_SimulatorSelector->getValue(), true); }
        }

        void CFirstModelSetComponent::openOwnModelSetDialog()
        {
            m_modelSetDialog->setSimulator(ui->comp_SimulatorSelector->getValue());
            m_modelSetDialog->show();
        }

        bool CFirstModelSetWizardPage::validatePage()
        {
            return true;
        }
    } // ns
} // ns
