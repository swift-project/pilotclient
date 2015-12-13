/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbmappingcomponent.h"
#include "ui_dbmappingcomponent.h"
#include "blackgui/guiutility.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/project.h"
#include <QShortcut>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackGui;
using namespace BlackGui::Editors;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Components
    {
        CDbMappingComponent::CDbMappingComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
            ui(new Ui::CDbMappingComponent)
        {
            ui->setupUi(this);
            this->ui->comp_StashAircraft->setMappingComponent(this);
            this->ui->tvp_AircraftModelsForVPilot->setAircraftModelMode(CAircraftModelListModel::VPilotRuleModel);
            this->ui->tvp_OwnAircraftModels->setAircraftModelMode(CAircraftModelListModel::OwnSimulatorModelMapping);

            connect(ui->editor_Model, &CModelMappingForm::requestSave, this, &CDbMappingComponent::saveSingleModelToDb);
            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onOwnModelsCountChanged);
            connect(ui->comp_StashAircraft->getView(), &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onStashCountChanged);
            connect(ui->comp_StashAircraft, &CDbStashComponent::stashedModelChanged, this, &CDbMappingComponent::ps_onStashedModelsChanged);

            ui->tvp_OwnAircraftModels->setDisplayAutomatically(true);
            ui->tvp_OwnAircraftModels->setCustomMenu(new CMappingSimulatorModelMenu(this));
            ui->tvp_OwnAircraftModels->setCustomMenu(new CStashMenu(this, true));
            ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(this->m_cachedOwnModels.get());

            // how to display forms
            ui->editor_AircraftIcao->setMappingMode();
            ui->editor_Distributor->setMappingMode();
            ui->editor_Livery->setMappingMode();

            this->ui->tw_ModelsToBeMapped->setTabIcon(TabStash, CIcons::appDbStash16());
            this->ui->tw_ModelsToBeMapped->setTabIcon(TabOwnModels, CIcons::appModels16());

            this->initVPilotLoading();
        }

        CDbMappingComponent::~CDbMappingComponent()
        {
            gracefulShutdown();
        }

        void CDbMappingComponent::initVPilotLoading()
        {
            bool canUseVPilot = CProject::isRunningOnWindowsNtPlatform() && CProject::isCompiledWithMsFlightSimulatorSupport();
            bool withVPilotRights = canUseVPilot && this->m_user.get().isMappingAdmin();
            this->m_withVPilot = withVPilotRights;
            static const QString tabName(this->ui->tw_ModelsToBeMapped->tabText(TabVPliot));

            if (this->m_vPilot1stInit && canUseVPilot)
            {
                ui->tvp_AircraftModelsForVPilot->setCustomMenu(new CStashMenu(this, true));
                connect(ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
                connect(ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onVPilotCountChanged);
                connect(&m_vPilotReader, &CVPilotRulesReader::readFinished, this, &CDbMappingComponent::ps_onLoadVPilotDataFinished);
                this->ui->tvp_AircraftModelsForVPilot->setCustomMenu(new CMappingVPilotMenu(this, true));
                this->ui->tvp_AircraftModelsForVPilot->setDisplayAutomatically(true);
                const CAircraftModelList cachedModels(m_cachedVPilotModels.get());
                this->ui->tvp_AircraftModelsForVPilot->updateContainerMaybeAsync(cachedModels);
                int noModels = cachedModels.size();
                CLogMessage(this).info("%1 cached vPilot models loaded") << noModels;
            }
            this->m_vPilot1stInit = false;
            this->ui->tab_VPilot->setEnabled(withVPilotRights);
            this->ui->tab_VPilot->setVisible(withVPilotRights);
            if (withVPilotRights)
            {
                this->ui->tw_ModelsToBeMapped->addTab(this->ui->tab_VPilot, tabName);
            }
            else
            {
                this->ui->tw_ModelsToBeMapped->removeTab(TabVPliot);
            }
        }

        bool CDbMappingComponent::initModelLoader(const CSimulatorInfo &simInfo)
        {
            // already loaded
            if (this->m_modelLoader && this->m_modelLoader->supportsSimulator(simInfo)) { return true; }

            // unload old
            if (this->m_modelLoader) { this->m_modelLoader->cancelLoading(); }

            this->m_modelLoader = IAircraftModelLoader::createModelLoader(simInfo);
            if (!this->m_modelLoader || !this->m_modelLoader->supportsSimulator(simInfo))
            {
                CLogMessage(this).error("Failed to init model loader %1") << simInfo.toQString();
                this->m_modelLoader.reset();
                return false;
            }
            else
            {
                bool c = connect(this->m_modelLoader.get(), &IAircraftModelLoader::loadingFinished, this, &CDbMappingComponent::ps_onInstalledModelLoadingFinished);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Failed connect for model loader");
                Q_UNUSED(c);
                return true;
            }
        }

        void CDbMappingComponent::setProvider(BlackMisc::Network::IWebDataServicesProvider *provider)
        {
            CWebDataServicesAware::setProvider(provider);
            this->ui->editor_Livery->setProvider(provider);
            this->ui->editor_Distributor->setProvider(provider);
            this->ui->editor_AircraftIcao->setProvider(provider);
            this->ui->comp_StashAircraft->setProvider(provider);
        }

        void CDbMappingComponent::gracefulShutdown()
        {
            this->disconnect();
            CWebDataServicesAware::gracefulShutdown();
            this->m_vPilotReader.gracefulShutdown();
            if (this->m_modelLoader) { this->m_modelLoader->gracefulShutdown(); }
        }

        bool CDbMappingComponent::hasModelsForStash() const
        {
            TabIndex tab = currentTabIndex();
            switch (tab)
            {
            case TabOwnModels:
                return ui->tvp_OwnAircraftModels->hasSelection();
            case TabVPliot:
                return ui->tvp_AircraftModelsForVPilot->hasSelection();
            default:
                break;
            }
            return false;
        }

        CAircraftModelList CDbMappingComponent::getModelsForStash() const
        {
            if (!hasModelsForStash()) { return CAircraftModelList(); }
            TabIndex tab = currentTabIndex();
            switch (tab)
            {
            case TabOwnModels:
                return ui->tvp_OwnAircraftModels->selectedObjects();
            case TabVPliot:
                return ui->tvp_AircraftModelsForVPilot->selectedObjects();
            default:
                break;
            }
            return CAircraftModelList();
        }

        CDbMappingComponent::TabIndex CDbMappingComponent::currentTabIndex() const
        {
            int t = ui->tw_ModelsToBeMapped->currentIndex();
            return static_cast<TabIndex>(t);
        }

        CStatusMessageList CDbMappingComponent::validate(bool withNestedForms) const
        {
            CStatusMessageList msgs(this->ui->editor_Model->validate(!withNestedForms));
            if (withNestedForms)
            {
                msgs.push_back(ui->editor_AircraftIcao->validate());
                msgs.push_back(ui->editor_Livery->validate(withNestedForms));
                msgs.push_back(ui->editor_Distributor->validate());
            }
            return msgs;
        }

        void CDbMappingComponent::saveSingleModelToDb()
        {
            CStatusMessageList msgs(validate(true));
            if (msgs.hasErrorMessages())
            {
                CLogMessage::preformatted(msgs);
                this->showMessages(msgs);
                return;
            }

            CAircraftModel model(getAircraftModel());
            msgs = this->asyncWriteModelToDb(model);
            if (!msgs.isEmpty())
            {
                CLogMessage(this).preformatted(msgs);
            }
        }

        void CDbMappingComponent::resizeForSelect()
        {
            int h = this->height();
            int h1 = h / 3 * 2;
            int h2 = h / 3;
            QList<int> sizes({h1, h2});
            this->ui->sp_MappingComponent->setSizes(sizes);
        }

        void CDbMappingComponent::resizeForMapping()
        {
            int h = this->height(); // total height
            int h2 = ui->qw_EditorsScrollArea->minimumHeight();
            h2 *= 1.10; // desired height of inner widget + some space for scrollarea
            int currentSize = ui->sp_MappingComponent->sizes().last(); // current size
            if (h2 <= currentSize) { return; }

            int h1;
            if (h * 0.90 > h2)
            {
                // enough space to display as whole
                h1 = h - h2;
            }
            else
            {
                h1 = h / 3;
                h2 = h / 3 * 2;
            }
            QList<int> sizes({h1, h2});
            this->ui->sp_MappingComponent->setSizes(sizes);
        }

        void CDbMappingComponent::ps_loadVPilotData()
        {
            if (this->m_vPilotReader.readInBackground(true))
            {
                CLogMessage(this).info("Start loading vPilot rulesets");
                this->ui->tvp_AircraftModelsForVPilot->showLoadIndicator();
            }
            else
            {
                CLogMessage(this).warning("Loading vPilot rulesets already in progress");
            }
        }

        void CDbMappingComponent::ps_onLoadVPilotDataFinished(bool success)
        {
            if (!m_withVPilot) { return; }
            if (success)
            {
                CLogMessage(this).info("Loading vPilot ruleset completed");
                const CAircraftModelList models(this->m_vPilotReader.getAsModels());
                if (this->ui->tvp_AircraftModelsForVPilot->displayAutomatically())
                {
                    this->ui->tvp_AircraftModelsForVPilot->updateContainerMaybeAsync(models);
                }
                CStatusMessage msg = m_cachedVPilotModels.set(models);
                if (msg.isWarningOrAbove())
                {
                    CLogMessage(this).preformatted(msg);
                }
                else
                {
                    CLogMessage(this).info("Written %1 vPilot rules to cache") << models.size();
                }
            }
            else
            {
                CLogMessage(this).error("Loading vPilot ruleset failed");
            }
            this->ui->tvp_OwnAircraftModels->hideLoadIndicator();
        }

        void CDbMappingComponent::ps_onStashedModelsChanged()
        {
            bool hlvp = this->ui->tvp_AircraftModelsForVPilot->derivedModel()->highlightGivenModelStrings();
            bool hlom = this->ui->tvp_OwnAircraftModels->derivedModel()->highlightGivenModelStrings();
            bool highlight =  hlom || hlvp;
            if (!highlight) { return; }
            const QStringList stashedModels(this->ui->comp_StashAircraft->getStashedModelStrings());
            if (hlvp)
            {
                this->ui->tvp_AircraftModelsForVPilot->derivedModel()->setHighlightModelsStrings(stashedModels);
            }
            if (hlom)
            {
                this->ui->tvp_OwnAircraftModels->derivedModel()->setHighlightModelsStrings(stashedModels);
            }
        }

        void CDbMappingComponent::ps_onVPilotCountChanged(int count, bool withFilter)
        {
            if (!m_withVPilot) { return; }
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_VPilot);
            QString o = this->ui->tw_ModelsToBeMapped->tabText(i);
            QString f = this->ui->tvp_AircraftModelsForVPilot->derivedModel()->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, this->ui->tvp_AircraftModelsForVPilot->rowCount()) + f;
            this->ui->tw_ModelsToBeMapped->setTabText(i, o);
        }

        void CDbMappingComponent::ps_onOwnModelsCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_OwnModels);
            QString o = this->ui->tw_ModelsToBeMapped->tabText(i);
            if (this->m_modelLoader)
            {
                QString sims(this->m_modelLoader->supportedSimulatorsAsString());
                if (!sims.isEmpty()) { o = o.append(" ").append(sims); }
            }
            QString f = this->ui->tvp_OwnAircraftModels->derivedModel()->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, this->ui->tvp_AircraftModelsForVPilot->rowCount()) + f;
            this->ui->tw_ModelsToBeMapped->setTabText(i, o);
        }

        void CDbMappingComponent::ps_onStashCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_StashAircraftModels);
            QString o = this->ui->tw_ModelsToBeMapped->tabText(i);
            QString f = this->ui->comp_StashAircraft->getView()->derivedModel()->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, this->ui->comp_StashAircraft->getView()->rowCount()) + f;
            this->ui->tw_ModelsToBeMapped->setTabText(i, o);
        }

        void CDbMappingComponent::ps_requestSimulatorModels()
        {
            QAction *a = qobject_cast<QAction *>(QObject::sender());
            if (!a) { return; }
            int f = a->data().toInt();
            CSimulatorInfo sim(f);
            this->ps_loadInstalledModels(sim);
        }

        void CDbMappingComponent::ps_userChanged()
        {
            this->initVPilotLoading();
        }

        void CDbMappingComponent::stashSelectedModels()
        {
            if (!this->hasModelsForStash()) { return; }
            CStatusMessageList msgs =
                this->ui->comp_StashAircraft->stashModels(
                    this->getModelsForStash()
                );
            if (msgs.hasWarningOrErrorMessages())
            {
                // maybe log, popup?
            }
        }

        void CDbMappingComponent::ps_onModelRowSelected(const QModelIndex &index)
        {
            QObject *sender = QObject::sender();
            CAircraftModel model;
            if (sender == this->ui->tvp_AircraftModelsForVPilot)
            {
                model = this->ui->tvp_AircraftModelsForVPilot->at(index);
            }
            else if (sender == this->ui->tvp_OwnAircraftModels)
            {
                model = this->ui->tvp_OwnAircraftModels->at(index);
            }
            else
            {
                return;
            }
            this->ui->editor_Model->setValue(model);

            const CLivery livery(this->smartLiverySelector(model.getLivery()));
            const CAircraftIcaoCode aircraftIcao(this->smartAircraftIcaoSelector(model.getAircraftIcaoCode()));
            const CDistributor distributor(this->smartDistributorSelector(model.getDistributor()));

            // if found, then set in editor
            if (livery.hasValidDbKey()) { this->ui->editor_Livery->setValue(livery); }
            else { this->ui->editor_Livery->clear(); }
            if (aircraftIcao.hasValidDbKey()) { this->ui->editor_AircraftIcao->setValue(aircraftIcao); }
            else { this->ui->editor_AircraftIcao->clear(); }
            if (distributor.hasValidDbKey()) { this->ui->editor_Distributor->setValue(distributor); }
            else { this->ui->editor_Distributor->clear(); }

            // request filtering
            emit filterByLivery(model.getLivery());
            emit filterByAircraftIcao(model.getAircraftIcaoCode());
            emit filterByDistributor(model.getDistributor());
        }

        void CDbMappingComponent::ps_loadInstalledModels(const CSimulatorInfo &simInfo)
        {
            //! \todo, load correct loader
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
            this->m_modelLoader->startLoading();
        }

        void CDbMappingComponent::ps_onInstalledModelLoadingFinished(bool success)
        {
            if (success && this->m_modelLoader)
            {
                const CAircraftModelList models(this->m_modelLoader->getAircraftModels());
                CLogMessage(this).info("Loading %1 of models completed") << models.size();
                if (this->ui->tvp_OwnAircraftModels->displayAutomatically())
                {
                    this->ui->tvp_OwnAircraftModels->updateContainer(models);
                }
                this->m_cachedOwnModels.set(models);
                CLogMessage(this).info("Written %1 own models to cache") << models.size();
            }
            else
            {
                CLogMessage(this).error("Loading of models failed, simulator");
                this->ui->tvp_OwnAircraftModels->hideLoadIndicator();
            }
            this->ui->tvp_OwnAircraftModels->hideLoadIndicator();
        }

        CAircraftModel CDbMappingComponent::getAircraftModel() const
        {
            CAircraftModel model(ui->editor_Model->getValue());
            model.setDistributor(ui->editor_Distributor->getValue());
            model.setAircraftIcaoCode(ui->editor_AircraftIcao->getValue());
            model.setLivery(ui->editor_Livery->getValue());
            return model;
        }

        void CDbMappingComponent::CMappingSimulatorModelMenu::customMenu(QMenu &menu) const
        {
            CSimulatorInfo sims = CSimulatorInfo::getLocallyInstalledSimulators();
            bool empty = sims.isNoSimulator() || sims.isUnspecified();
            if (!empty)
            {
                QMenu *load = menu.addMenu(CIcons::appModels16(), "Load installed models");
                QAction *a = nullptr;
                CDbMappingComponent *mapComp = qobject_cast<CDbMappingComponent *>(this->parent());
                Q_ASSERT_X(mapComp, Q_FUNC_INFO, "Cannot access parent");

                if (sims.fs9())
                {
                    a = load->addAction(CIcons::appModels16(), "FS9 models", mapComp, SLOT(ps_requestSimulatorModels()));
                    a->setData(QVariant(static_cast<int>(CSimulatorInfo::FS9)));
                }
                if (sims.fsx())
                {
                    a = load->addAction(CIcons::appModels16(), "FSX models", mapComp, SLOT(ps_requestSimulatorModels()));
                    a->setData(QVariant(static_cast<int>(CSimulatorInfo::FSX)));
                }
                if (sims.p3d())
                {
                    a = load->addAction(CIcons::appModels16(), "P3D models", mapComp, SLOT(ps_requestSimulatorModels()));
                    a->setData(QVariant(static_cast<int>(CSimulatorInfo::P3D)));
                }
                if (sims.xplane())
                {
                    a = load->addAction(CIcons::appModels16(), "XPlane models", mapComp, SLOT(ps_requestSimulatorModels()));
                    a->setData(QVariant(static_cast<int>(CSimulatorInfo::XPLANE)));
                }
                menu.addSeparator();
            }
            this->nestedCustomMenu(menu);
        }

        void CDbMappingComponent::CMappingVPilotMenu::customMenu(QMenu &menu) const
        {
            CDbMappingComponent *mapComp = qobject_cast<CDbMappingComponent *>(this->parent());
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "Cannot access mapping component");

            bool canUseVPilot = mappingComponent()->withVPilot();
            if (canUseVPilot)
            {
                menu.addAction(CIcons::appMappings16(), "Load vPilot Rules", mapComp, SLOT(ps_loadVPilotData()));
                menu.addSeparator();
            }
            this->nestedCustomMenu(menu);
        }

        CDbMappingComponent *CDbMappingComponent::CMappingVPilotMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

        void CDbMappingComponent::CStashMenu::customMenu(QMenu &menu) const
        {
            CDbMappingComponent *mapComp = qobject_cast<CDbMappingComponent *>(this->parent());
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "Cannot access mapping component");
            if (mapComp->hasModelsForStash())
            {
                QAction *a = menu.addAction(CIcons::appMappings16(), "Stash", mapComp, SLOT(stashSelectedModels()), Qt::ALT + Qt::Key_S);
                a->setShortcut(Qt::ALT + Qt::Key_S);

            }
            this->nestedCustomMenu(menu);
        }

        CDbMappingComponent *CDbMappingComponent::CStashMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }
    } // ns
} // ns
