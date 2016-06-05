/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbownmodelscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/menus/aircraftmodelmenus.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "ui_dbownmodelscomponent.h"

#include <QAction>
#include <QIcon>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Menus;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Components
    {
        CDbOwnModelsComponent::CDbOwnModelsComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbOwnModelsComponent)
        {
            ui->setupUi(this);
            ui->tvp_OwnAircraftModels->setAircraftModelMode(CAircraftModelListModel::OwnSimulatorModelMapping);
            ui->tvp_OwnAircraftModels->addFilterDialog();
            ui->tvp_OwnAircraftModels->setDisplayAutomatically(true);
            ui->tvp_OwnAircraftModels->setCustomMenu(new CMergeWithDbDataMenu(ui->tvp_OwnAircraftModels, this->modelLoader(), false));
            ui->tvp_OwnAircraftModels->setCustomMenu(new CLoadModelsMenu(this, true));

            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::requestUpdate, this, &CDbOwnModelsComponent::ps_requestOwnModelsUpdate);

            // should be single simulator or no simulator (default)
            this->m_simulatorSelection.synchronize();
            const CSimulatorInfo simulator(this->m_simulatorSelection.getCopy());
            const bool s = this->initModelLoader(!simulator.isSingleSimulator() ? CSimulatorInfo(CSimulatorInfo::FSX) : simulator);
            if (s)
            {
                this->m_modelLoader->startLoading(IAircraftModelLoader::CacheOnly);
            }
            else
            {
                CLogMessage(this).error("Init of model loader failed in component");
            }
        }

        CDbOwnModelsComponent::~CDbOwnModelsComponent()
        {
            // void
        }

        CAircraftModelView *CDbOwnModelsComponent::view() const
        {
            return ui->tvp_OwnAircraftModels;
        }

        CAircraftModelListModel *CDbOwnModelsComponent::model() const
        {
            return ui->tvp_OwnAircraftModels->derivedModel();
        }

        IAircraftModelLoader *CDbOwnModelsComponent::modelLoader() const
        {
            return this->m_modelLoader.get();
        }

        CAircraftModel CDbOwnModelsComponent::getOwnModelForModelString(const QString &modelString) const
        {
            if (!this->m_modelLoader) { return CAircraftModel(); }
            return this->m_modelLoader->getAircraftModels().findFirstByModelStringOrDefault(modelString);
        }

        CAircraftModelList CDbOwnModelsComponent::getOwnModels() const
        {
            static const CAircraftModelList empty;
            if (!this->m_modelLoader) { return empty; }
            return this->m_modelLoader->getAircraftModels();
        }

        CAircraftModelList CDbOwnModelsComponent::getOwnSelectedModels() const
        {
            return ui->tvp_OwnAircraftModels->selectedObjects();
        }

        const CSimulatorInfo CDbOwnModelsComponent::getOwnModelsSimulator() const
        {
            static const CSimulatorInfo noSim;
            if (!this->m_modelLoader) { return noSim; }
            return this->m_modelLoader->getSimulator();
        }

        void CDbOwnModelsComponent::setSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            this->ps_loadInstalledModels(simulator, IAircraftModelLoader::InBackgroundWithCache);
        }

        int CDbOwnModelsComponent::getOwnModelsCount() const
        {
            if (!this->m_modelLoader) { return 0; }
            return this->m_modelLoader->getAircraftModelsCount();
        }

        CStatusMessage CDbOwnModelsComponent::updateViewAndCache(const CAircraftModelList &models)
        {
            const CStatusMessage m  = this->m_modelLoader->setCachedModels(models, this->getOwnModelsSimulator());
            if (m.isSuccess())
            {
                ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(models);
            }
            return m;
        }

        void CDbOwnModelsComponent::gracefulShutdown()
        {
            if (this->m_modelLoader) { this->m_modelLoader->gracefulShutdown(); }
        }

        bool CDbOwnModelsComponent::initModelLoader(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");

            // already loaded
            if (this->m_modelLoader && this->m_modelLoader->supportsSimulator(simulator))
            {
                this->setSaveFileName(simulator);
                return true;
            }

            // mismatching loader
            if (this->m_modelLoader)
            {
                this->m_modelLoader->gracefulShutdown();
            }

            // create loader, also syncronizes the caches
            this->m_modelLoader = IAircraftModelLoader::createModelLoader(simulator); // last selected simulator or explicit given
            if (!this->m_modelLoader || !this->m_modelLoader->supportsSimulator(simulator))
            {
                CLogMessage(this).error("Failed to init model loader %1") << simulator.toQString();
                this->m_modelLoader.reset();
                return false;
            }
            else
            {
                bool c = connect(this->m_modelLoader.get(), &IAircraftModelLoader::loadingFinished, this, &CDbOwnModelsComponent::ps_onOwnModelsLoadingFinished);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Failed connect for model loader");
                Q_UNUSED(c);
                this->setSaveFileName(simulator);
                return true;
            }
        }

        void CDbOwnModelsComponent::setSaveFileName(const CSimulatorInfo &sim)
        {
            Q_ASSERT_X(sim.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            const QString n("simulator models " + sim.toQString(true));
            this->ui->tvp_OwnAircraftModels->setSaveFileName(n);
        }

        void CDbOwnModelsComponent::CLoadModelsMenu::customMenu(CMenuActions &menuActions)
        {
            const CSimulatorInfo sims = CSimulatorInfo::getLocallyInstalledSimulators();
            const bool noSims = sims.isNoSimulator() || sims.isUnspecified();
            if (!noSims)
            {
                CDbOwnModelsComponent *ownModelsComp = qobject_cast<CDbOwnModelsComponent *>(this->parent());
                Q_ASSERT_X(ownModelsComp, Q_FUNC_INFO, "Cannot access parent");

                if (this->m_loadActions.isEmpty()) { this->m_loadActions = QList<QAction *>({nullptr, nullptr, nullptr, nullptr}); }
                menuActions.addMenuSimulator();
                if (sims.fsx())
                {
                    if (!this->m_loadActions[0])
                    {
                        this->m_loadActions[0] = new QAction(CIcons::appModels16(), "FSX models", this);
                        connect(this->m_loadActions[0], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::FSX), IAircraftModelLoader::InBackgroundWithCache);
                        });
                    }
                    menuActions.addAction(this->m_loadActions[0], CMenuAction::pathSimulator());
                }
                if (sims.p3d())
                {
                    if (!this->m_loadActions[1])
                    {
                        this->m_loadActions[1] = new QAction(CIcons::appModels16(), "P3D models", this);
                        connect(this->m_loadActions[1], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::P3D), IAircraftModelLoader::InBackgroundWithCache);
                        });
                    }
                    menuActions.addAction(this->m_loadActions[1], CMenuAction::pathSimulator());
                }
                if (sims.fs9())
                {
                    if (!this->m_loadActions[2])
                    {
                        this->m_loadActions[2] = new QAction(CIcons::appModels16(), "FS9 models", this);
                        connect(this->m_loadActions[2], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::FS9), IAircraftModelLoader::InBackgroundWithCache);
                        });
                    }
                    menuActions.addAction(this->m_loadActions[2], CMenuAction::pathSimulator());
                }
                if (sims.xplane())
                {
                    if (!this->m_loadActions[3])
                    {
                        this->m_loadActions[3] = new QAction(CIcons::appModels16(), "XPlane models", this);
                        connect(this->m_loadActions[3], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::XPLANE), IAircraftModelLoader::InBackgroundWithCache);
                        });
                    }
                    menuActions.addAction(this->m_loadActions[3], CMenuAction::pathSimulator());
                }

                // with models loaded I allow a refresh reload
                // I need those models because I want to merge with DM data in the loader
                if (sGui->getWebDataServices() && sGui->getWebDataServices()->getModelsCount() > 0)
                {
                    if (this->m_reloadActions.isEmpty()) { this->m_reloadActions = QList<QAction *>({nullptr, nullptr, nullptr, nullptr}); }
                    menuActions.addMenu(CIcons::refresh16(), "Force model reload", CMenuAction::pathSimulatorModelsReload());
                    if (sims.fsx())
                    {
                        if (!this->m_reloadActions[0])
                        {
                            this->m_reloadActions[0] = new QAction(CIcons::appModels16(), "FSX models", this);
                            connect(this->m_reloadActions[0], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::FSX), IAircraftModelLoader::InBackgroundNoCache);
                            });
                        }
                        menuActions.addAction(this->m_reloadActions[0], CMenuAction::pathSimulatorModelsReload());
                    }
                    if (sims.p3d())
                    {
                        if (!this->m_reloadActions[1])
                        {
                            this->m_reloadActions[1] = new QAction(CIcons::appModels16(), "P3D models", this);
                            connect(this->m_reloadActions[1], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::P3D), IAircraftModelLoader::InBackgroundNoCache);
                            });
                        }
                        menuActions.addAction(this->m_reloadActions[1], CMenuAction::pathSimulatorModelsReload());
                    }
                    if (sims.fs9())
                    {
                        if (!this->m_reloadActions[2])
                        {
                            this->m_reloadActions[2] = new QAction(CIcons::appModels16(), "FS9 models", this);
                            connect(this->m_reloadActions[2], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::FS9), IAircraftModelLoader::InBackgroundNoCache);
                            });
                        }
                        menuActions.addAction(this->m_reloadActions[2], CMenuAction::pathSimulatorModelsReload());
                    }
                    if (sims.xplane())
                    {
                        if (!this->m_reloadActions[3])
                        {
                            this->m_reloadActions[3] = new QAction(CIcons::appModels16(), "XPlane models", this);
                            connect(this->m_reloadActions[3], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::XPLANE), IAircraftModelLoader::InBackgroundNoCache);
                            });
                        }
                        menuActions.addAction(this->m_reloadActions[3], CMenuAction::pathSimulatorModelsReload());
                    }
                }
                else
                {
                    // dummy action grayed out
                    CMenuAction a = menuActions.addAction(CIcons::refresh16(), "Force model reload impossible, no DB data", CMenuAction::pathSimulator());
                    a.setActionEnabled(false); // gray out
                }
            }
            this->nestedCustomMenu(menuActions);
        }

        void CDbOwnModelsComponent::ps_requestOwnModelsUpdate()
        {
            if (!this->m_modelLoader) { return; }
            this->ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(
                this->m_modelLoader->getAircraftModels()
            );
        }

        void CDbOwnModelsComponent::ps_loadInstalledModels(const CSimulatorInfo &simulator, IAircraftModelLoader::LoadMode mode)
        {
            if (!this->initModelLoader(simulator))
            {
                CLogMessage(this).error("Cannot load model loader for %1") << simulator.toQString();
                return;
            }

            if (!this->m_modelLoader->isLoadingFinished())
            {
                CLogMessage(this).info("Loading for %1 already in progress") << simulator.toQString();
                return;
            }

            CLogMessage(this).info("Starting loading for %1") << simulator.toQString();
            this->ui->tvp_OwnAircraftModels->showLoadIndicator();
            Q_ASSERT_X(sGui && sGui->getWebDataServices(), Q_FUNC_INFO, "missing web data services");
            this->m_modelLoader->startLoading(mode, sGui->getWebDataServices()->getModels());
        }

        void CDbOwnModelsComponent::ps_onOwnModelsLoadingFinished(bool success, const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Expect single simulator");
            if (success && this->m_modelLoader)
            {
                const CAircraftModelList models(this->m_modelLoader->getAircraftModels());
                const int modelsLoaded = models.size();
                this->ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(models);
                if (modelsLoaded < 1)
                {
                    // loading ok, but no data
                    CLogMessage(this).warning("Loading completed, but no models");
                }
            }
            else
            {
                this->ui->tvp_OwnAircraftModels->clear();
                CLogMessage(this).error("Loading of models failed, simulator %1") << simulator.toQString();
            }
            this->ui->tvp_OwnAircraftModels->hideLoadIndicator();
        }

        void CDbOwnModelsComponent::ps_requestSimulatorModels(const CSimulatorInfo &simulator, IAircraftModelLoader::LoadMode mode)
        {
            this->ps_loadInstalledModels(simulator, mode);
        }
    } // ns
} // ns
