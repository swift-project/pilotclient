/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbautostashingcomponent.h"
#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/dbmodelmappingmodifycomponent.h"
#include "blackgui/components/dbownmodelscomponent.h"
#include "blackgui/components/dbownmodelsetcomponent.h"
#include "blackgui/components/dbstashcomponent.h"
#include "blackgui/components/modelmatchercomponent.h"
#include "blackgui/editors/aircrafticaoform.h"
#include "blackgui/editors/distributorform.h"
#include "blackgui/editors/liveryform.h"
#include "blackgui/editors/modelmappingform.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/menus/aircraftmodelmenus.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/authenticateduser.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/simulation/aircraftmodelutils.h"
#include "ui_dbmappingcomponent.h"

#include <QAction>
#include <QDialog>
#include <QFrame>
#include <QKeySequence>
#include <QMenu>
#include <QModelIndex>
#include <QPoint>
#include <QSplitter>
#include <QTabWidget>
#include <QVariant>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

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
            connect(ui->comp_OwnAircraftModels->view(), &CAircraftModelView::modelDataChanged, this, &CDbMappingComponent::ps_onOwnModelsChanged);
            connect(ui->comp_OwnAircraftModels->view(), &CAircraftModelView::requestStash, this, &CDbMappingComponent::stashSelectedModels);
            connect(ui->comp_OwnAircraftModels->view(), &CAircraftModelView::toggledHighlightStashedModels, this, &CDbMappingComponent::ps_onStashedModelsChanged);

            connect(ui->comp_StashAircraft->view(), &CAircraftModelView::modelDataChanged, this, &CDbMappingComponent::ps_onStashedModelsDataChanged);
            connect(ui->comp_StashAircraft->view(), &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
            connect(ui->comp_StashAircraft->view(), &CAircraftModelView::requestHandlingOfStashDrop, this, &CDbMappingComponent::ps_handleStashDropRequest);
            connect(ui->comp_StashAircraft, &CDbStashComponent::stashedModelsChanged, this, &CDbMappingComponent::ps_onStashedModelsChanged);
            connect(ui->comp_StashAircraft, &CDbStashComponent::modelsSuccessfullyPublished, this, &CDbMappingComponent::ps_onModelsSuccessfullyPublished);

            connect(ui->comp_OwnModelSet->view(), &CAircraftModelView::modelDataChanged, this, &CDbMappingComponent::ps_onModelSetChanged);
            connect(ui->comp_OwnModelSet->view(), &CAircraftModelView::requestStash, this, &CDbMappingComponent::stashSelectedModels);

            connect(ui->tw_ModelsToBeMapped, &QTabWidget::currentChanged, this, &CDbMappingComponent::ps_tabIndexChanged);
            connect(ui->tw_ModelsToBeMapped, &QTabWidget::currentChanged, ui->comp_ModelMatcher , &CModelMatcherComponent::tabIndexChanged);

            connect(ui->comp_OwnModelSet->view(), &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);

            // initial values
            this->ps_onModelSetChanged(ui->comp_OwnModelSet->view()->rowCount(), ui->comp_OwnModelSet->view()->hasFilter());
            this->ps_onStashedModelsDataChanged(ui->comp_StashAircraft->view()->rowCount(), ui->comp_StashAircraft->view()->hasFilter());
            this->ps_onOwnModelsChanged(ui->comp_OwnAircraftModels->view()->rowCount(), ui->comp_OwnAircraftModels->view()->hasFilter());

            // how to display forms
            ui->editor_AircraftIcao->setSelectOnly();
            ui->editor_Distributor->setSelectOnly();
            ui->editor_Livery->setSelectOnly();

            this->ui->tw_ModelsToBeMapped->setTabIcon(TabStash, CIcons::appDbStash16());
            this->ui->tw_ModelsToBeMapped->setTabIcon(TabOwnModels, CIcons::appModels16());
            this->ui->tw_ModelsToBeMapped->setTabIcon(TabOwnModelSet, CIcons::appModels16());

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
            this->m_vPilotEnabled = vPilotSupport && this->m_swiftDbUser.get().isMappingAdmin();
            static const QString tabName(this->ui->tw_ModelsToBeMapped->tabText(TabVPilot));

            if (this->m_vPilot1stInit && vPilotSupport)
            {
                this->m_vPilot1stInit = false;
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::doubleClicked, this, &CDbMappingComponent::ps_onModelRowSelected);
                connect(this->ui->tvp_AircraftModelsForVPilot, &CAircraftModelView::modelDataChanged, this, &CDbMappingComponent::ps_onVPilotDataChanged);
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
            this->ui->tab_VPilot->setEnabled(this->m_vPilotEnabled);
            this->ui->tab_VPilot->setVisible(this->m_vPilotEnabled);
            if (this->m_vPilotEnabled)
            {
                // create / restore tab
                this->ui->tw_ModelsToBeMapped->addTab(this->ui->tab_VPilot, tabName);
                this->ps_onVPilotDataChanged(
                    this->ui->tvp_AircraftModelsForVPilot->rowCount(),
                    this->ui->tvp_AircraftModelsForVPilot->hasFilter());
            }
            else
            {
                this->m_vPilotFormatted = false;
                this->ui->tw_ModelsToBeMapped->removeTab(TabVPilot);
            }
        }

        void CDbMappingComponent::formatVPilotView()
        {
            if (!this->m_vPilotEnabled || this->m_vPilotFormatted) { return; }
            this->m_vPilotFormatted = true;
            this->ui->tvp_AircraftModelsForVPilot->presizeOrFullResizeToContents();
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
            const TabIndex tab = currentTabIndex();
            switch (tab)
            {
            case TabOwnModels:
                return ui->comp_OwnAircraftModels->view()->hasSelectedModelsToStash();
            case TabOwnModelSet:
                return ui->comp_OwnModelSet->view()->hasSelectedModelsToStash();
            case TabVPilot:
                return ui->tvp_AircraftModelsForVPilot->hasSelectedModelsToStash();
            default:
                break;
            }
            return false;
        }

        CAircraftModelView *CDbMappingComponent::currentModelView() const
        {
            const TabIndex tab = currentTabIndex();
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
            const int i = this->ui->tw_ModelsToBeMapped->currentIndex();
            return this->ui->tw_ModelsToBeMapped->tabText(i);
        }

        void CDbMappingComponent::updateEditorsWhenApplicable()
        {
            const CAircraftModel currentEditorModel(ui->editor_Model->getValue());
            if (!currentEditorModel.hasModelString()) { return; } // no related model
            const QString modelString(currentEditorModel.getModelString());
            const CAircraftModel currentStashedModel(ui->comp_StashAircraft->getStashedModel(modelString));
            if (!currentStashedModel.hasModelString()) { return; }

            // we have found a model in the stashed models and this is the one currently displayed
            // in the editors
            bool updated = false;
            const CLivery stashedLivery(currentStashedModel.getLivery());
            if (stashedLivery.hasValidDbKey())
            {
                if (ui->editor_Livery->setValue(stashedLivery)) { updated = true; }
            }

            const CDistributor stashedDistributor(currentStashedModel.getDistributor());
            if (stashedDistributor.hasValidDbKey())
            {
                if (ui->editor_Distributor->setValue(stashedDistributor)) { updated = true; }
            }

            const CAircraftIcaoCode stashedIcaoCode(currentStashedModel.getAircraftIcaoCode());
            if (stashedIcaoCode.hasValidDbKey())
            {
                if (ui->editor_AircraftIcao->setValue(stashedIcaoCode)) { updated = true; }
            }

            if (updated)
            {
                CLogMessage(this).info("Updated editor data for '%1'") << modelString;
            }
        }

        CAircraftModelList CDbMappingComponent::getSelectedModelsToStash() const
        {
            if (!hasSelectedModelsToStash()) { return CAircraftModelList(); }
            const TabIndex tab = currentTabIndex();
            switch (tab)
            {
            case TabOwnModels:
                return ui->comp_OwnAircraftModels->view()->selectedObjects();
            case TabOwnModelSet:
                return ui->comp_OwnModelSet->view()->selectedObjects();
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
            const int t = ui->tw_ModelsToBeMapped->currentIndex();
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
            const CLivery stdLivery(sGui->getWebDataServices()->getStdLiveryForAirlineCode(code));
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
            const QStringList modelStrings(sGui->getWebDataServices()->getModelStrings());
            if (modelStrings.isEmpty()) { return; }
            switch (currentTabIndex())
            {
            case TabVPilot:
            case TabOwnModels:
            case TabOwnModelSet:
            case TabStash:
                this->currentModelView()->removeModelsWithModelString(modelStrings);
                break;
            default:
                break;
            }
        }

        void CDbMappingComponent::ps_toggleAutoFiltering()
        {
            this->m_autoFilterInDbViews = !this->m_autoFilterInDbViews;
        }

        void CDbMappingComponent::ps_applyFormLiveryData()
        {
            if (this->ui->comp_StashAircraft->view()->selectedRowCount() < 1) { return; }
            const CStatusMessageList msgs = this->ui->editor_Livery->validate(true);
            if (msgs.hasErrorMessages())
            {
                this->showOverlayMessages(msgs);
            }
            else
            {
                this->ui->comp_StashAircraft->applyToSelected(this->ui->editor_Livery->getValue());
            }
        }

        void CDbMappingComponent::ps_applyFormAircraftIcaoData()
        {
            if (this->ui->comp_StashAircraft->view()->selectedRowCount() < 1) { return; }
            const CStatusMessageList msgs = this->ui->editor_AircraftIcao->validate(true);
            if (msgs.hasErrorMessages())
            {
                this->showOverlayMessages(msgs);
            }
            else
            {
                this->ui->comp_StashAircraft->applyToSelected(this->ui->editor_AircraftIcao->getValue());
            }
        }

        void CDbMappingComponent::ps_applyFormDistributorData()
        {
            if (this->ui->comp_StashAircraft->view()->selectedRowCount() < 1) { return; }
            const CStatusMessageList msgs = this->ui->editor_Distributor->validate(true);
            if (msgs.hasErrorMessages())
            {
                this->showOverlayMessages(msgs);
            }
            else
            {
                this->ui->comp_StashAircraft->applyToSelected(this->ui->editor_Distributor->getValue());
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
            if (!m_vPilotEnabled) { return; }
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
            this->ui->tvp_AircraftModelsForVPilot->hideLoadIndicator();
        }

        void CDbMappingComponent::ps_onVPilotCacheChanged()
        {
            if (this->ui->tvp_AircraftModelsForVPilot->displayAutomatically())
            {
                this->ui->tvp_AircraftModelsForVPilot->updateContainerMaybeAsync(this->m_vPilotReader.getAsModelsFromCache());
            }
            else
            {
                this->ui->tvp_AircraftModelsForVPilot->hideLoadIndicator();
            }
        }

        void CDbMappingComponent::ps_requestVPilotDataUpdate()
        {
            this->ps_onVPilotCacheChanged();
        }

        void CDbMappingComponent::ps_onStashedModelsChanged()
        {
            emit this->ps_digestStashedModelsChanged();
        }

        void CDbMappingComponent::ps_onStashedModelsChangedDigest()
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
            const CDbMappingComponent::TabIndex ti = static_cast<CDbMappingComponent::TabIndex>(index);
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
            case CDbMappingComponent::TabVPilot:
                // fall thru intended
                this->formatVPilotView();
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

        void CDbMappingComponent::ps_onVPilotDataChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            const int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_VPilot);
            QString o = this->ui->tw_ModelsToBeMapped->tabText(i);
            QString f = this->ui->tvp_AircraftModelsForVPilot->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, this->ui->tvp_AircraftModelsForVPilot->rowCount()) + f;
            this->ui->tw_ModelsToBeMapped->setTabText(i, o);
        }

        void CDbMappingComponent::ps_onOwnModelsChanged(int count, bool withFilter)
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

        void CDbMappingComponent::ps_onStashedModelsDataChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int i = this->ui->tw_ModelsToBeMapped->indexOf(this->ui->tab_StashAircraftModels);
            QString o = this->ui->tw_ModelsToBeMapped->tabText(i);
            const QString f = this->ui->comp_StashAircraft->view()->hasFilter() ? "F" : "";
            o = CGuiUtility::replaceTabCountValue(o, this->ui->comp_StashAircraft->view()->rowCount()) + f;
            this->ui->tw_ModelsToBeMapped->setTabText(i, o);

            // update editors
            this->updateEditorsWhenApplicable();
        }

        void CDbMappingComponent::ps_onModelSetChanged(int count, bool withFilter)
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

        void CDbMappingComponent::setOwnModelsSimulator(const CSimulatorInfo &simulator)
        {
            this->ui->comp_OwnAircraftModels->setSimulator(simulator);
        }

        int CDbMappingComponent::getOwnModelsCount() const
        {
            return this->ui->comp_OwnAircraftModels->getOwnModelsCount();
        }

        void CDbMappingComponent::setOwnModelSetSimulator(const CSimulatorInfo &simulator)
        {
            Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
            this->ui->comp_OwnModelSet->setModelSetSimulator(simulator);
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

        void CDbMappingComponent::CMappingVPilotMenu::customMenu(CMenuActions &menuActions)
        {
            CDbMappingComponent *mapComp = qobject_cast<CDbMappingComponent *>(this->parent());
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "Cannot access mapping component");

            const bool canUseVPilot = mappingComponent()->withVPilot();
            if (canUseVPilot)
            {
                this->m_menuAction = menuActions.addAction(this->m_menuAction, CIcons::appMappings16(), "Load vPilot Rules", CMenuAction::pathVPilot(), this, { mapComp, &CDbMappingComponent::ps_loadVPilotData });
            }
            this->nestedCustomMenu(menuActions);
        }

        CDbMappingComponent *CDbMappingComponent::CMappingVPilotMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

        CDbMappingComponent::CModelStashToolsMenu::CModelStashToolsMenu(CDbMappingComponent *mappingComponent, bool separator) :
            BlackGui::Menus::IMenuDelegate(mappingComponent, separator)
        {}

        void CDbMappingComponent::CModelStashToolsMenu::customMenu(CMenuActions &menuActions)
        {
            CDbMappingComponent *mapComp = mappingComponent();
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "no mapping component");
            if (!mapComp->currentModelView()->isEmpty() && mapComp->currentModelView()->getMenu().testFlag(CViewBaseNonTemplate::MenuCanStashModels))
            {
                menuActions.addMenuStash();

                // auto filter in DB views
                this->m_stashFiltering = menuActions.addAction(this->m_stashFiltering, CIcons::filter16(), "Auto filtering in DB views (on/off)", CMenuAction::pathStash(), this, { mapComp, &CDbMappingComponent::ps_toggleAutoFiltering });
                this->m_stashFiltering->setCheckable(true);
                this->m_stashFiltering->setChecked(mapComp->m_autoFilterInDbViews);
                this->addRemoveDbModels(menuActions);

                this->m_autoStashing = menuActions.addAction(this->m_autoStashing, CIcons::appDbStash16(), "Auto stashing", CMenuAction::pathStash(), this, { mapComp, &CDbMappingComponent::ps_displayAutoStashingDialog });
                if (mapComp->m_autoStashDialog && mapComp->m_autoStashDialog->isCompleted())
                {
                    menuActions.addAction(CIcons::appDbStash16(), "Last auto stash run", CMenuAction::pathStash(), nullptr, { mapComp->m_autoStashDialog.data(), &CDbAutoStashingComponent::showLastResults });
                }
            }
            else if (mapComp->currentTabIndex() == CDbMappingComponent::TabStash)
            {
                this->addRemoveDbModels(menuActions);
            }
            this->nestedCustomMenu(menuActions);
        }

        void CDbMappingComponent::CModelStashToolsMenu::addRemoveDbModels(CMenuActions &menuActions)
        {
            CDbMappingComponent *mapComp = mappingComponent();
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "no mapping component");

            const int dbModels = sGui->getWebDataServices()->getModelsCount();
            if (dbModels > 0)
            {
                // we have keys and data by which we could delete them from view
                const QString msgDelete("Delete " + QString::number(dbModels) + " DB model(s) from " + mapComp->currentTabText());
                menuActions.addAction(CIcons::delete16(), msgDelete, CMenuAction::pathStash(), nullptr, { mapComp, &CDbMappingComponent::ps_removeDbModelsFromView});
            }
        }

        CDbMappingComponent *CDbMappingComponent::CModelStashToolsMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

        void CDbMappingComponent::COwnModelSetMenu::customMenu(CMenuActions &menuActions)
        {
            CDbMappingComponent *mapComp = mappingComponent();
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "no mapping component");
            if (mapComp->currentTabIndex() == CDbMappingComponent::TabOwnModels && mapComp->currentModelView()->hasSelection())
            {
                menuActions.addMenuModelSet();
                this->m_menuAction = menuActions.addAction(this->m_menuAction, CIcons::appModels16(), "Add to own model set", CMenuAction::pathModelSet(), this, { mapComp, &CDbMappingComponent::ps_addToOwnModelSet });
            }
            this->nestedCustomMenu(menuActions);
        }

        CDbMappingComponent *CDbMappingComponent::COwnModelSetMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }

        void CDbMappingComponent::CApplyDbDataMenu::customMenu(CMenuActions &menuActions)
        {
            CDbMappingComponent *mapComp = mappingComponent();
            Q_ASSERT_X(mapComp, Q_FUNC_INFO, "no mapping component");

            if (mapComp->currentTabIndex() == CDbMappingComponent::TabStash && mapComp->currentModelView()->hasSelection())
            {
                if (this->m_menuActions.isEmpty()) { this->m_menuActions = QList<QAction *>({ nullptr, nullptr, nullptr, nullptr }); }

                // stash view and selection
                menuActions.addMenuStashEditor();

                this->m_menuActions[0] = menuActions.addAction(this->m_menuActions[0], CIcons::appAircraftIcao16(), "Current aircraft ICAO", CMenuAction::pathStashEditor(), this, { mapComp, &CDbMappingComponent::ps_applyFormAircraftIcaoData });
                this->m_menuActions[1] = menuActions.addAction(this->m_menuActions[1], CIcons::appDistributors16(), "Current distributor", CMenuAction::pathStashEditor(), this, { mapComp, &CDbMappingComponent::ps_applyFormDistributorData });
                this->m_menuActions[2] = menuActions.addAction(this->m_menuActions[2], CIcons::appLiveries16(), "Current livery", CMenuAction::pathStashEditor(), this, { mapComp, &CDbMappingComponent::ps_applyFormLiveryData });
                this->m_menuActions[3] = menuActions.addAction(this->m_menuActions[3], CIcons::databaseTable16(), "Modify DB model data", CMenuAction::pathStashEditor(), this, { mapComp, &CDbMappingComponent::ps_modifyModelDialog });
            }
            this->nestedCustomMenu(menuActions);
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

        void CDbMappingComponent::CMergeWithVPilotMenu::customMenu(CMenuActions &menuActions)
        {
            const CAircraftModelView *mv = mappingComponent()->ui->comp_OwnAircraftModels->view();
            const CSimulatorInfo sim = mappingComponent()->ui->comp_OwnAircraftModels->getOwnModelsSimulator();
            if (!mappingComponent()->withVPilot() || mv->isEmpty() || !sim.isSingleSimulator() || !sim.isMicrosoftOrPrepare3DSimulator())
            {
                this->nestedCustomMenu(menuActions);
                return;
            }

            if (this->m_menuActions.isEmpty()) { this->m_menuActions = QList<QAction *>({ nullptr, nullptr }); }
            menuActions.addMenu("Merge with vPilot data", CMenuAction::pathVPilot());
            this->m_menuActions[0] = menuActions.addAction(this->m_menuActions[0], "All", CMenuAction::pathVPilot(), this, { mappingComponent(), &CDbMappingComponent::ps_mergeWithVPilotModels });
            if (mv->hasSelection())
            {
                this->m_menuActions[1] = menuActions.addAction(this->m_menuActions[1], "Selected only", CMenuAction::pathVPilot(), this, { mappingComponent(), &CDbMappingComponent::ps_mergeSelectedWithVPilotModels });
            }
            this->nestedCustomMenu(menuActions);
        }

        CDbMappingComponent *CDbMappingComponent::CMergeWithVPilotMenu::mappingComponent() const
        {
            return qobject_cast<CDbMappingComponent *>(this->parent());
        }
    } // ns
} // ns
