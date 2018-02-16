/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackcore/db/databaseutils.h"
#include "blackgui/components/dbownmodelscomponent.h"
#include "blackgui/components/simulatorselector.h"
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
#include <QFileDialog>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Db;
using namespace BlackGui::Menus;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Components
    {
        CDbOwnModelsComponent::CDbOwnModelsComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
            ui(new Ui::CDbOwnModelsComponent)
        {
            ui->setupUi(this);
            ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
            ui->tvp_OwnAircraftModels->setAircraftModelMode(CAircraftModelListModel::OwnAircraftModelMappingTool);
            ui->tvp_OwnAircraftModels->addFilterDialog();
            ui->tvp_OwnAircraftModels->setDisplayAutomatically(true);
            ui->tvp_OwnAircraftModels->setCustomMenu(new CLoadModelsMenu(this, true));
            ui->tvp_OwnAircraftModels->setSimulatorForLoading(ui->comp_SimulatorSelector->getValue());

            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::requestUpdate, this, &CDbOwnModelsComponent::requestOwnModelsUpdate);

            // Last selection isPinned -> no sync needed
            const CSimulatorInfo simulator(m_simulatorSelection.get());
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            const bool success = this->initModelLoader(simulator);
            if (success)
            {
                m_modelLoader->startLoading(IAircraftModelLoader::CacheOnly);
            }
            else
            {
                CLogMessage(this).error("Init of model loader failed in component");
            }

            ui->comp_SimulatorSelector->setValue(simulator);
            ui->le_Simulator->setText(simulator.toQString());
            connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CDbOwnModelsComponent::onSimulatorChanged);

            // menu
            ui->tvp_OwnAircraftModels->setCustomMenu(new CConsolidateWithDbDataMenu(ui->tvp_OwnAircraftModels, this, false));
        }

        CDbOwnModelsComponent::~CDbOwnModelsComponent()
        {
            // void
        }

        const CLogCategoryList &CDbOwnModelsComponent::getLogCategories()
        {
            static const CLogCategoryList l({ CLogCategory::modelGui(), CLogCategory::guiComponent() });
            return l;
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
            return m_modelLoader.get();
        }

        bool CDbOwnModelsComponent::requestModelsInBackground(const CSimulatorInfo &simulator, bool onlyIfNotEmpty)
        {
            this->setSimulator(simulator);
            if (onlyIfNotEmpty && this->getOwnModelsCount() > 0) { return false; }
            this->requestSimulatorModels(simulator, onlyIfNotEmpty ? IAircraftModelLoader::InBackgroundNoCache : IAircraftModelLoader::LoadInBackground);
            return true;
        }

        CAircraftModel CDbOwnModelsComponent::getOwnModelForModelString(const QString &modelString) const
        {
            if (!m_modelLoader) { return CAircraftModel(); }
            return m_modelLoader->getAircraftModels().findFirstByModelStringOrDefault(modelString);
        }

        CAircraftModelList CDbOwnModelsComponent::getOwnModels() const
        {
            static const CAircraftModelList empty;
            if (!m_modelLoader) { return empty; }
            return m_modelLoader->getAircraftModels();
        }

        CAircraftModelList CDbOwnModelsComponent::getOwnCachedModels(const CSimulatorInfo &simulator) const
        {
            static const CAircraftModelList empty;
            if (!m_modelLoader) { return empty; }
            return m_modelLoader->getCachedAircraftModels(simulator);
        }

        CAircraftModelList CDbOwnModelsComponent::getOwnSelectedModels() const
        {
            return ui->tvp_OwnAircraftModels->selectedObjects();
        }

        const CSimulatorInfo CDbOwnModelsComponent::getOwnModelsSimulator() const
        {
            static const CSimulatorInfo noSim;
            if (!m_modelLoader) { return noSim; }
            return m_modelLoader->getSimulator();
        }

        void CDbOwnModelsComponent::setSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            this->loadInstalledModels(simulator, IAircraftModelLoader::InBackgroundWithCache);
            ui->comp_SimulatorSelector->setValue(simulator);
            ui->le_Simulator->setText(simulator.toQString());
        }

        int CDbOwnModelsComponent::getOwnModelsCount() const
        {
            if (!m_modelLoader) { return 0; }
            return m_modelLoader->getAircraftModelsCount();
        }

        QString CDbOwnModelsComponent::getInfoString() const
        {
            if (!m_modelLoader) { return ""; }
            return m_modelLoader->getInfoString();
        }

        QString CDbOwnModelsComponent::getInfoStringFsFamily() const
        {
            if (!m_modelLoader) { return ""; }
            return m_modelLoader->getInfoStringFsFamily();
        }

        CStatusMessage CDbOwnModelsComponent::updateViewAndCache(const CAircraftModelList &models)
        {
            const CStatusMessage m  = m_modelLoader->setCachedModels(models, this->getOwnModelsSimulator());
            if (m.isSuccess())
            {
                ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(models);
            }
            return m;
        }

        void CDbOwnModelsComponent::gracefulShutdown()
        {
            if (m_modelLoader) { m_modelLoader->gracefulShutdown(); }
        }

        void CDbOwnModelsComponent::setModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            this->modelLoader()->setCachedModels(models, simulator);
            ui->tvp_OwnAircraftModels->replaceOrAddModelsWithString(models);
        }

        void CDbOwnModelsComponent::updateModels(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            this->modelLoader()->replaceOrAddCachedModels(models, simulator);
            const CAircraftModelList allModels(m_modelLoader->getAircraftModels());
            ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(allModels);
        }

        bool CDbOwnModelsComponent::initModelLoader(const CSimulatorInfo &simulator)
        {
            // called when simulator is changed / init
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");

            // already loaded
            if (m_modelLoader && m_modelLoader->supportsSimulator(simulator))
            {
                this->setSaveFileName(simulator);
                return true;
            }

            // mismatching loader
            if (m_modelLoader)
            {
                m_modelLoader->gracefulShutdown();
            }

            // create loader, also synchronizes the caches
            m_modelLoader = IAircraftModelLoader::createModelLoader(simulator); // last selected simulator or explicit given
            if (!m_modelLoader || !m_modelLoader->supportsSimulator(simulator))
            {
                CLogMessage(this).error("Failed to init model loader %1") << simulator.toQString();
                m_modelLoader.reset();
                return false;
            }
            else
            {
                const bool c = connect(m_modelLoader.get(), &IAircraftModelLoader::loadingFinished,
                                       this, &CDbOwnModelsComponent::onOwnModelsLoadingFinished, Qt::QueuedConnection);
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
            ui->tvp_OwnAircraftModels->setSaveFileName(n);
        }

        QString CDbOwnModelsComponent::directorySelector(const CSimulatorInfo &simulatorInfo)
        {
            const QString text("Open directory (%1)");
            const QString dir = QFileDialog::getExistingDirectory(nullptr, text.arg(simulatorInfo.toQString()), "",
                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            return dir;
        }

        void CDbOwnModelsComponent::CLoadModelsMenu::customMenu(CMenuActions &menuActions)
        {
            // for the moment I use all sims, I could restrict to CSimulatorInfo::getLocallyInstalledSimulators();
            const CSimulatorInfo sims =  CSimulatorInfo::allSimulators();
            const bool noSims = sims.isNoSimulator() || sims.isUnspecified();
            if (!noSims)
            {
                CDbOwnModelsComponent *ownModelsComp = qobject_cast<CDbOwnModelsComponent *>(this->parent());
                Q_ASSERT_X(ownModelsComp, Q_FUNC_INFO, "Cannot access parent");

                if (m_loadActions.isEmpty()) { m_loadActions = QList<QAction *>({nullptr, nullptr, nullptr, nullptr}); }
                menuActions.addMenuSimulator();
                if (sims.fsx())
                {
                    if (!m_loadActions[0])
                    {
                        m_loadActions[0] = new QAction(CIcons::appModels16(), "FSX models", this);
                        connect(m_loadActions[0], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelsComp->requestSimulatorModelsWithCacheInBackground(CSimulatorInfo(CSimulatorInfo::FSX));
                        });
                    }
                    menuActions.addAction(m_loadActions[0], CMenuAction::pathSimulator());
                }
                if (sims.p3d())
                {
                    if (!m_loadActions[1])
                    {
                        m_loadActions[1] = new QAction(CIcons::appModels16(), "P3D models", this);
                        connect(m_loadActions[1], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelsComp->requestSimulatorModelsWithCacheInBackground(CSimulatorInfo(CSimulatorInfo::P3D));
                        });
                    }
                    menuActions.addAction(m_loadActions[1], CMenuAction::pathSimulator());
                }
                if (sims.fs9())
                {
                    if (!m_loadActions[2])
                    {
                        m_loadActions[2] = new QAction(CIcons::appModels16(), "FS9 models", this);
                        connect(m_loadActions[2], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelsComp->requestSimulatorModelsWithCacheInBackground(CSimulatorInfo(CSimulatorInfo::FS9));
                        });
                    }
                    menuActions.addAction(m_loadActions[2], CMenuAction::pathSimulator());
                }
                if (sims.xplane())
                {
                    if (!m_loadActions[3])
                    {
                        m_loadActions[3] = new QAction(CIcons::appModels16(), "XPlane models", this);
                        connect(m_loadActions[3], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                        {
                            Q_UNUSED(checked);
                            ownModelsComp->requestSimulatorModelsWithCacheInBackground(CSimulatorInfo(CSimulatorInfo::XPLANE));
                        });
                    }
                    menuActions.addAction(m_loadActions[3], CMenuAction::pathSimulator());
                }

                // with models loaded I allow a refresh reload
                // I need those models because I want to merge with DM data in the loader
                if (sGui->getWebDataServices() && sGui->getWebDataServices()->getModelsCount() > 0)
                {
                    if (m_reloadActions.isEmpty()) { m_reloadActions = QList<QAction *>({nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}); }
                    menuActions.addMenu(CIcons::refresh16(), "Force model reload", CMenuAction::pathSimulatorModelsReload());
                    if (sims.fsx())
                    {
                        if (!m_reloadActions[0])
                        {
                            m_reloadActions[0] = new QAction(CIcons::appModels16(), "FSX models", this);
                            connect(m_reloadActions[0], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                ownModelsComp->requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::FSX), IAircraftModelLoader::InBackgroundNoCache);
                            });

                            m_reloadActions[1] = new QAction(CIcons::appModels16(), "FSX models from directory", this);
                            connect(m_reloadActions[1], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                const CSimulatorInfo sim(CSimulatorInfo::FSX);
                                const QString dir = CDbOwnModelsComponent::directorySelector(sim);
                                if (!dir.isEmpty())
                                {
                                    ownModelsComp->requestSimulatorModels(sim, IAircraftModelLoader::InBackgroundNoCache, QStringList(dir));
                                }
                            });
                        }
                        menuActions.addAction(m_reloadActions[0], CMenuAction::pathSimulatorModelsReload());
                        menuActions.addAction(m_reloadActions[1], CMenuAction::pathSimulatorModelsReload());
                    }
                    if (sims.p3d())
                    {
                        if (!m_reloadActions[2])
                        {
                            m_reloadActions[2] = new QAction(CIcons::appModels16(), "P3D models", this);
                            connect(m_reloadActions[2], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                ownModelsComp->requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::P3D), IAircraftModelLoader::InBackgroundNoCache);
                            });

                            m_reloadActions[3] = new QAction(CIcons::appModels16(), "P3D models from directoy", this);
                            connect(m_reloadActions[3], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                const CSimulatorInfo sim(CSimulatorInfo::P3D);
                                const QString dir = CDbOwnModelsComponent::directorySelector(sim);
                                if (!dir.isEmpty())
                                {
                                    ownModelsComp->requestSimulatorModels(sim, IAircraftModelLoader::InBackgroundNoCache, QStringList(dir));
                                }
                            });
                        }
                        menuActions.addAction(m_reloadActions[2], CMenuAction::pathSimulatorModelsReload());
                        menuActions.addAction(m_reloadActions[3], CMenuAction::pathSimulatorModelsReload());
                    }
                    if (sims.fs9())
                    {
                        if (!m_reloadActions[4])
                        {
                            m_reloadActions[4] = new QAction(CIcons::appModels16(), "FS9 models", this);
                            connect(m_reloadActions[4], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                ownModelsComp->requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::FS9), IAircraftModelLoader::InBackgroundNoCache);
                            });

                            m_reloadActions[5] = new QAction(CIcons::appModels16(), "FS9 models from directoy", this);
                            connect(m_reloadActions[5], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                const CSimulatorInfo sim(CSimulatorInfo::FS9);
                                const QString dir = CDbOwnModelsComponent::directorySelector(sim);
                                if (!dir.isEmpty())
                                {
                                    ownModelsComp->requestSimulatorModels(sim, IAircraftModelLoader::InBackgroundNoCache, QStringList(dir));
                                }
                            });
                        }
                        menuActions.addAction(m_reloadActions[4], CMenuAction::pathSimulatorModelsReload());
                        menuActions.addAction(m_reloadActions[5], CMenuAction::pathSimulatorModelsReload());
                    }
                    if (sims.xplane())
                    {
                        if (!m_reloadActions[6])
                        {
                            m_reloadActions[6] = new QAction(CIcons::appModels16(), "XPlane models", this);
                            connect(m_reloadActions[6], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                ownModelsComp->requestSimulatorModels(CSimulatorInfo(CSimulatorInfo::XPLANE), IAircraftModelLoader::InBackgroundNoCache);
                            });
                            m_reloadActions[7] = new QAction(CIcons::appModels16(), "XPlane models from directoy", this);
                            connect(m_reloadActions[7], &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked)
                            {
                                Q_UNUSED(checked);
                                const CSimulatorInfo sim(CSimulatorInfo::XPLANE);
                                const QString dir = CDbOwnModelsComponent::directorySelector(sim);
                                if (!dir.isEmpty())
                                {
                                    ownModelsComp->requestSimulatorModels(sim, IAircraftModelLoader::InBackgroundNoCache, QStringList(dir));
                                }
                            });
                        }
                        menuActions.addAction(m_reloadActions[6], CMenuAction::pathSimulatorModelsReload());
                        menuActions.addAction(m_reloadActions[7], CMenuAction::pathSimulatorModelsReload());
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

        void CDbOwnModelsComponent::requestOwnModelsUpdate()
        {
            if (!m_modelLoader) { return; }
            ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(
                m_modelLoader->getAircraftModels()
            );
        }

        void CDbOwnModelsComponent::loadInstalledModels(const CSimulatorInfo &simulator, IAircraftModelLoader::LoadMode mode, const QStringList &modelDirectories)
        {
            if (!this->initModelLoader(simulator))
            {
                CLogMessage(this).error("Cannot init model loader for %1") << simulator.toQString();
                return;
            }

            if (m_modelLoader->isLoadingInProgress())
            {
                CLogMessage(this).info("Loading for '%1' already in progress") << simulator.toQString();
                return;
            }

            CLogMessage(this).info("Starting loading for '%1'") << simulator.toQString();
            ui->tvp_OwnAircraftModels->showLoadIndicator();
            Q_ASSERT_X(sGui && sGui->getWebDataServices(), Q_FUNC_INFO, "missing web data services");
            m_modelLoader->startLoading(mode, static_cast<int (*)(CAircraftModelList &, bool)>(&CDatabaseUtils::consolidateModelsWithDbData), modelDirectories);
        }

        void CDbOwnModelsComponent::onOwnModelsLoadingFinished(const CStatusMessageList &statusMessages, const CSimulatorInfo &simulator, IAircraftModelLoader::LoadFinishedInfo info)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Expect single simulator");
            if (statusMessages.isSuccess() && m_modelLoader)
            {
                const CAircraftModelList models(m_modelLoader->getAircraftModels());
                const int modelsLoaded = models.size();
                ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(models);
                if (modelsLoaded < 1)
                {
                    // loading ok, but no data
                    CLogMessage(this).warning("Loading completed, but no models");
                }
            }
            else
            {
                ui->tvp_OwnAircraftModels->clear();
                CLogMessage(this).error("Loading of models failed, simulator '%1'") << simulator.toQString();
            }

            if (statusMessages.hasWarningOrErrorMessages()) {
                this->showOverlayMessages(statusMessages);
            }

            // cache loads may occur in background, do not adjust UI settings
            if (info == IAircraftModelLoader::CacheLoaded) { return; }

            // parsed loads normally explicit
            ui->le_Simulator->setText(simulator.toQString());
            ui->comp_SimulatorSelector->setValue(simulator);
        }

        void CDbOwnModelsComponent::requestSimulatorModels(const CSimulatorInfo &simulator, IAircraftModelLoader::LoadMode mode, const QStringList &modelDirectories)
        {
            this->loadInstalledModels(simulator, mode, modelDirectories);
        }

        void CDbOwnModelsComponent::requestSimulatorModelsWithCacheInBackground(const CSimulatorInfo &simulator)
        {
            this->requestSimulatorModels(simulator, IAircraftModelLoader::InBackgroundWithCache);
        }

        void CDbOwnModelsComponent::onSimulatorChanged()
        {
            const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
            ui->tvp_OwnAircraftModels->setSimulatorForLoading(simulator);
            this->requestSimulatorModelsWithCacheInBackground(simulator);
            ui->le_Simulator->setText(simulator.toQString());
        }
    } // ns
} // ns
