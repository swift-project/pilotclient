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
#include "blackgui/components/modelmatchercomponent.h"
#include "blackgui/menus/aircraftmodelmenus.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/shortcut.h"
#include "blackmisc/simulation/fscommon/aircraftcfgparser.h"
#include "blackmisc/simulation/aircraftmodelutils.h"
#include "blackmisc/logmessage.h"
#include <QFile>
#include <QShortcut>

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
using namespace BlackGui::Menus;

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
            this->ui->comp_OwnModelSet->setMappingComponent(this);

            this->ui->tvp_AircraftModelsForVPilot->setAircraftModelMode(CAircraftModelListModel::VPilotRuleModel);
            this->ui->tvp_AircraftModelsForVPilot->addFilterDialog();

            // model menus
            ui->comp_OwnAircraftModels->view()->setCustomMenu(new CShowSimulatorFileMenu(ui->comp_OwnAircraftModels->view(), this, true));
            ui->comp_OwnAircraftModels->view()->setCustomMenu(new CMergeWithVPilotMenu(this));
            ui->comp_OwnAircraftModels->view()->setCustomMenu(new COwnModelSetMenu(this, true));
            ui->comp_OwnAircraftModels->view()->setCustomMenu(new CModelStashToolsMenu(this, false));

            ui->comp_OwnModelSet->view()->setCustomMenu(new CShowSimulatorFileMenu(ui->comp_OwnModelSet->view(), this, true));
            ui->comp_OwnModelSet->view()->setCustomMenu(new CModelStashToolsMenu(this, true));

            ui->comp_StashAircraft->view()->setCustomMenu(new CShowSimulatorFileMenu(ui->comp_StashAircraft->view(), this, true));
            ui->comp_StashAircraft->view()->setCustomMenu(new CApplyDbDataMenu(this, true));
            ui->comp_StashAircraft->view()->setCustomMenu(new CModelStashToolsMenu(this, false));

            // connects
            connect(ui->editor_Model, &CModelMappingForm::requestStash, this, &CDbMappingComponent::ps_stashCurrentModel);

            connect(ui->comp_OwnAircraftModels->view(), &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
            connect(ui->comp_OwnAircraftModels->view(), &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onOwnModelsCountChanged);
            connect(ui->comp_OwnAircraftModels->view(), &CAircraftModelView::requestStash, this, &CDbMappingComponent::stashSelectedModels);
            connect(ui->comp_OwnAircraftModels->view(), &CAircraftModelView::toggledHighlightStashedModels, this, &CDbMappingComponent::ps_onStashedModelsChanged);

            connect(ui->comp_StashAircraft->view(), &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onStashCountChanged);
            connect(ui->comp_StashAircraft->view(), &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
            connect(ui->comp_StashAircraft->view(), &CAircraftModelView::requestHandlingOfStashDrop, this, &CDbMappingComponent::ps_handleStashDropRequest);
            connect(ui->comp_StashAircraft, &CDbStashComponent::stashedModelsChanged, this, &CDbMappingComponent::ps_onStashedModelsChanged);
            connect(ui->comp_StashAircraft, &CDbStashComponent::modelsSuccessfullyPublished, this, &CDbMappingComponent::ps_onModelsSuccessfullyPublished);

            connect(ui->comp_OwnModelSet->view(), &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onModelSetCountChanged);
            connect(ui->tw_ModelsToBeMapped, &QTabWidget::currentChanged, this, &CDbMappingComponent::ps_tabIndexChanged);
            connect(ui->tw_ModelsToBeMapped, &QTabWidget::currentChanged, ui->comp_ModelMatcher , &CModelMatcherComponent::tabIndexChanged);

            connect(ui->comp_OwnModelSet->view(), &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);

            // initial values
            this->ps_onModelSetCountChanged(ui->comp_OwnModelSet->view()->rowCount(), ui->comp_OwnModelSet->view()->hasFilter());
            this->ps_onStashCountChanged(ui->comp_StashAircraft->view()->rowCount(), ui->comp_StashAircraft->view()->hasFilter());

            // how to display forms
            ui->editor_AircraftIcao->setSelectOnly();
            ui->editor_Distributor->setSelectOnly();
            ui->editor_Livery->setSelectOnly();

            this->ui->tw_ModelsToBeMapped->setTabIcon(TabStash, CIcons::appDbStash16());
            this->ui->tw_ModelsToBeMapped->setTabIcon(TabOwnModels, CIcons::appModels16());

            // custom menu
            this->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this, &CDbMappingComponent::customContextMenuRequested, this, &CDbMappingComponent::ps_onCustomContextMenu);

            // vPilot
            this->initVPilotLoading();
        }

        CDbMappingComponent::~CDbMappingComponent()
        {
            gracefulShutdown();
        }

        void CDbMappingComponent::initVPilotLoading()
        {
            const bool canUseVPilot = true; // general flag if vPilot can be used/not used
            this->m_withVPilot = canUseVPilot && this->m_swiftDbUser.get().isMappingAdmin();
            static const QString tabName(this->ui->tw_ModelsToBeMapped->tabText(TabVPilot));

            if (this->m_vPilot1stInit && canUseVPilot)
            {
                this->m_vPilot1stInit = false;
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::rowCountChanged, this, &CDbMappingComponent::ps_onVPilotCountChanged);
                connect(&m_vPilotReader, &CVPilotRulesReader::readFinished, this, &CDbMappingComponent::ps_onLoadVPilotDataFinished);
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::requestStash, this, &CDbMappingComponent::stashSelectedModels);
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::toggledHighlightStashedModels, this, &CDbMappingComponent::ps_onStashedModelsChanged);
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::requestUpdate, this, &CDbMappingComponent::ps_requestVPilotDataUpdate);

                this->ui->tvp_AircraftModelsForVPilot->setCustomMenu(new CMappingVPilotMenu(this, true));
                this->ui->tvp_AircraftModelsForVPilot->setCustomMenu(new CModelStashToolsMenu(this, false));
                this->ui->tvp_AircraftModelsForVPilot->setDisplayAutomatically(true);

                this->ui->tvp_AircraftModelsForVPilot->addFilterDialog();
                const CAircraftModelList vPilotModels(m_vPilotReader.getAsModelsFromCache());
                this->ui->tvp_AircraftModelsForVPilot->updateContainerMaybeAsync(vPilotModels);
                const int noModels = vPilotModels.size();
                CLogMessage(this).info("%1 cached vPilot models loaded") << noModels;
            }
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

        CAircraftModel CDbMappingComponent::getModelFromView(const QModelIndex &index) const
        {
            if (!index.isValid()) { return CAircraftModel(); }
            const QObject *sender = QObject::sender();

            // check if we have an explicit sender
            if (sender == this->ui->tvp_AircraftModelsForVPilot)
            {
                return this->ui->tvp_AircraftModelsForVPilot->at(index);
            }
            else if (sender == this->ui->comp_OwnAircraftModels->view())
            {
                return this->ui->comp_OwnAircraftModels->view()->at(index);
            }
            else if (sender == this->ui->comp_StashAircraft || sender == this->ui->comp_StashAircraft->view())
            {
                return this->ui->comp_StashAircraft->view()->at(index);
            }
            else if (sender == this->ui->comp_OwnModelSet->view())
            {
                return this->ui->comp_OwnModelSet->view()->at(index);
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
            ui->comp_OwnAircraftModels->gracefulShutdown();
        }

        bool CDbMappingComponent::hasSelectedModelsToStash() const
        {
            TabIndex tab = currentTabIndex();
            switch (tab)
            {
            case TabOwnModels:
                return ui->comp_OwnAircraftModels->view()->hasSelectedModelsToStash();
            case TabVPilot:
                return ui->tvp_AircraftModelsForVPilot->hasSelectedModelsToStash();
            default:
                break;
            }
            return false;
        }

        CAircraftModelView *CDbMappingComponent::currentModelView() const
        {
            TabIndex tab = currentTabIndex();
            switch (tab)
            {
            case TabOwnModels:
                return ui->comp_OwnAircraftModels->view();
            case TabVPilot:
                return ui->tvp_AircraftModelsForVPilot;
            case TabStash:
                return ui->comp_StashAircraft->view();
            case TabOwnModelSet:
                return ui->comp_OwnModelSet->view();
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
                return ui->comp_OwnAircraftModels->view()->selectedObjects();
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

        CDbMappingComponent::TabIndex CDbMappingComponent::currentTabIndex() const
        {
            if (!ui->tw_ModelsToBeMapped) { return CDbMappingComponent::NoValidTab; }
            int t = ui->tw_ModelsToBeMapped->currentIndex();
            return static_cast<TabIndex>(t);
        }

        bool CDbMappingComponent::isStashTab() const
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
            if (this->ui->comp_StashAircraft->view()->selectedRowCount() < 1) { return; }

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
            if (this->ui->comp_StashAircraft->view()->hasSingleSelectedRow())
            {
                this->m_modelModifyDialog->setValue(this->ui->comp_StashAircraft->view()->selectedObject());
            }

            QDialog::DialogCode s = static_cast<QDialog::DialogCode>(this->m_modelModifyDialog->exec());
            if (s == QDialog::Rejected) { return; }
            CPropertyIndexVariantMap vm = this->m_modelModifyDialog->getValues();
            this->ui->comp_StashAircraft->applyToSelected(vm);
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
            }
            else
            {
                CLogMessage(this).error("Loading vPilot ruleset failed");
            }
            this->ui->comp_OwnAircraftModels->view()->hideLoadIndicator();
        }

        void CDbMappingComponent::ps_onVPilotCacheChanged()
        {
            if (this->ui->tvp_AircraftModelsForVPilot->displayAutomatically())
            {
                this->ui->tvp_AircraftModelsForVPilot->updateContainerMaybeAsync(this->m_vPilotReader.getAsModelsFromCache());
            }
        }

        void CDbMappingComponent::ps_requestVPilotDataUpdate()
        {
            this->ps_onVPilotCacheChanged();
        }

        void CDbMappingComponent::ps_onStashedModelsChanged()
        {
            const bool hlvp = this->ui->tvp_AircraftModelsForVPilot->derivedModel()->highlightModelStrings();
            const bool hlom = this->ui->comp_OwnAircraftModels->view()->derivedModel()->highlightModelStrings();
            const bool highlight =  hlom || hlvp;
            if (!highlight) { return; }
            const QStringList stashedModels(this->ui->comp_StashAircraft->getStashedModelStrings());
            if (hlvp)
            {
                this->ui->tvp_AircraftModelsForVPilot->derivedModel()->setHighlightModelStrings(stashedModels);
            }
            if (hlom)
            {
                this->ui->comp_OwnAircraftModels->view()->derivedModel()->setHighlightModelStrings(stashedModels);
            }
        }

        void CDbMappingComponent::ps_tabIndexChanged(int index)
        {
            CDbMappingComponent::TabIndex ti = static_cast<CDbMappingComponent::TabIndex>(index);
            switch (ti)
            {
            case CDbMappingComponent::TabOwnModelSet:
                ui->frp_Editors->setVisible(true);
                ui->editor_Model->setVisible(true);
                this->resizeForSelect();
                break;
            case CDbMappingComponent::TabModelMatcher:
                ui->editor_Model->setVisible(false);
                ui->frp_Editors->setVisible(false);
                this->resizeForSelect();
                break;
            default:
                ui->frp_Editors->setVisible(true);
                ui->editor_Model->setVisible(true);
                break;
            }
            emit this->tabIndexChanged(index);
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
            const int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_VPilot);
            QString o = this->ui->tw_ModelsToBeMapped->tabText(i);
            QString f = this->ui->tvp_AircraftModelsForVPilot->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, this->ui->tvp_AircraftModelsForVPilot->rowCount()) + f;
            this->ui->tw_ModelsToBeMapped->setTabText(i, o);
        }

        void CDbMappingComponent::ps_onOwnModelsCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            const int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_OwnModels);
            static const QString ot(this->ui->tw_ModelsToBeMapped->tabText(i));
            QString o(ot);
            const QString sim(ui->comp_OwnAircraftModels->getOwnModelsSimulator().toQString(true));
            if (!sim.isEmpty()) { o = o.append(" ").append(sim); }
            QString f = this->ui->comp_OwnAircraftModels->view()->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, this->ui->comp_OwnAircraftModels->view()->rowCount()) + f;
            this->ui->tw_ModelsToBeMapped->setTabText(i, o);
        }

        void CDbMappingComponent::ps_addToOwnModelSet()
        {
            if (this->currentTabIndex() != CDbMappingComponent::TabOwnModels) { return; }
            if (!currentModelView()->hasSelection()) { return; }
            const CAircraftModelList models(this->currentModelView()->selectedObjects());
            const CStatusMessage m = this->addToOwnModelSet(models, this->getOwnModelsSimulator());
            CLogMessage::preformatted(m);
        }

        void CDbMappingComponent::ps_mergeWithVPilotModels()
        {
            if (!ui->comp_OwnAircraftModels->modelLoader()) { return; }
            if (this->m_vPilotReader.getModelsCount() < 1) { return; }
            const CSimulatorInfo sim(ui->comp_OwnAircraftModels->getOwnModelsSimulator());
            if (!sim.isSingleSimulator() || !sim.isMicrosoftOrPrepare3DSimulator()) { return; }
            CAircraftModelList ownModels(getOwnModels());
            if (ownModels.isEmpty()) { return; }
            ui->comp_OwnAircraftModels->view()->showLoadIndicator();
            CAircraftModelUtilities::mergeWithVPilotData(ownModels, this->m_vPilotReader.getAsModelsFromCache(), true);
            ui->comp_OwnAircraftModels->updateViewAndCache(ownModels);
        }

        void CDbMappingComponent::ps_mergeSelectedWithVPilotModels()
        {
            if (!ui->comp_OwnAircraftModels->modelLoader()) { return; }
            if (this->m_vPilotReader.getModelsCount() < 1) { return; }
            if (!ui->comp_OwnAircraftModels->view()->hasSelection()) { return; }
            const CSimulatorInfo sim(ui->comp_OwnAircraftModels->getOwnModelsSimulator());
            if (!sim.isSingleSimulator() || !sim.isMicrosoftOrPrepare3DSimulator()) { return; }
            CAircraftModelList ownModels(getOwnSelectedModels()); // subset
            if (ownModels.isEmpty()) { return; }
            ui->comp_OwnAircraftModels->view()->showLoadIndicator();
            CAircraftModelUtilities::mergeWithVPilotData(ownModels, this->m_vPilotReader.getAsModelsFromCache(), true);

            // full models
            CAircraftModelList allModels = this->m_vPilotReader.getAsModelsFromCache();
            allModels.replaceOrAddModelsWithString(ownModels, Qt::CaseInsensitive);
            ui->comp_OwnAircraftModels->updateViewAndCache(allModels);
        }

        void CDbMappingComponent::ps_onCustomContextMenu(const QPoint &point)
        {
            QPoint globalPos = this->mapToGlobal(point);
            QScopedPointer<QMenu> contextMenu(new QMenu(this));

            contextMenu->addAction("Max.data area", this, &CDbMappingComponent::resizeForSelect, QKeySequence(Qt::CTRL + Qt::Key_M, Qt::Key_D));
            contextMenu->addAction("Max.mapping area", this, &CDbMappingComponent::resizeForMapping, QKeySequence(Qt::CTRL + Qt::Key_M, Qt::Key_M));
            QAction *selectedItem = contextMenu.data()->exec(globalPos);
            Q_UNUSED(selectedItem);
        }

        void CDbMappingComponent::ps_onStashCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_StashAircraftModels);
            QString o = this->ui->tw_ModelsToBeMapped->tabText(i);
            const QString f = this->ui->comp_StashAircraft->view()->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, this->ui->comp_StashAircraft->view()->rowCount()) + f;
            this->ui->tw_ModelsToBeMapped->setTabText(i, o);
        }

        void CDbMappingComponent::ps_onModelSetCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_OwnModelSet);
            QString o = "Model set " + ui->comp_OwnModelSet->getModelSetSimulator().toQString(true);
            const QString f = this->ui->comp_OwnModelSet->view()->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, this->ui->comp_OwnModelSet->view()->rowCount()) + f;
            this->ui->tw_ModelsToBeMapped->setTabText(i, o);
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

        CAircraftModel CDbMappingComponent::getEditorAircraftModel() const
        {
            CAircraftModel model(ui->editor_Model->getValue());
            model.setDistributor(ui->editor_Distributor->getValue());
            model.setAircraftIcaoCode(ui->editor_AircraftIcao->getValue());
            model.setLivery(ui->editor_Livery->getValue());
            return model;
        }

        CAircraftModelList CDbMappingComponent::getOwnModels() const
        {
            return this->ui->comp_OwnAircraftModels->getOwnModels();
        }

        CAircraftModelList CDbMappingComponent::getOwnSelectedModels() const
        {
            return this->ui->comp_OwnAircraftModels->getOwnSelectedModels();
        }

        CAircraftModel CDbMappingComponent::getOwnModelForModelString(const QString &modelString) const
        {
            return this->ui->comp_OwnAircraftModels->getOwnModelForModelString(modelString);
        }

        const CSimulatorInfo CDbMappingComponent::getOwnModelsSimulator() const
        {
            return this->ui->comp_OwnAircraftModels->getOwnModelsSimulator();
        }

        int CDbMappingComponent::getOwnModelsCount() const
        {
            return this->ui->comp_OwnAircraftModels->getOwnModelsCount();
        }

        CStatusMessage CDbMappingComponent::stashModel(const CAircraftModel &model, bool replace)
        {
            return this->ui->comp_StashAircraft->stashModel(model, replace);
        }

        CStatusMessageList CDbMappingComponent::stashModels(const CAircraftModelList &models)
        {
            return this->ui->comp_StashAircraft->stashModels(models);
        }

        CStatusMessage CDbMappingComponent::addToOwnModelSet(const CAircraftModelList &models, const CSimulatorInfo &simulator)
        {
            return this->ui->comp_OwnModelSet->addToModelSet(models, simulator);
        }

        CAircraftModel CDbMappingComponent::consolidateModel(const CAircraftModel &model) const
        {
            return this->ui->comp_StashAircraft->consolidateModel(model);
        }

        void CDbMappingComponent::replaceStashedModelsUnvalidated(const CAircraftModelList &models) const
        {
            this->ui->comp_StashAircraft->replaceModelsUnvalidated(models);
        }

        void CDbMappingComponent::CMappingVPilotMenu::customMenu(QMenu &menu) const
        {
            CDbMappingComponent *mapComp = qobject_cast<CDbMappingComponent *>(this->parent());
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "Cannot access mapping component");

            bool canUseVPilot = mappingComponent()->withVPilot();
            if (canUseVPilot)
            {
                this->addSeparator(menu);
                menu.addAction(CIcons::appMappings16(), "Load vPilot Rules", mapComp, &CDbMappingComponent::ps_loadVPilotData);
            }
            this->nestedCustomMenu(menu);
        }

        CDbMappingComponent *CDbMappingComponent::CMappingVPilotMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

        CDbMappingComponent::CModelStashToolsMenu::CModelStashToolsMenu(CDbMappingComponent *mappingComponent, bool separator) :
            BlackGui::Menus::IMenuDelegate(mappingComponent, separator)
        {}

        void CDbMappingComponent::CModelStashToolsMenu::customMenu(QMenu &menu) const
        {
            CDbMappingComponent *mapComp = mappingComponent();
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "no mapping component");
            const bool canConnectDb = sGui->getWebDataServices()->canConnectSwiftDb();
            if (!canConnectDb) { this->nestedCustomMenu(menu); return; }

            if (!mapComp->currentModelView()->isEmpty() && mapComp->currentModelView()->getMenu().testFlag(CViewBaseNonTemplate::MenuCanStashModels))
            {
                QMenu *stashMenu = new QMenu("Stash tools", &menu);
                int dbModels = sGui->getWebDataServices()->getModelsCount();
                if (dbModels > 0)
                {
                    // we have keys and data by which we could delete them from view
                    const QString msgDelete("Delete " + QString::number(dbModels) + " DB model(s) from " + mapComp->currentTabText());
                    stashMenu->addAction(CIcons::delete16(), msgDelete, mapComp, &CDbMappingComponent::ps_removeDbModelsFromView);
                }

                const QString msgAutoStash("Auto stashing");
                stashMenu->addAction(CIcons::appDbStash16(), msgAutoStash, mapComp, &CDbMappingComponent::ps_displayAutoStashingDialog);

                if (mapComp->m_autoStashDialog && mapComp->m_autoStashDialog->isCompleted())
                {
                    stashMenu->addAction(CIcons::appDbStash16(), "Last auto stash run", mapComp->m_autoStashDialog.data(), &CDbAutoStashingComponent::showLastResults);
                }

                // auto filter in DB views
                QAction *a = stashMenu->addAction(CIcons::filter16(), "Auto filtering in DB views (on/off)", mapComp, &CDbMappingComponent::ps_toggleAutoFiltering);
                a->setCheckable(true);
                a->setChecked(mapComp->m_autoFilterInDbViews);

                // add menu
                if (stashMenu->isEmpty())
                {
                    menu.deleteLater();
                }
                else
                {
                    this->addSeparator(menu);
                    menu.addMenu(stashMenu);
                    stashMenu->setIcon(CIcons::appDbStash16());
                }
            }
            this->nestedCustomMenu(menu);
        }

        void CDbMappingComponent::CModelStashToolsMenu::addSeparator(QMenu &menu) const
        {
            if (this->previousMenuItemContains("Stash", menu, Qt::CaseInsensitive)) { return; }
            IMenuDelegate::addSeparator(menu);
        }

        CDbMappingComponent *CDbMappingComponent::CModelStashToolsMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

        void CDbMappingComponent::COwnModelSetMenu::customMenu(QMenu &menu) const
        {
            CDbMappingComponent *mapComp = mappingComponent();
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "no mapping component");
            if (mapComp->currentTabIndex() == CDbMappingComponent::TabOwnModels && mapComp->currentModelView()->hasSelection())
            {
                this->addSeparator(menu);
                menu.addAction(CIcons::appAircraft16(), "Add to own model set", mapComp, &CDbMappingComponent::ps_addToOwnModelSet);
            }
            this->nestedCustomMenu(menu);
        }

        CDbMappingComponent *CDbMappingComponent::COwnModelSetMenu::mappingComponent() const
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
                QMenu *subMenu = menu.addMenu(CIcons::database16(), "Apply editor DB data (to selected)");
                QAction *a = nullptr;

                a = subMenu->addAction(CIcons::appAircraftIcao16(), "Current aircraft ICAO", mapComp, &CDbMappingComponent::ps_applyDbData);
                a->setData(CAircraftIcaoCode().getClassName());

                a = subMenu->addAction(CIcons::appDistributors16(), "Current distributor", mapComp, &CDbMappingComponent::ps_applyDbData);
                a->setData(CDistributor().getClassName());

                a = subMenu->addAction(CIcons::appLiveries16(), "Current livery", mapComp, &CDbMappingComponent::ps_applyDbData);
                a->setData(CLivery().getClassName());

                // a = subMenu->addAction(CIcons::appAirlineIcao16(), "Current airline ICAO", mapComp, &CDbMappingComponent::ps_applyDbData);
                // a->setData(CAirlineIcaoCode().getClassName());

                menu.addAction(CIcons::databaseTable16(), "Modify DB model data", mapComp, &CDbMappingComponent::ps_applyDbData);
                a->setData(CAircraftModel().getClassName());
            }
            this->nestedCustomMenu(menu);
        }

        void CDbMappingComponent::CApplyDbDataMenu::addSeparator(QMenu &menu) const
        {
            if (this->previousMenuItemContains("DB", menu)) { return; }
            IMenuDelegate::addSeparator(menu);
        }

        CDbMappingComponent *CDbMappingComponent::CApplyDbDataMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

        CDbMappingComponent::CMergeWithVPilotMenu::CMergeWithVPilotMenu(CDbMappingComponent *mappingComponent, bool separator) :
            IMenuDelegate(mappingComponent, separator)
        {
            Q_ASSERT_X(mappingComponent, Q_FUNC_INFO, "Missing vPilot reader");
        }

        void CDbMappingComponent::CMergeWithVPilotMenu::customMenu(QMenu &menu) const
        {
            const CAircraftModelView *mv = mappingComponent()->ui->comp_OwnAircraftModels->view();
            const CSimulatorInfo sim = mappingComponent()->ui->comp_OwnAircraftModels->getOwnModelsSimulator();
            if (!mappingComponent()->withVPilot() || mv->isEmpty() || !sim.isSingleSimulator() || !sim.isMicrosoftOrPrepare3DSimulator())
            {
                this->nestedCustomMenu(menu);
                return;
            }
            this->addSeparator(menu);
            QMenu *mm = menu.addMenu("Merge with vPilot data");
            mm->addAction("All", mappingComponent(), &CDbMappingComponent::ps_mergeWithVPilotModels);
            if (mv->hasSelection())
            {
                mm->addAction("Selected only", mappingComponent(), &CDbMappingComponent::ps_mergeSelectedWithVPilotModels);
            }
            this->nestedCustomMenu(menu);
        }

        CDbMappingComponent *CDbMappingComponent::CMergeWithVPilotMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }
    } // ns
} // ns
