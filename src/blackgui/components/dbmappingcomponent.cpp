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
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/project.h"
#include "blackgui/guiutility.h"

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
            this->ui->tvp_AircraftModelsForVPilot->setAircraftModelMode(CAircraftModelListModel::VPilotRuleModel);
            connect(ui->editor_Model, &CModelMappingForm::requestSave, this, &CDbMappingComponent::save);
            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onOwnModelsCountChanged);

            ui->tvp_OwnAircraftModels->setCustomMenu(new CMappingSimulatorModelMenu(this));
            ui->tvp_OwnAircraftModels->setDisplayAutomatically(true);

            ui->editor_AircraftIcao->setMappingMode();
            ui->editor_Distributor->setMappingMode();
            ui->editor_Livery->setMappingMode();

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
            static const QString tabName(this->ui->tw_ModelsToBeMapped->tabText(1));

            if (this->m_vPilot1stInit && canUseVPilot)
            {
                connect(ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
                connect(ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onVPilotCountChanged);
                connect(&m_vPilotReader, &CVPilotRulesReader::readFinished, this, &CDbMappingComponent::ps_onLoadVPilotDataFinished);
                this->ui->tvp_AircraftModelsForVPilot->setCustomMenu(new CMappingVPilotMenu(this));
                this->ui->tvp_AircraftModelsForVPilot->setDisplayAutomatically(true);
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
                this->ui->tw_ModelsToBeMapped->removeTab(1);
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
        }

        void CDbMappingComponent::gracefulShutdown()
        {
            this->disconnect();
            CWebDataServicesAware::gracefulShutdown();
            this->m_vPilotReader.gracefulShutdown();
            if (this->m_modelLoader) { this->m_modelLoader->gracefulShutdown(); }
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

        void CDbMappingComponent::save()
        {
            CStatusMessageList msgs(validate(true));
            if (msgs.hasErrorMessages())
            {
                CLogMessage::preformatted(msgs);
                this->showMessages(msgs);
                return;
            }

            CAircraftModel model(getAircraftModel());
            msgs = this->asyncWriteModel(model);
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
                if (this->ui->tvp_AircraftModelsForVPilot->displayAutomatically())
                {
                    this->ui->tvp_AircraftModelsForVPilot->updateContainerMaybeAsync(
                        this->m_vPilotReader.getAsModels()
                    );
                }
            }
            else
            {
                CLogMessage(this).error("Loading vPilot ruleset failed");
            }
            this->ui->tvp_OwnAircraftModels->hideLoadIndicator();
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
                CLogMessage(this).info("Loading of models completed");
                if (this->ui->tvp_OwnAircraftModels->displayAutomatically())
                {
                    this->ui->tvp_OwnAircraftModels->updateContainer(
                        this->m_modelLoader->getAircraftModels()
                    );
                }
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

        void CDbMappingComponent::CMappingVPilotMenu::customMenu(QMenu &menu) const
        {
            CDbMappingComponent *mapComp = qobject_cast<CDbMappingComponent *>(this->parent());
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "Cannot access parent");

            bool canUseVPilot = mappingComponent()->withVPilot();
            if (canUseVPilot)
            {
                menu.addAction(CIcons::appMappings16(), "Load vPilot Rules", mapComp, SLOT(ps_loadVPilotData()));
                menu.addSeparator();
            }
        }

        CDbMappingComponent *CDbMappingComponent::CMappingVPilotMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

    } // ns
} // ns
