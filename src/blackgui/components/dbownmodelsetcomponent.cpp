/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/guiapplication.h"
#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/dbownmodelsetcomponent.h"
#include "blackgui/components/dbownmodelsetformdialog.h"
#include "blackgui/components/firstmodelsetdialog.h"
#include "blackgui/components/copymodelsfromotherswiftversionsdialog.h"
#include "blackgui/menus/aircraftmodelmenus.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/simulation/aircraftmodelutils.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/distributorlistpreferences.h"
#include "blackmisc/compare.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/orderable.h"
#include "ui_dbownmodelsetcomponent.h"

#include <QAction>
#include <QDialog>
#include <QFlags>
#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QWidget>
#include <Qt>
#include <QtGlobal>
#include <QDesktopServices>

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
            ui->tvp_OwnModelSet->menuAddItems(CAircraftModelView::MenuStashing);
            ui->tvp_OwnModelSet->menuRemoveItems(CAircraftModelView::MenuDisplayAutomaticallyAndRefresh | CAircraftModelView::MenuBackend | CAircraftModelView::MenuRefresh);
            ui->tvp_OwnModelSet->menuAddItems(CAircraftModelView::MenuRemoveSelectedRows | CAircraftModelView::MenuClear);
            ui->tvp_OwnModelSet->menuAddItems(CAircraftModelView::MenuRemoveSelectedRows | CAircraftModelView::MenuMaterializeFilter);
            ui->tvp_OwnModelSet->addFilterDialog();
            ui->tvp_OwnModelSet->setCustomMenu(new CLoadModelSetMenu(this));
            ui->tvp_OwnModelSet->setCustomMenu(new CConsolidateWithDbDataMenu(ui->tvp_OwnModelSet, this, true));
            ui->tvp_OwnModelSet->setCustomMenu(new CConsolidateWithSimulatorModels(ui->tvp_OwnModelSet, this, false));
            ui->tvp_OwnModelSet->menuAddItems(CAircraftModelView::MenuLoadAndSave);
            ui->tvp_OwnModelSet->setSorting(CAircraftModel::IndexOrderString);
            ui->tvp_OwnModelSet->initAsOrderable();
            ui->tvp_OwnModelSet->setSimulatorForLoading(ui->comp_SimulatorSelector->getValue());
            ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);

            //! \fixme maybe it would be better to set those in stylesheet file
            ui->pb_SaveAsSetForSimulator->setStyleSheet("padding-left: 3px; padding-right: 3px;");

            connect(ui->pb_CreateNewSet, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked);
            connect(ui->pb_LoadExistingSet, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked);
            connect(ui->pb_SaveAsSetForSimulator, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked);
            connect(ui->pb_ShowMatrix, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked);
            connect(ui->pb_CopyFromAnotherSwift, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked);
            connect(ui->pb_FirstSet, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked);
            connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CDbOwnModelSetComponent::setSimulator, Qt::QueuedConnection);
            connect(&m_modelSetLoader, &CAircraftModelSetLoader::simulatorChanged, this, &CDbOwnModelSetComponent::changeSimulator, Qt::QueuedConnection);
            connect(ui->tvp_OwnModelSet, &CAircraftModelView::modelDataChanged, this, &CDbOwnModelSetComponent::onRowCountChanged);
            connect(ui->tvp_OwnModelSet, &CAircraftModelView::modelChanged, this, &CDbOwnModelSetComponent::viewModelChanged);
            connect(ui->tvp_OwnModelSet, &CAircraftModelView::jsonModelsForSimulatorLoaded, this, &CDbOwnModelSetComponent::onJsonDataLoaded);

            const CSimulatorInfo simulator = m_modelSetLoader.getSimulator();
            if (simulator.isSingleSimulator())
            {
                ui->comp_SimulatorSelector->setValue(simulator);
                ui->le_Simulator->setText(simulator.toQString(true));
                const QPointer<CDbOwnModelSetComponent> myself(this);
                QTimer::singleShot(500, [ = ]()
                {
                    if (myself.isNull() || !sApp || sApp->isShuttingDown()) { return; }
                    this->updateViewToCurrentModels();
                });
            }
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
                ui->tvp_OwnModelSet->clear();
                return;
            }

            // unempty set, consolidate
            CAircraftModelList cleanModelList(models.matchesSimulator(simulator)); // remove those not matching the simulator
            const int diff = models.size() - cleanModelList.size();
            if (diff > 0)
            {
                CLogMessage(this).warning("Removed %1 models from set because not matching %2") << diff << simulator.toQString(true);
            }
            cleanModelList.resetOrder();
            ui->tvp_OwnModelSet->updateContainerMaybeAsync(cleanModelList);
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
            CAircraftModelList updatedModels(ui->tvp_OwnModelSet->container());
            updatedModels.replaceOrAddModelsWithString(cleanModelList, Qt::CaseInsensitive);
            updatedModels.resetOrder();
            ui->tvp_OwnModelSet->updateContainerMaybeAsync(updatedModels);
        }

        const CAircraftModelList &CDbOwnModelSetComponent::getModelSetFromView() const
        {
            return ui->tvp_OwnModelSet->container();
        }

        int CDbOwnModelSetComponent::getModelSetCountFromView() const
        {
            return ui->tvp_OwnModelSet->container().size();
        }

        CSimulatorInfo CDbOwnModelSetComponent::getModelSetSimulator() const
        {
            return m_modelSetLoader.getSimulator();
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

            const bool allowExcludedModels = m_modelSettings.get().getAllowExcludedModels();
            CAircraftModelList updateModels(this->getModelSetFromView());
            int d = updateModels.replaceOrAddModelsWithString(models, Qt::CaseInsensitive);
            if (d > 0)
            {
                if (!allowExcludedModels) { updateModels.removeIfExcluded(); }
                updateModels.resetOrder();
                ui->tvp_OwnModelSet->updateContainerMaybeAsync(updateModels);
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
                connect(this->getMappingComponent(), &CDbMappingComponent::tabIndexChanged, this, &CDbOwnModelSetComponent::tabIndexChanged);
            }
        }

        void CDbOwnModelSetComponent::tabIndexChanged(int index)
        {
            Q_UNUSED(index);
        }

        void CDbOwnModelSetComponent::buttonClicked()
        {
            const QObject *sender = QObject::sender();
            if (sender == ui->pb_CreateNewSet)
            {
                this->createNewSet();
                return;
            }

            if (sender == ui->pb_LoadExistingSet)
            {
                ui->tvp_OwnModelSet->showFileLoadDialog();
                return;
            }

            if (sender == ui->pb_SaveAsSetForSimulator)
            {
                const CAircraftModelList ml(ui->tvp_OwnModelSet->container());
                if (!ml.isEmpty())
                {
                    const CStatusMessage m = m_modelSetLoader.setCachedModels(ml);
                    CLogMessage::preformatted(m);
                }
                return;
            }

            if (sender == ui->pb_ShowMatrix)
            {
                this->showAirlineAircraftMatrix();
                return;
            }

            if (sender == ui->pb_FirstSet)
            {
                this->firstSet();
                return;
            }

            if (sender == ui->pb_CopyFromAnotherSwift)
            {
                this->copyFromAnotherSwift();
                return;
            }
        }

        void CDbOwnModelSetComponent::onRowCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            const int realUnfilteredCount = ui->tvp_OwnModelSet->container().size();
            const bool canSave = this->getModelSetSimulator().isSingleSimulator() && (realUnfilteredCount > 0);
            ui->pb_SaveAsSetForSimulator->setEnabled(canSave);
            if (canSave)
            {
                this->setSaveFileName(this->getModelSetSimulator());
                ui->pb_SaveAsSetForSimulator->setText("save for " + this->getModelSetSimulator().toQString(true));
            }
            else
            {
                ui->pb_SaveAsSetForSimulator->setText("save");
            }
        }

        void CDbOwnModelSetComponent::onJsonDataLoaded(const CSimulatorInfo &simulator)
        {
            if (simulator.isSingleSimulator())
            {
                this->setSimulator(simulator);
            }
        }

        void CDbOwnModelSetComponent::distributorPreferencesChanged()
        {
            const CDistributorListPreferences preferences = m_distributorPreferences.getThreadLocal();
            const CSimulatorInfo simuulator = preferences.getLastUpdatedSimulator();
            if (simuulator.isSingleSimulator())
            {
                this->updateDistributorOrder(simuulator);
            }
        }

        void CDbOwnModelSetComponent::viewModelChanged()
        {
            const bool hasData = ui->tvp_OwnModelSet->rowCount() > 0;
            ui->pb_SaveAsSetForSimulator->setEnabled(hasData);
        }

        void CDbOwnModelSetComponent::setSaveFileName(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            const QString name("modelset" + simulator.toQString(true));
            ui->tvp_OwnModelSet->setSaveFileName(name);
        }

        void CDbOwnModelSetComponent::updateViewToCurrentModels()
        {
            const CAircraftModelList models(m_modelSetLoader.getAircraftModels());
            ui->tvp_OwnModelSet->updateContainerMaybeAsync(models);
        }

        void CDbOwnModelSetComponent::createNewSet()
        {
            // make sure both tabs display the same simulator
            // since we use the componet also in the launcher wizard, mc might not be existing
            const CSimulatorInfo simulator(this->getModelSetSimulator());
            CDbMappingComponent *mc = this->getMappingComponent();
            if (mc) { mc->setOwnModelsSimulator(simulator); }

            if (!m_modelSetFormDialog)
            {
                m_modelSetFormDialog.reset(new CDbOwnModelSetFormDialog(this));
                m_modelSetFormDialog->setMappingComponent(this->getMappingComponent());
            }

            if (this->getMappingComponent()->getOwnModelsCount() > 0)
            {
                m_modelSetFormDialog->setModal(true);
                m_modelSetFormDialog->reloadData();
                const QDialog::DialogCode rc = static_cast<QDialog::DialogCode>(m_modelSetFormDialog->exec());
                if (rc == QDialog::Accepted)
                {
                    this->setModelSet(m_modelSetFormDialog->getModelSet(), m_modelSetFormDialog->getSimulatorInfo());
                }
            }
            else
            {
                const CStatusMessage m = CStatusMessage(this).error("No model data for %1") << simulator.toQString(true);
                if (mc) { mc->showOverlayMessage(m); }
            }
        }

        void CDbOwnModelSetComponent::firstSet()
        {
            if (!m_firstModelSet)
            {
                m_firstModelSet.reset(new CFirstModelSetDialog(this));
            }
            m_firstModelSet->show();
        }

        void CDbOwnModelSetComponent::copyFromAnotherSwift()
        {
            if (!m_copyFromAnotherSwift)
            {
                m_copyFromAnotherSwift.reset(new CCopyModelsFromOtherSwiftVersionsDialog(this));
            }
            m_copyFromAnotherSwift->show();
        }

        void CDbOwnModelSetComponent::changeSimulator(const CSimulatorInfo &simulator)
        {
            m_modelSetLoader.setSimulator(simulator);
            ui->tvp_OwnModelSet->setSimulatorForLoading(simulator);
            ui->le_Simulator->setText(simulator.toQString(true));
            ui->comp_SimulatorSelector->setValue(simulator);
            this->updateViewToCurrentModels();
        }

        void CDbOwnModelSetComponent::showAirlineAircraftMatrix() const
        {
            const CAircraftModelList set(this->getModelSetFromView());
            const QString file = CAircraftModelUtilities::createIcaoAirlineAircraftHtmlMatrixFile(set, CGuiApplication::getTemporaryDirectory());
            if (file.isEmpty()) { return; }
            QDesktopServices::openUrl(QUrl::fromLocalFile(file));
        }

        void CDbOwnModelSetComponent::setSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            const CSimulatorInfo currentSimulator = m_modelSetLoader.getSimulator();
            if (currentSimulator == simulator) { return; } // avoid unnecessary signals
            this->changeSimulator(simulator);
        }

        void CDbOwnModelSetComponent::updateDistributorOrder(const CSimulatorInfo &simulator)
        {
            CAircraftModelList modelSet = m_modelSetLoader.getAircraftModels(simulator);
            if (modelSet.isEmpty()) { return; }
            const CDistributorListPreferences preferences = m_distributorPreferences.getThreadLocal();
            const CDistributorList distributors = preferences.getDistributors(simulator);
            if (distributors.isEmpty()) { return; }
            modelSet.updateDistributorOrder(distributors);
            m_modelSetLoader.setModels(modelSet, simulator);

            // display?
            const CSimulatorInfo currentSimulator(this->getModelSetSimulator());
            if (simulator == currentSimulator)
            {
                ui->tvp_OwnModelSet->updateContainerAsync(modelSet);
            }
        }

        void CDbOwnModelSetComponent::CLoadModelSetMenu::customMenu(CMenuActions &menuActions)
        {
            // for the moment I use all sims, I could restrict to CSimulatorInfo::getLocallyInstalledSimulators();
            const CSimulatorInfo sims =  CSimulatorInfo::allSimulators();
            const bool noSims = sims.isNoSimulator() || sims.isUnspecified();
            if (!noSims)
            {
                CDbOwnModelSetComponent *ownModelSetComp = qobject_cast<CDbOwnModelSetComponent *>(this->parent());
                Q_ASSERT_X(ownModelSetComp, Q_FUNC_INFO, "Cannot access parent");
                if (m_setActions.isEmpty())
                {
                    if (sims.fsx())
                    {
                        QAction *a = new QAction(CIcons::appModels16(), "FSX models", this);
                        connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelSetComp->setSimulator(CSimulatorInfo(CSimulatorInfo::FSX));
                        });
                        m_setActions.append(a);

                        a = new QAction(CIcons::appModels16(), "New set FSX models", this);
                        connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::FSX));
                        });
                        m_setNewActions.append(a);
                    }
                    if (sims.p3d())
                    {
                        QAction *a = new QAction(CIcons::appModels16(), "P3D models", this);
                        connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelSetComp->setSimulator(CSimulatorInfo(CSimulatorInfo::P3D));
                        });
                        m_setActions.append(a);

                        a = new QAction(CIcons::appModels16(), "New set P3D models", this);
                        connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::P3D));
                        });
                        m_setNewActions.append(a);
                    }
                    if (sims.fs9())
                    {
                        QAction *a = new QAction(CIcons::appModels16(), "FS9 models", this);
                        connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelSetComp->setSimulator(CSimulatorInfo(CSimulatorInfo::FS9));
                        });
                        m_setActions.append(a);

                        a = new QAction(CIcons::appModels16(), "New set FS9 models", this);
                        connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::FS9));
                        });
                        m_setNewActions.append(a);
                    }
                    if (sims.xplane())
                    {
                        QAction *a = new QAction(CIcons::appModels16(), "XPlane models", this);
                        connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelSetComp->setSimulator(CSimulatorInfo(CSimulatorInfo::XPLANE));
                        });
                        m_setActions.append(a);

                        a = new QAction(CIcons::appModels16(), "New set XPlane models", this);
                        connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::XPLANE));
                        });
                        m_setNewActions.append(a);
                    }

                    QAction *a = new QAction(CIcons::appDistributors16(), "Apply distributor preferences", this);
                    connect(a, &QAction::triggered, ownModelSetComp, &CDbOwnModelSetComponent::distributorPreferencesChanged);
                    m_setActions.append(a);
                }
                menuActions.addMenuModelSet();
                menuActions.addActions(m_setActions, CMenuAction::pathModelSet());
                menuActions.addActions(m_setNewActions, CMenuAction::pathModelSetNew());
            }
            this->nestedCustomMenu(menuActions);
        }
    } // ns
} // ns
