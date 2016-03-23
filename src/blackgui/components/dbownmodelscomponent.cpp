/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbownmodelscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/menus/aircraftmodelmenus.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/logmessage.h"
#include "ui_dbownmodelscomponent.h"

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
            ui->tvp_OwnAircraftModels->setCustomMenu(new CShowSimulatorFileMenu(ui->tvp_OwnAircraftModels, false), true);

            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::requestUpdate, this, &CDbOwnModelsComponent::ps_requestOwnModelsUpdate);

            this->m_lastInteractions.synchronize();
            const CSimulatorInfo sim = this->m_lastInteractions.get().getLastSimulatorSelection();
            if (sim.isSingleSimulator())
            {
                // if we have already use this before, use it again, but only from cache
                this->initModelLoader(sim);
                this->m_modelLoader->startLoading(IAircraftModelLoader::CacheOnly);
            }

            ui->tvp_OwnAircraftModels->setCustomMenu(new CMergeWithDbDataMenu(ui->tvp_OwnAircraftModels, this->m_modelLoader.get(), false));
            ui->tvp_OwnAircraftModels->setCustomMenu(new CLoadModelsMenu(this, true));
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

        const CSimulatorInfo &CDbOwnModelsComponent::getOwnModelsSimulator() const
        {
            static const CSimulatorInfo noSim;
            if (!this->m_modelLoader) { return noSim; }
            return this->m_modelLoader->getSimulator();
        }

        int CDbOwnModelsComponent::getOwnModelsCount() const
        {
            if (!this->m_modelLoader) { return 0; }
            return this->m_modelLoader->getAircraftModelsCount();
        }

        void CDbOwnModelsComponent::gracefulShutdown()
        {
            if (this->m_modelLoader) { this->m_modelLoader->gracefulShutdown(); }
        }

        bool CDbOwnModelsComponent::initModelLoader(const CSimulatorInfo &simInfo)
        {
            // already loaded
            Q_ASSERT_X(simInfo.isSingleSimulator(), Q_FUNC_INFO, "need single simulator");
            if (this->m_modelLoader && this->m_modelLoader->supportsSimulator(simInfo))
            {
                this->setSaveFileName(simInfo);
                return true;
            }

            // unload old
            if (this->m_modelLoader)
            {
                this->m_modelLoader->gracefulShutdown();
            }

            // create loader, also syncronizes the caches
            this->m_modelLoader = IAircraftModelLoader::createModelLoader(simInfo);
            if (!this->m_modelLoader || !this->m_modelLoader->supportsSimulator(simInfo))
            {
                CLogMessage(this).error("Failed to init model loader %1") << simInfo.toQString();
                this->m_modelLoader.reset();
                return false;
            }
            else
            {
                bool c = connect(this->m_modelLoader.get(), &IAircraftModelLoader::loadingFinished, this, &CDbOwnModelsComponent::ps_onOwnModelsLoadingFinished);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Failed connect for model loader");
                Q_UNUSED(c);
                this->setSaveFileName(simInfo);
                return true;
            }
        }

        void CDbOwnModelsComponent::setSaveFileName(const CSimulatorInfo &sim)
        {
            Q_ASSERT_X(sim.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            const QString n("simulator models " + sim.toQString(true));
            this->ui->tvp_OwnAircraftModels->setSaveFileName(n);
        }

        void CDbOwnModelsComponent::CLoadModelsMenu::customMenu(QMenu &menu) const
        {
            CSimulatorInfo sims = CSimulatorInfo::getLocallyInstalledSimulators();
            bool noSims = sims.isNoSimulator() || sims.isUnspecified();
            if (!noSims)
            {
                this->addSeparator(menu);
                QMenu *load = menu.addMenu(CIcons::appModels16(), "Load installed models");
                CDbOwnModelsComponent *ownModelsComp = qobject_cast<CDbOwnModelsComponent *>(this->parent());
                Q_ASSERT_X(ownModelsComp, Q_FUNC_INFO, "Cannot access parent");
                if (sims.fsx())
                {
                    load->addAction(CIcons::appModels16(), "FSX models", ownModelsComp, [ownModelsComp]()
                    {
                        ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::FSX), IAircraftModelLoader::InBackgroundWithCache);
                    });
                }
                if (sims.p3d())
                {
                    load->addAction(CIcons::appModels16(), "P3D models", ownModelsComp, [ownModelsComp]()
                    {
                        ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::P3D), IAircraftModelLoader::InBackgroundWithCache);
                    });
                }
                if (sims.fs9())
                {
                    load->addAction(CIcons::appModels16(), "FS9 models", ownModelsComp, [ownModelsComp]()
                    {
                        ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::FS9), IAircraftModelLoader::InBackgroundWithCache);
                    });
                }
                if (sims.xplane())
                {
                    load->addAction(CIcons::appModels16(), "XP models", ownModelsComp, [ownModelsComp]()
                    {
                        ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::XPLANE), IAircraftModelLoader::InBackgroundWithCache);
                    });
                }

                // with models loaded I allow a refresh reload
                if (sGui->getWebDataServices() && sGui->getWebDataServices()->getModelsCount() > 0)
                {
                    QMenu *reloadMenu = load->addMenu("Force reload");
                    if (sims.fsx())
                    {
                        reloadMenu->addAction(CIcons::appModels16(), "FSX models", ownModelsComp, [ownModelsComp]()
                        {
                            ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::FSX), IAircraftModelLoader::InBackgroundNoCache);
                        });
                    }
                    if (sims.p3d())
                    {
                        reloadMenu->addAction(CIcons::appModels16(), "P3D models", ownModelsComp, [ownModelsComp]()
                        {
                            ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::P3D), IAircraftModelLoader::InBackgroundNoCache);
                        });
                    }
                    if (sims.fs9())
                    {
                        reloadMenu->addAction(CIcons::appModels16(), "FS9 models", ownModelsComp, [ownModelsComp]()
                        {
                            ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::FS9), IAircraftModelLoader::InBackgroundNoCache);
                        });
                    }
                    if (sims.xplane())
                    {
                        reloadMenu->addAction(CIcons::appModels16(), "XP models", ownModelsComp, [ownModelsComp]()
                        {
                            ownModelsComp->ps_requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::XPLANE), IAircraftModelLoader::InBackgroundNoCache);
                        });
                    }
                }
            }
            this->nestedCustomMenu(menu);
        }

        void CDbOwnModelsComponent::ps_requestOwnModelsUpdate()
        {
            if (!this->m_modelLoader) { return; }
            this->ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(
                this->m_modelLoader->getAircraftModels()
            );
        }

        void CDbOwnModelsComponent::ps_loadInstalledModels(const CSimulatorInfo &simInfo, IAircraftModelLoader::LoadMode mode)
        {
            if (!this->initModelLoader(simInfo))
            {
                CLogMessage(this).error("Cannot load model loader for %1") << simInfo.toQString();
                return;
            }

            if (!this->m_modelLoader->isLoadingFinished())
            {
                CLogMessage(this).info("Loading for %1 already in progress") << simInfo.toQString();
                return;
            }

            CLogMessage(this).info("Starting loading for %1") << simInfo.toQString();
            this->ui->tvp_OwnAircraftModels->showLoadIndicator();
            Q_ASSERT_X(sGui && sGui->getWebDataServices(), Q_FUNC_INFO, "missing web data services");
            this->m_modelLoader->startLoading(mode, sGui->getWebDataServices()->getModels());
        }

        void CDbOwnModelsComponent::ps_onOwnModelsLoadingFinished(bool success, const CSimulatorInfo &simInfo)
        {
            Q_ASSERT_X(simInfo.isSingleSimulator(), Q_FUNC_INFO, "Expect single simulator");
            if (success && this->m_modelLoader)
            {
                const CAircraftModelList models(this->m_modelLoader->getAircraftModels());
                const int modelsLoaded = models.size();
                this->ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(models);
                if (modelsLoaded > 0)
                {
                    // store for later
                    Data::CDbOwnModelsComponent mc(this->m_lastInteractions.get());
                    if (simInfo.isSingleSimulator() &&  mc.getLastSimulatorSelection() != simInfo)
                    {
                        mc.setLastSimulatorSelection(simInfo);
                        this->m_lastInteractions.set(mc);
                    }
                }
                else
                {
                    // loading ok, but no data
                    CLogMessage(this).warning("Loading completed, but no models");
                }
            }
            else
            {
                this->ui->tvp_OwnAircraftModels->hideLoadIndicator();
                CLogMessage(this).error("Loading of models failed, simulator %1") << simInfo.toQString();
            }
            this->ui->tvp_OwnAircraftModels->hideLoadIndicator();
        }

        void CDbOwnModelsComponent::ps_requestSimulatorModels(const CSimulatorInfo &simInfo, IAircraftModelLoader::LoadMode mode)
        {
            this->ps_loadInstalledModels(simInfo, mode);
        }
    } // ns
} // ns
