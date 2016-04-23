/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbownmodelsetcomponent.h"
#include "blackgui/models/aircrafticaolistmodel.h"
#include "blackgui/menus/aircraftmodelmenus.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/logmessage.h"
#include "dbmappingcomponent.h"
#include "dbownmodelsetdialog.h"
#include "ui_dbownmodelsetcomponent.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Models;
using namespace BlackGui::Menus;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        CDbOwnModelSetComponent::CDbOwnModelSetComponent(QWidget *parent) :
            QFrame(parent),
            CDbMappingComponentAware(parent),
            ui(new Ui::CDbOwnModelSetComponent)
        {
            ui->setupUi(this);
            ui->tvp_OwnModelSet->setAircraftModelMode(CAircraftModelListModel::OwnModelSet);
            ui->tvp_OwnModelSet->menuRemoveItems(CAircraftModelView::MenuDisplayAutomaticallyAndRefresh | CAircraftModelView::MenuStashing | CAircraftModelView::MenuBackend | CAircraftModelView::MenuRefresh);
            ui->tvp_OwnModelSet->menuAddItems(CAircraftModelView::MenuRemoveSelectedRows | CAircraftModelView::MenuClear);
            ui->tvp_OwnModelSet->addFilterDialog();
            ui->tvp_OwnModelSet->setCustomMenu(new CLoadModelsMenu(this));
            ui->tvp_OwnModelSet->setJsonLoad(CAircraftModelView::AllowOnlySingleSimulator | CAircraftModelView::ReduceToOneSimulator);
            ui->tvp_OwnModelSet->setCustomMenu(new CMergeWithDbDataMenu(ui->tvp_OwnModelSet, this, true));
            ui->tvp_OwnModelSet->menuAddItems(CAircraftModelView::MenuOrderable);
            ui->tvp_OwnModelSet->initAsOrderable();

            connect(ui->pb_CreateNewSet, &QPushButton::clicked, this, &CDbOwnModelSetComponent::ps_buttonClicked);
            connect(ui->pb_LoadExistingSet, &QPushButton::clicked, this, &CDbOwnModelSetComponent::ps_buttonClicked);
            connect(ui->pb_SaveAsSetForSimulator, &QPushButton::clicked, this, &CDbOwnModelSetComponent::ps_buttonClicked);
            connect(&this->m_modelSetLoader, &CAircraftModelSetLoader::simulatorChanged, this, &CDbOwnModelSetComponent::ps_onSimulatorChanged);
            connect(ui->tvp_OwnModelSet, &CAircraftModelView::rowCountChanged, this, &CDbOwnModelSetComponent::ps_onRowCountChanged);
            connect(ui->tvp_OwnModelSet, &CAircraftModelView::jsonModelsForSimulatorLoaded, this, &CDbOwnModelSetComponent::ps_onJsonDataLoaded);

            this->ps_onRowCountChanged(ui->tvp_OwnModelSet->rowCount(), ui->tvp_OwnModelSet->hasFilter());
        }

        CDbOwnModelSetComponent::~CDbOwnModelSetComponent()
        {
            // void
        }

        Views::CAircraftModelView *CDbOwnModelSetComponent::view() const
        {
            return ui->tvp_OwnModelSet;
        }

        void CDbOwnModelSetComponent::setModelSet(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            this->setSimulator(simulator);
            if (models.isEmpty())
            {
                this->ui->tvp_OwnModelSet->clear();
                return;
            }

            // unempty set, consolidate
            CAircraftModelList cleanModelList(models.matchesSimulator(simulator)); // remove those not matching the simulator
            const int diff = models.size() - cleanModelList.size();
            if (diff > 0)
            {
                CLogMessage(this).warning("Removed %1 models from set because not matching %2") << diff << simulator.toQString(true);
            }
            this->ui->tvp_OwnModelSet->updateContainerMaybeAsync(cleanModelList);
        }

        void CDbOwnModelSetComponent::replaceOrAddModelSet(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            if (models.isEmpty()) { return; }
            CAircraftModelList cleanModelList(models.matchesSimulator(simulator)); // remove those not matching the simulator
            const int diff = models.size() - cleanModelList.size();
            if (diff > 0)
            {
                CLogMessage(this).warning("Removed %1 models from set because not matching %2") << diff << simulator.toQString(true);
            }
            if (cleanModelList.isEmpty()) { return; }
            CAircraftModelList updatedModels(this->ui->tvp_OwnModelSet->container());
            updatedModels.replaceOrAddModelsWithString(cleanModelList, Qt::CaseInsensitive);
            this->ui->tvp_OwnModelSet->updateContainerMaybeAsync(updatedModels);
        }

        const CAircraftModelList &CDbOwnModelSetComponent::getModelSet() const
        {
            return ui->tvp_OwnModelSet->container();
        }

        CStatusMessage CDbOwnModelSetComponent::addToModelSet(const CAircraftModel &model, const CSimulatorInfo &simulator)
        {
            return this->addToModelSet(CAircraftModelList({model}), simulator);
        }

        CStatusMessage CDbOwnModelSetComponent::addToModelSet(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data", true); }
            if (!this->getModelSetSimulator().isSingleSimulator())
            {
                // no sim yet, we set it
                this->setSimulator(simulator);
            }
            if (simulator != this->getModelSetSimulator())
            {
                // only currently selected sim allowed
                return CStatusMessage(this, CStatusMessage::SeverityError,
                                      "Cannot add data for " + simulator.toQString(true) + " to " + this->getModelSetSimulator().toQString(true), true);
            }
            CAircraftModelList updateModels(this->getModelSet());
            int d = updateModels.replaceOrAddModelsWithString(models, Qt::CaseInsensitive);
            if (d > 0)
            {
                this->ui->tvp_OwnModelSet->updateContainerMaybeAsync(updateModels);
                return CStatusMessage(this, CStatusMessage::SeverityInfo, "Modified " + QString::number(d) + " entries in model set " + this->getModelSetSimulator().toQString(true), true);
            }
            else
            {
                return CStatusMessage(this, CStatusMessage::SeverityInfo, "No data modified in model set", true);
            }
        }

        void CDbOwnModelSetComponent::setMappingComponent(CDbMappingComponent *component)
        {
            CDbMappingComponentAware::setMappingComponent(component);
            if (component)
            {
                connect(this->getMappingComponent(), &CDbMappingComponent::tabIndexChanged, this, &CDbOwnModelSetComponent::ps_tabIndexChanged);
            }
        }

        void CDbOwnModelSetComponent::ps_tabIndexChanged(int index)
        {
            Q_UNUSED(index);
        }

        void CDbOwnModelSetComponent::ps_buttonClicked()
        {
            const QObject *sender = QObject::sender();
            if (sender == ui->pb_CreateNewSet)
            {
                if (!this->m_modelSetDialog)
                {
                    this->m_modelSetDialog.reset(new CDbOwnModelSetDialog(this));
                    this->m_modelSetDialog->setMappingComponent(this->getMappingComponent());
                }

                if (this->getMappingComponent()->getOwnModelsCount() > 0)
                {
                    this->m_modelSetDialog->setModal(true);
                    this->m_modelSetDialog->reloadData();
                    QDialog::DialogCode rc = static_cast<QDialog::DialogCode>(this->m_modelSetDialog->exec());
                    if (rc == QDialog::Accepted)
                    {
                        this->setModelSet(this->m_modelSetDialog->getModelSet(), this->m_modelSetDialog->getSimulatorInfo());
                    }
                }
                else
                {
                    static const CStatusMessage m(CStatusMessage(this, CStatusMessage::SeverityError, "No model data"));
                    this->getMappingComponent()->showOverlayMessage(m);
                }
            }
            else if (sender == ui->pb_LoadExistingSet)
            {
                this->ui->tvp_OwnModelSet->showFileLoadDialog();
            }
            else if (sender == ui->pb_SaveAsSetForSimulator)
            {
                const CAircraftModelList ml(ui->tvp_OwnModelSet->container());
                if (!ml.isEmpty())
                {
                    const CStatusMessage m = this->m_modelSetLoader.setCachedModels(ml);
                    CLogMessage::preformatted(m);
                }
            }
        }

        void CDbOwnModelSetComponent::ps_changeSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            this->m_modelSetLoader.changeSimulator(simulator);
            this->setSimulator(simulator);
        }

        void CDbOwnModelSetComponent::ps_onSimulatorChanged(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            const CAircraftModelList models(this->m_modelSetLoader.getAircraftModels());
            this->setSimulator(simulator);
            ui->tvp_OwnModelSet->updateContainerMaybeAsync(models);
        }

        void CDbOwnModelSetComponent::ps_onRowCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int realUnfilteredCount = ui->tvp_OwnModelSet->container().size();
            bool canSave = this->m_simulator.isSingleSimulator() && (realUnfilteredCount > 0);
            this->ui->pb_SaveAsSetForSimulator->setEnabled(canSave);
            if (canSave)
            {
                this->setSaveFileName(this->m_simulator);
                ui->pb_SaveAsSetForSimulator->setText("save for " + this->m_simulator.toQString(true));
            }
            else
            {
                ui->pb_SaveAsSetForSimulator->setText("save");
            }
        }

        void CDbOwnModelSetComponent::ps_onJsonDataLoaded(const CSimulatorInfo &simulator)
        {
            if (simulator.isSingleSimulator())
            {
                this->setSimulator(simulator);
            }
        }

        void CDbOwnModelSetComponent::ps_preferencesChanged()
        {
            const CDistributorListPreferences preferences = this->m_distributorPreferences.get();
            const CSimulatorInfo simuulator = preferences.getLastUpdatedSimulator();
            if (simuulator.isSingleSimulator())
            {
                this->updateDistributorOrder(simuulator);
            }
        }

        void CDbOwnModelSetComponent::setSaveFileName(const CSimulatorInfo &sim)
        {
            Q_ASSERT_X(sim.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            const QString name("modelset" + sim.toQString(true));
            this->ui->tvp_OwnModelSet->setSaveFileName(name);
        }

        void CDbOwnModelSetComponent::setSimulator(const CSimulatorInfo &sim)
        {
            this->m_simulator = sim;
            this->ui->le_Simulator->setText(sim.toQString(true));
        }

        void CDbOwnModelSetComponent::updateDistributorOrder(const CSimulatorInfo &simulator)
        {
            CAircraftModelList modelSet = this->m_modelSetLoader.getAircraftModels(simulator);
            if (modelSet.isEmpty()) { return; }
            const CDistributorListPreferences preferences = this->m_distributorPreferences.get();
            const CDistributorList distributors = preferences.getDistributors(simulator);
            if (distributors.isEmpty()) { return; }
            modelSet.updateDistributorOrder(distributors);
            this->m_modelSetLoader.setModels(modelSet, simulator);

            // display?
            const CSimulatorInfo currentSimulator(this->getModelSetSimulator());
            if (simulator == currentSimulator)
            {
                ui->tvp_OwnModelSet->updateContainerAsync(modelSet);
            }
        }

        void CDbOwnModelSetComponent::CLoadModelsMenu::customMenu(QMenu &menu) const
        {
            const CSimulatorInfo sims = CSimulatorInfo::getLocallyInstalledSimulators();
            bool noSims = sims.isNoSimulator() || sims.isUnspecified();
            if (!noSims)
            {
                this->addSeparator(menu);
                QMenu *load = menu.addMenu(CIcons::appModels16(), "Model set");
                CDbOwnModelSetComponent *ownModelSetComp = qobject_cast<CDbOwnModelSetComponent *>(this->parent());
                Q_ASSERT_X(ownModelSetComp, Q_FUNC_INFO, "Cannot access parent");
                if (sims.fsx())
                {
                    load->addAction(CIcons::appModels16(), "FSX models", ownModelSetComp, [ownModelSetComp]()
                    {
                        ownModelSetComp->ps_changeSimulator(CSimulatorInfo(CSimulatorInfo::FSX));
                    });
                }
                if (sims.p3d())
                {
                    load->addAction(CIcons::appModels16(), "P3D models", ownModelSetComp, [ownModelSetComp]()
                    {
                        ownModelSetComp->ps_changeSimulator(CSimulatorInfo(CSimulatorInfo::P3D));
                    });
                }
                if (sims.fs9())
                {
                    load->addAction(CIcons::appModels16(), "FS9 models", ownModelSetComp, [ownModelSetComp]()
                    {
                        ownModelSetComp->ps_changeSimulator(CSimulatorInfo(CSimulatorInfo::FS9));
                    });
                }
                if (sims.xplane())
                {
                    load->addAction(CIcons::appModels16(), "XP models", ownModelSetComp, [ownModelSetComp]()
                    {
                        ownModelSetComp->ps_changeSimulator(CSimulatorInfo(CSimulatorInfo::XPLANE));
                    });
                }

                QMenu *emptySetMenu = load->addMenu("New empty set");
                if (sims.fsx())
                {
                    emptySetMenu->addAction(CIcons::appModels16(), "FSX models", ownModelSetComp, [ownModelSetComp]()
                    {
                        ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::FSX));
                    });
                }
                if (sims.p3d())
                {
                    emptySetMenu->addAction(CIcons::appModels16(), "P3D models", ownModelSetComp, [ownModelSetComp]()
                    {
                        ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::P3D));
                    });
                }
                if (sims.fs9())
                {
                    emptySetMenu->addAction(CIcons::appModels16(), "FS9 models", ownModelSetComp, [ownModelSetComp]()
                    {
                        ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::FS9));
                    });
                }
                if (sims.xplane())
                {
                    emptySetMenu->addAction(CIcons::appModels16(), "XP models", ownModelSetComp, [ownModelSetComp]()
                    {
                        ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::XPLANE));
                    });
                }
            }
            this->nestedCustomMenu(menu);
        }
    } // ns
} // ns
