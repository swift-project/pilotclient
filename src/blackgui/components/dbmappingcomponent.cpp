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
#include "blackgui/components/dbautostashingcomponent.h"
#include "blackgui/components/dbmodelmappingmodifycomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/shortcut.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/logmessage.h"
#include <QFile>
#include <QShortcut>
#include <QDesktopServices>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
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
            ui(new Ui::CDbMappingComponent),
            m_autoStashDialog(new CDbAutoStashingComponent(this)),
            m_modelModifyDialog(new CDbModelMappingModifyComponent(this))
        {
            ui->setupUi(this);
            this->ui->comp_StashAircraft->setMappingComponent(this);

            this->ui->tvp_AircraftModelsForVPilot->setAircraftModelMode(CAircraftModelListModel::VPilotRuleModel);
            this->ui->tvp_AircraftModelsForVPilot->addFilterDialog();
            this->ui->tvp_OwnAircraftModels->setAircraftModelMode(CAircraftModelListModel::OwnSimulatorModelMapping);
            this->ui->tvp_OwnAircraftModels->addFilterDialog();

            // connects
            connect(ui->editor_Model, &CModelMappingForm::requestStash, this, &CDbMappingComponent::ps_stashCurrentModel);

            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onOwnModelsCountChanged);
            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::requestStash, this, &CDbMappingComponent::stashSelectedModels);
            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::toggledHighlightStashedModels, this, &CDbMappingComponent::ps_onStashedModelsChanged);
            connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::requestUpdate, this, &CDbMappingComponent::ps_requestOwnModelsUpdate);

            connect(ui->comp_StashAircraft->getView(), &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onStashCountChanged);
            connect(ui->comp_StashAircraft->getView(), &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
            connect(ui->comp_StashAircraft->getView(), &CAircraftModelView::requestHandlingOfStashDrop, this, &CDbMappingComponent::ps_handleStashDropRequest);
            connect(ui->comp_StashAircraft, &CDbStashComponent::stashedModelsChanged, this, &CDbMappingComponent::ps_onStashedModelsChanged);
            connect(ui->comp_StashAircraft, &CDbStashComponent::modelsSuccessfullyPublished, this, &CDbMappingComponent::ps_onModelsSuccessfullyPublished);

            ui->tvp_OwnAircraftModels->setDisplayAutomatically(true);
            ui->tvp_OwnAircraftModels->setCustomMenu(new CShowSimulatorFile(this), false);
            ui->tvp_OwnAircraftModels->setCustomMenu(new CMappingOwnSimulatorModelMenu(this));
            ui->tvp_OwnAircraftModels->setCustomMenu(new CModelStashTools(this, false));
            ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(this->m_cachedOwnModels.get());

            // how to display forms
            ui->editor_AircraftIcao->setSelectOnly();
            ui->editor_Distributor->setSelectOnly();
            ui->editor_Livery->setSelectOnly();

            this->ui->tw_ModelsToBeMapped->setTabIcon(TabStash, CIcons::appDbStash16());
            this->ui->tw_ModelsToBeMapped->setTabIcon(TabOwnModels, CIcons::appModels16());
            this->ui->comp_StashAircraft->getView()->setCustomMenu(new CApplyDbDataMenu(this));

            // vPilot
            this->initVPilotLoading();
        }

        CDbMappingComponent::~CDbMappingComponent()
        {
            gracefulShutdown();
        }

        void CDbMappingComponent::initVPilotLoading()
        {
            bool canUseVPilot = true; // general flag if vPilot can be used/not used
            this->m_withVPilot = canUseVPilot && this->m_swiftDbUser.get().isMappingAdmin();
            static const QString tabName(this->ui->tw_ModelsToBeMapped->tabText(TabVPilot));

            if (this->m_vPilot1stInit && canUseVPilot)
            {
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onVPilotCountChanged);
                connect(&m_vPilotReader, &CVPilotRulesReader::readFinished, this, &CDbMappingComponent::ps_onLoadVPilotDataFinished);
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::requestStash, this, &CDbMappingComponent::stashSelectedModels);
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::toggledHighlightStashedModels, this, &CDbMappingComponent::ps_onStashedModelsChanged);
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::requestUpdate, this, &CDbMappingComponent::ps_requestVPilotDataUpdate);

                this->ui->tvp_AircraftModelsForVPilot->setCustomMenu(new CMappingVPilotMenu(this, true));
                this->ui->tvp_AircraftModelsForVPilot->setCustomMenu(new CModelStashTools(this, false));
                this->ui->tvp_AircraftModelsForVPilot->setDisplayAutomatically(true);
                this->ui->tvp_AircraftModelsForVPilot->addFilterDialog();
                const CAircraftModelList cachedModels(m_cachedVPilotModels.get());
                this->ui->tvp_AircraftModelsForVPilot->updateContainerMaybeAsync(cachedModels);
                int noModels = cachedModels.size();
                CLogMessage(this).info("%1 cached vPilot models loaded") << noModels;
            }
            this->m_vPilot1stInit = false;
            this->ui->tab_VPilot->setEnabled(this->m_withVPilot);
            this->ui->tab_VPilot->setVisible(this->m_withVPilot);
            if (this->m_withVPilot)
            {
                // create / restore tab
                this->ui->tw_ModelsToBeMapped->addTab(this->ui->tab_VPilot, tabName);
                this->ps_onVPilotCountChanged(
                    this->ui->tvp_AircraftModelsForVPilot->rowCount(),
                    this->ui->tvp_AircraftModelsForVPilot->hasFilter());
            }
            else
            {
                this->ui->tw_ModelsToBeMapped->removeTab(TabVPilot);
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
                bool c = connect(this->m_modelLoader.get(), &IAircraftModelLoader::loadingFinished, this, &CDbMappingComponent::ps_onOwnModelsLoadingFinished);
                Q_ASSERT_X(c, Q_FUNC_INFO, "Failed connect for model loader");
                Q_UNUSED(c);
                return true;
            }
        }

        CAircraftModel CDbMappingComponent::getModelFromView(const QModelIndex &index) const
        {
            if (!index.isValid()) { return CAircraftModel(); }
            const QObject *sender = QObject::sender();
            if (sender == this->ui->tvp_AircraftModelsForVPilot)
            {
                return this->ui->tvp_AircraftModelsForVPilot->at(index);
            }
            else if (sender == this->ui->tvp_OwnAircraftModels)
            {
                return this->ui->tvp_OwnAircraftModels->at(index);
            }
            else if (sender == this->ui->comp_StashAircraft || sender == this->ui->comp_StashAircraft->getView())
            {
                return this->ui->comp_StashAircraft->getView()->at(index);
            }

            // no sender, use current tab
            const CAircraftModelView *v = this->currentModelView();
            if (!v) { return CAircraftModel(); }
            return v->at(index);
        }

        void CDbMappingComponent::gracefulShutdown()
        {
            this->disconnect();
            this->m_vPilotReader.gracefulShutdown();
            if (this->m_modelLoader) { this->m_modelLoader->gracefulShutdown(); }
        }

        bool CDbMappingComponent::hasSelectedModelsToStash() const
        {
            TabIndex tab = currentTabIndex();
            switch (tab)
            {
            case TabOwnModels:
                return ui->tvp_OwnAircraftModels->hasSelectedModelsToStash();
            case TabVPilot:
                return ui->tvp_AircraftModelsForVPilot->hasSelectedModelsToStash();
            default:
                break;
            }
            return false;
        }

        const CAircraftModelView *CDbMappingComponent::currentModelView() const
        {
            TabIndex tab = currentTabIndex();
            switch (tab)
            {
            case TabOwnModels:
                return ui->tvp_OwnAircraftModels;
            case TabVPilot:
                return ui->tvp_AircraftModelsForVPilot;
            case TabStash:
                return ui->comp_StashAircraft->getView();
            default:
                return nullptr;
            }
        }

        CAircraftModelView *CDbMappingComponent::currentModelView()
        {
            TabIndex tab = currentTabIndex();
            switch (tab)
            {
            case TabOwnModels:
                return ui->tvp_OwnAircraftModels;
            case TabVPilot:
                return ui->tvp_AircraftModelsForVPilot;
            case TabStash:
                return ui->comp_StashAircraft->getView();
            default:
                return nullptr;
            }
        }

        QString CDbMappingComponent::currentTabText() const
        {
            int i = this->ui->tw_ModelsToBeMapped->currentIndex();
            return this->ui->tw_ModelsToBeMapped->tabText(i);
        }

        CAircraftModelList CDbMappingComponent::getSelectedModelsToStash() const
        {
            if (!hasSelectedModelsToStash()) { return CAircraftModelList(); }
            TabIndex tab = currentTabIndex();
            switch (tab)
            {
            case TabOwnModels:
                return ui->tvp_OwnAircraftModels->selectedObjects();
            case TabVPilot:
                return ui->tvp_AircraftModelsForVPilot->selectedObjects();
            default:
                break;
            }
            return CAircraftModelList();
        }

        const CAircraftModelList &CDbMappingComponent::getStashedModels() const
        {
            return ui->comp_StashAircraft->getStashedModels();
        }

        QStringList CDbMappingComponent::getStashedModelStrings() const
        {
            return ui->comp_StashAircraft->getStashedModelStrings();
        }

        CAircraftModel CDbMappingComponent::getOwnModelForModelString(const QString &modelString) const
        {
            return m_cachedOwnModels.get().findFirstByModelString(modelString);
        }

        CDbMappingComponent::TabIndex CDbMappingComponent::currentTabIndex() const
        {
            if (!ui->tw_ModelsToBeMapped) { return CDbMappingComponent::NoValidTab; }
            int t = ui->tw_ModelsToBeMapped->currentIndex();
            return static_cast<TabIndex>(t);
        }

        bool CDbMappingComponent::isStashedTab() const
        {
            return currentTabIndex() == TabStash;
        }

        CStatusMessageList CDbMappingComponent::validateCurrentModel(bool withNestedForms) const
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

        void CDbMappingComponent::ps_handleStashDropRequest(const CAirlineIcaoCode &code) const
        {
            CLivery stdLivery(sGui->getWebDataServices()->getStdLiveryForAirlineCode(code));
            if (!stdLivery.hasValidDbKey()) { return; }
            this->ui->comp_StashAircraft->applyToSelected(stdLivery);
        }

        void CDbMappingComponent::ps_stashCurrentModel()
        {
            const CAircraftModel model(getEditorAircraftModel());
            CStatusMessageList msgs(this->validateCurrentModel(true));
            if (!msgs.hasErrorMessages())
            {
                msgs.push_back(
                    this->ui->comp_StashAircraft->stashModel(model, true)
                );
            }
            if (msgs.hasErrorMessages())
            {
                this->showOverlayMessages(msgs);
            }
        }

        void CDbMappingComponent::ps_displayAutoStashingDialog()
        {
            this->m_autoStashDialog->exec();
        }

        void CDbMappingComponent::ps_removeDbModelsFromView()
        {
            QStringList modelStrings(sGui->getWebDataServices()->getModelStrings());
            if (modelStrings.isEmpty()) { return; }
            if (currentTabIndex() == TabVPilot || currentTabIndex() == TabOwnModels)
            {
                this->currentModelView()->removeModelsWithModelString(modelStrings);
            }
        }

        void CDbMappingComponent::ps_toggleAutoFiltering()
        {
            this->m_autoFilterInDbViews = !this->m_autoFilterInDbViews;
        }

        void CDbMappingComponent::ps_applyDbData()
        {
            QAction *sender = qobject_cast<QAction *>(this->sender());
            if (!sender) { return; }
            QString cn(sender->data().toString());
            if (cn.isEmpty()) { return; }
            if (this->ui->comp_StashAircraft->getView()->selectedRowCount() < 1) { return; }

            CStatusMessageList msgs;
            if (CLivery().getClassName() == cn)
            {
                msgs = this->ui->editor_Livery->validate(true);
                if (!msgs.hasErrorMessages())
                {
                    this->ui->comp_StashAircraft->applyToSelected(this->ui->editor_Livery->getValue());
                }
            }
            else if (CDistributor().getClassName() == cn)
            {
                msgs = this->ui->editor_Distributor->validate();
                if (!msgs.hasErrorMessages())
                {
                    this->ui->comp_StashAircraft->applyToSelected(this->ui->editor_Distributor->getValue());
                }
            }
            else if (CAircraftIcaoCode().getClassName() == cn)
            {
                msgs = this->ui->editor_AircraftIcao->validate();
                if (!msgs.hasErrorMessages())
                {
                    this->ui->comp_StashAircraft->applyToSelected(this->ui->editor_AircraftIcao->getValue());
                }
            }
            else if (CAirlineIcaoCode().getClassName() == cn)
            {
                msgs = this->ui->editor_Livery->validateAirlineIcao();
                if (!msgs.hasErrorMessages())
                {
                    this->ui->comp_StashAircraft->applyToSelected(this->ui->editor_Livery->getValueAirlineIcao());
                }
            }

            // errors if any
            if (msgs.hasErrorMessages())
            {
                this->showOverlayMessages(msgs);
            }
        }

        void CDbMappingComponent::ps_modifyModelDialog()
        {
            // only one model selected, use as default
            if (this->ui->comp_StashAircraft->getView()->hasSingleSelectedRow())
            {
                this->m_modelModifyDialog->setValue(this->ui->comp_StashAircraft->getView()->selectedObject());
            }

            QDialog::DialogCode s = static_cast<QDialog::DialogCode>(this->m_modelModifyDialog->exec());
            if (s == QDialog::Rejected) { return; }
            CPropertyIndexVariantMap vm = this->m_modelModifyDialog->getValues();
            this->ui->comp_StashAircraft->applyToSelected(vm);
        }

        void CDbMappingComponent::ps_showSimulatorFile()
        {
            if (!this->currentModelView()->hasSelection()) { return; }
            const CAircraftModelList models(this->currentModelView()->selectedObjects().findWithFileName());
            if (models.isEmpty()) { return; }
            int trails = 0;

            for (const CAircraftModel &model : models)
            {
                trails++;
                if (QFile::exists(model.getFileName()))
                {
                    const QString url("file:///" + model.getFileName());
                    QDesktopServices::openUrl(QUrl(url));
                    break;
                }
                if (trails > 10) { break; }
            }
        }

        void CDbMappingComponent::resizeForSelect()
        {
            this->maxTableView();
        }

        void CDbMappingComponent::resizeForMapping()
        {
            const int h = this->height(); // total height
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
            const QList<int> sizes({h1, h2});
            this->ui->sp_MappingComponent->setSizes(sizes);
        }

        void CDbMappingComponent::maxTableView()
        {
            const int h = this->height();
            int h1 = h;
            int h2 = 0;
            const QList<int> sizes({h1, h2});
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
                    CLogMessage::preformatted(msg);
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

        void CDbMappingComponent::ps_onVPilotCacheChanged()
        {
            if (this->ui->tvp_AircraftModelsForVPilot->displayAutomatically())
            {
                this->ui->tvp_AircraftModelsForVPilot->updateContainerMaybeAsync(this->m_cachedVPilotModels.get());
            }
        }

        void CDbMappingComponent::ps_requestVPilotDataUpdate()
        {
            this->ps_onVPilotCacheChanged();
        }

        void CDbMappingComponent::ps_requestOwnModelsUpdate()
        {
            this->ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(
                this->m_cachedOwnModels.get()
            );
        }

        void CDbMappingComponent::ps_onStashedModelsChanged()
        {
            bool hlvp = this->ui->tvp_AircraftModelsForVPilot->derivedModel()->highlightModelStrings();
            bool hlom = this->ui->tvp_OwnAircraftModels->derivedModel()->highlightModelStrings();
            bool highlight =  hlom || hlvp;
            if (!highlight) { return; }
            const QStringList stashedModels(this->ui->comp_StashAircraft->getStashedModelStrings());
            if (hlvp)
            {
                this->ui->tvp_AircraftModelsForVPilot->derivedModel()->setHighlightModelStrings(stashedModels);
            }
            if (hlom)
            {
                this->ui->tvp_OwnAircraftModels->derivedModel()->setHighlightModelStrings(stashedModels);
            }
        }

        void CDbMappingComponent::ps_onModelsSuccessfullyPublished(const CAircraftModelList &models)
        {
            if (models.isEmpty()) { return; }
            emit this->requestUpdatedData(CEntityFlags::ModelEntity);
        }

        void CDbMappingComponent::ps_onVPilotCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_VPilot);
            QString o = this->ui->tw_ModelsToBeMapped->tabText(i);
            QString f = this->ui->tvp_AircraftModelsForVPilot->hasFilter() ? "F" : "";
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
            QString f = this->ui->tvp_OwnAircraftModels->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, this->ui->tvp_OwnAircraftModels->rowCount()) + f;
            this->ui->tw_ModelsToBeMapped->setTabText(i, o);
        }

        void CDbMappingComponent::ps_onStashCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_StashAircraftModels);
            QString o = this->ui->tw_ModelsToBeMapped->tabText(i);
            QString f = this->ui->comp_StashAircraft->getView()->hasFilter() ? "F" : "";
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
            if (!this->hasSelectedModelsToStash()) { return; }
            CStatusMessageList msgs =
                this->ui->comp_StashAircraft->stashModels(
                    this->getSelectedModelsToStash()
                );
            if (msgs.hasWarningOrErrorMessages())
            {
                this->showOverlayMessages(msgs);
            }
        }

        void CDbMappingComponent::ps_onModelRowSelected(const QModelIndex &index)
        {
            CAircraftModel model(this->getModelFromView(index)); // data from view
            if (!model.hasModelString()) { return; }

            // we either use the model, or try to resolve the data to DB data
            bool dbModel = model.hasValidDbKey();
            const CLivery livery(dbModel ? model.getLivery() : sGui->getWebDataServices()->smartLiverySelector(model.getLivery()));
            const CAircraftIcaoCode aircraftIcao(dbModel ? model.getAircraftIcaoCode() : sGui->getWebDataServices()->smartAircraftIcaoSelector(model.getAircraftIcaoCode()));
            const CDistributor distributor(dbModel ? model.getDistributor() : sGui->getWebDataServices()->smartDistributorSelector(model.getDistributor()));

            // set model part
            this->ui->editor_Model->setValue(model);

            // if found, then set in editor
            if (livery.hasValidDbKey())
            {
                this->ui->editor_Livery->setValue(livery);
            }
            else
            {
                this->ui->editor_Livery->clear();
            }
            if (aircraftIcao.hasValidDbKey())
            {
                this->ui->editor_AircraftIcao->setValue(aircraftIcao);
            }
            else
            {
                this->ui->editor_AircraftIcao->clear();
            }
            if (distributor.hasValidDbKey())
            {
                this->ui->editor_Distributor->setValue(distributor);
            }
            else
            {
                this->ui->editor_Distributor->clear();
            }

            // request filtering
            if (this->m_autoFilterInDbViews)
            {
                emit filterByLivery(model.getLivery());
                emit filterByAircraftIcao(model.getAircraftIcaoCode());
                emit filterByDistributor(model.getDistributor());
            }
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

        void CDbMappingComponent::ps_onOwnModelsLoadingFinished(bool success)
        {
            if (success && this->m_modelLoader)
            {
                const CAircraftModelList models(this->m_modelLoader->getAircraftModels());
                CLogMessage(this).info("Loading %1 of models completed") << models.size();
                this->m_cachedOwnModels.set(models);
                CLogMessage(this).info("Written %1 own models to cache") << models.size();

                // when the cache writting is done the view vill be updated in the
                // cache changed slot
            }
            else
            {
                CLogMessage(this).error("Loading of models failed, simulator");
                this->ui->tvp_OwnAircraftModels->hideLoadIndicator();
            }
            this->ui->tvp_OwnAircraftModels->hideLoadIndicator();
        }

        void CDbMappingComponent::ps_ownModelsCacheChanged()
        {
            if (this->ui->tvp_OwnAircraftModels->displayAutomatically())
            {
                this->ui->tvp_OwnAircraftModels->updateContainer(this->m_cachedOwnModels.get());
            }
        }

        CAircraftModel CDbMappingComponent::getEditorAircraftModel() const
        {
            CAircraftModel model(ui->editor_Model->getValue());
            model.setDistributor(ui->editor_Distributor->getValue());
            model.setAircraftIcaoCode(ui->editor_AircraftIcao->getValue());
            model.setLivery(ui->editor_Livery->getValue());
            return model;
        }

        CStatusMessage CDbMappingComponent::stashModel(const CAircraftModel &model, bool replace)
        {
            return this->ui->comp_StashAircraft->stashModel(model, replace);
        }

        CStatusMessageList CDbMappingComponent::stashModels(const CAircraftModelList &models)
        {
            return this->ui->comp_StashAircraft->stashModels(models);
        }

        CAircraftModel CDbMappingComponent::consolidateModel(const CAircraftModel &model) const
        {
            return this->ui->comp_StashAircraft->consolidateModel(model);
        }

        void CDbMappingComponent::replaceStashedModelsUnvalidated(const CAircraftModelList &models) const
        {
            this->ui->comp_StashAircraft->replaceModelsUnvalidated(models);
        }

        void CDbMappingComponent::CMappingOwnSimulatorModelMenu::customMenu(QMenu &menu) const
        {
            CSimulatorInfo sims = CSimulatorInfo::getLocallyInstalledSimulators();
            bool noSims = sims.isNoSimulator() || sims.isUnspecified();
            if (!noSims)
            {
                this->addSeparator(menu);
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
                this->addSeparator(menu);
                menu.addAction(CIcons::appMappings16(), "Load vPilot Rules", mapComp, SLOT(ps_loadVPilotData()));
            }
            this->nestedCustomMenu(menu);
        }

        CDbMappingComponent *CDbMappingComponent::CMappingVPilotMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

        void CDbMappingComponent::CModelStashTools::customMenu(QMenu &menu) const
        {
            CDbMappingComponent *mapComp = mappingComponent();
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "no mapping component");
            bool canConnectDb = sGui->getWebDataServices()->canConnectSwiftDb();
            if (canConnectDb)
            {
                if (!mapComp->currentModelView()->isEmpty() && mapComp->currentModelView()->getMenu().testFlag(CViewBaseNonTemplate::MenuCanStashModels))
                {
                    this->addSeparator(menu);
                    int dbModels = sGui->getWebDataServices()->getModelsCount();
                    if (dbModels > 0)
                    {
                        // we have keys and data where we could delete them from
                        const QString msgDelete("Delete " + QString::number(dbModels) + " DB model(s) from " + mapComp->currentTabText());
                        menu.addAction(CIcons::delete16(), msgDelete, mapComp, SLOT(ps_removeDbModelsFromView()));
                    }

                    // we have keys and data where we could delete them from
                    const QString msgAutoStash("Auto stashing");
                    menu.addAction(CIcons::appDbStash16(), msgAutoStash, mapComp, SLOT(ps_displayAutoStashingDialog()));

                    if (mapComp->m_autoStashDialog && mapComp->m_autoStashDialog->isCompleted())
                    {
                        menu.addAction(CIcons::appDbStash16(), "Last auto stash run", mapComp->m_autoStashDialog.data(), SLOT(showLastResults()));
                    }

                    // auto filter in DB views
                    QAction *a = menu.addAction(CIcons::filter16(), "Auto filtering in DB views (on/off)", mapComp, SLOT(ps_toggleAutoFiltering()));
                    a->setCheckable(true);
                    a->setChecked(mapComp->m_autoFilterInDbViews);
                }
            }
            this->nestedCustomMenu(menu);
        }

        CDbMappingComponent *CDbMappingComponent::CModelStashTools::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

        void CDbMappingComponent::CApplyDbDataMenu::customMenu(QMenu &menu) const
        {
            CDbMappingComponent *mapComp = mappingComponent();
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "no mapping component");

            if (mapComp->currentTabIndex() == CDbMappingComponent::TabStash && mapComp->currentModelView()->hasSelection())
            {
                this->addSeparator(menu);

                // stash view and selection
                QMenu *subMenu = menu.addMenu(CIcons::database16(), "Apply DB data (to selected)");
                QAction *a = nullptr;

                a = subMenu->addAction(CIcons::appAircraftIcao16(), "Current aircraft ICAO", mapComp, SLOT(ps_applyDbData()));
                a->setData(CAircraftIcaoCode().getClassName());

                a = subMenu->addAction(CIcons::appDistributors16(), "Current distributor", mapComp, SLOT(ps_applyDbData()));
                a->setData(CDistributor().getClassName());

                a = subMenu->addAction(CIcons::appLiveries16(), "Current livery", mapComp, SLOT(ps_applyDbData()));
                a->setData(CLivery().getClassName());

                // a = subMenu->addAction(CIcons::appAirlineIcao16(), "Current airline ICAO", mapComp, SLOT(ps_applyDbData()));
                // a->setData(CAirlineIcaoCode().getClassName());

                menu.addAction(CIcons::databaseTable16(), "Modify model data", mapComp, SLOT(ps_modifyModelDialog()));
            }
            this->nestedCustomMenu(menu);
        }

        CDbMappingComponent *CDbMappingComponent::CApplyDbDataMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

        void CDbMappingComponent::CShowSimulatorFile::customMenu(QMenu &menu) const
        {
            CDbMappingComponent *mapComp = mappingComponent();
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "no mapping component");

            if (mapComp->currentTabIndex() == CDbMappingComponent::TabOwnModels && mapComp->currentModelView()->hasSelection())
            {
                this->addSeparator(menu);
                menu.addAction(CIcons::text16(), "Open simulator file", mapComp, SLOT(ps_showSimulatorFile()));
            }

            this->nestedCustomMenu(menu);
        }

        CDbMappingComponent *CDbMappingComponent::CShowSimulatorFile::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

    } // ns
} // ns
