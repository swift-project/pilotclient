/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/filters/aircraftmodelfilterdialog.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/shortcut.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/icons.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatorinfolist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QAction>
#include <QDropEvent>
#include <QMap>
#include <QShortcut>
#include <QString>
#include <QWidget>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;
using namespace BlackGui::Filters;
using namespace BlackGui::Menus;

namespace BlackGui
{
    namespace Views
    {
        CAircraftModelView::CAircraftModelView(QWidget *parent) : COrderableViewWithDbObjects(parent)
        {
            // default
            this->standardInit(new CAircraftModelListModel(CAircraftModelListModel::OwnAircraftModelClient, this));

            // shortcut
            new QShortcut(CShortcut::keyStash(), this, SLOT(ps_requestStash()), nullptr, Qt::WidgetShortcut);

            // default mode
            CAircraftModelListModel::AircraftModelMode mode = derivedModel()->getModelMode();
            this->setAircraftModelMode(mode);
            this->setSortIndicator();
        }

        void CAircraftModelView::setAircraftModelMode(CAircraftModelListModel::AircraftModelMode mode)
        {
            this->setCustomMenu(nullptr, false); // delete everything
            derivedModel()->setAircraftModelMode(mode);
            this->setSortIndicator();
            switch (mode)
            {
            case CAircraftModelListModel::StashModel:
                this->m_menus = MenuDefaultNoClear;
                break;
            case CAircraftModelListModel::Database:
                this->m_menus = MenuDefaultDbViews;
                break;
            case CAircraftModelListModel::VPilotRuleModel:
                this->m_menus = MenuDefaultNoClear | MenuStashing;
                break;
            case CAircraftModelListModel::OwnAircraftModelMappingTool:
                this->m_menus = MenuDefaultNoClear | MenuStashing | MenuLoadAndSave;
                break;
            case CAircraftModelListModel::OwnAircraftModelClient:
            default:
                this->m_menus = MenuDefaultNoClear | MenuBackend;
                break;
            }
        }

        int CAircraftModelView::applyToSelected(const CLivery &livery)
        {
            if (!hasSelection()) { return 0; }
            int c = this->updateSelected(CVariant::from(livery), CAircraftModel::IndexLivery);
            return c;
        }

        int CAircraftModelView::applyToSelected(const CAircraftIcaoCode &icao)
        {
            if (!hasSelection()) { return 0; }
            int c = this->updateSelected(CVariant::from(icao), CAircraftModel::IndexAircraftIcaoCode);
            return c;
        }

        int CAircraftModelView::applyToSelected(const CDistributor &distributor)
        {
            if (!hasSelection()) { return 0; }
            int c = this->updateSelected(CVariant::from(distributor), CAircraftModel::IndexDistributor);
            return c;
        }

        int CAircraftModelView::applyToSelected(const CPropertyIndexVariantMap &vm)
        {
            if (!hasSelection()) { return 0; }
            int c = this->updateSelected(vm);
            return c;
        }

        bool CAircraftModelView::hasSelectedModelsToStash() const
        {
            return m_menus.testFlag(MenuCanStashModels) && hasSelection();
        }

        void CAircraftModelView::setAcceptedMetaTypeIds()
        {
            Q_ASSERT(this->m_model);
            this->m_model->setAcceptedMetaTypeIds(
            {
                qMetaTypeId<CAirlineIcaoCode>(), qMetaTypeId<CAirlineIcaoCodeList>(),
                qMetaTypeId<CAircraftIcaoCode>(), qMetaTypeId<CAircraftIcaoCodeList>(),
                qMetaTypeId<CLivery>(), qMetaTypeId<CLiveryList>(),
                qMetaTypeId<CDistributor>(), qMetaTypeId<CDistributorList>(),
                qMetaTypeId<CAircraftModel>(), qMetaTypeId<CAircraftModelList>(),
            });
        }

        void CAircraftModelView::addFilterDialog()
        {
            this->setFilterDialog(new CAircraftModelFilterDialog(this));
        }

        CAircraftModelFilterDialog *CAircraftModelView::getFilterDialog() const
        {
            return qobject_cast<CAircraftModelFilterDialog *>(this->getFilterWidget());
        }

        int CAircraftModelView::removeModelsWithModelString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity)
        {
            if (modelStrings.isEmpty()) { return 0; }
            CAircraftModelList copy(this->container());
            const int delta = copy.removeModelsWithString(modelStrings, sensitivity);
            if (delta > 0)
            {
                this->updateContainerMaybeAsync(copy);
            }
            return delta;
        }

        int CAircraftModelView::removeModelsWithModelString(const CAircraftModelList &models, Qt::CaseSensitivity sensitivity)
        {
            return this->removeModelsWithModelString(models.getModelStringList(), sensitivity);
        }

        int CAircraftModelView::replaceOrAddModelsWithString(const CAircraftModelList &models, Qt::CaseSensitivity sensitivity)
        {
            if (models.isEmpty()) { return 0; }
            CAircraftModelList copy(this->container());
            int c = copy.replaceOrAddModelsWithString(models, sensitivity);
            if (c == 0) { return 0; }
            this->updateContainerMaybeAsync(copy);
            return c;
        }

        void CAircraftModelView::setHighlightModelStrings(const QStringList &highlightModels)
        {
            this->derivedModel()->setHighlightModelStrings(highlightModels);
        }

        void CAircraftModelView::setHighlightModelStrings(bool highlight)
        {
            this->derivedModel()->setHighlightModelStrings(highlight);
        }

        void CAircraftModelView::setHighlightModelStringsColor(const QBrush &brush)
        {
            this->derivedModel()->setHighlightModelStringsColor(brush);
        }

        bool CAircraftModelView::highlightModelStrings() const
        {
            return this->derivedModel()->highlightModelStrings();
        }

        void CAircraftModelView::dropEvent(QDropEvent *event)
        {
            // moves from myself are ignored
            // depends on isDropAllowed() / acceptDrop() if this function is called
            Qt::DropAction action = event->dropAction();
            if (action == Qt::MoveAction)
            {
                COrderableViewWithDbObjects::dropEvent(event);
                return;
            }
            if (!this->isDropAllowed()) { return; }
            if (!event) { return; }
            const QMimeData *mime = event->mimeData();
            if (!mime) { return; }

            if (CGuiUtility::hasSwiftVariantMimeType(mime))
            {
                CVariant valueVariant(CGuiUtility::fromSwiftDragAndDropData(mime));
                if (valueVariant.isValid())
                {
                    if (valueVariant.canConvert<CAircraftModel>())
                    {
                        CAircraftModel model = valueVariant.value<CAircraftModel>();
                        if (!model.hasModelString()) { return; }
                        const CAircraftModelList models({model});
                        this->derivedModel()->replaceOrAddByModelString(models);
                        return;
                    }
                    else if (valueVariant.canConvert<CAircraftModelList>())
                    {
                        CAircraftModelList models(valueVariant.value<CAircraftModelList>());
                        if (models.isEmpty()) { return; }
                        this->derivedModel()->replaceOrAddByModelString(models);
                        return;
                    }

                    // only for selected members
                    if (!this->hasSelection()) { return; }
                    if (valueVariant.canConvert<CAircraftIcaoCode>())
                    {
                        CAircraftIcaoCode icao = valueVariant.value<CAircraftIcaoCode>();
                        if (icao.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(icao);
                    }
                    else if (valueVariant.canConvert<CAircraftIcaoCodeList>())
                    {
                        CAircraftIcaoCodeList icaos(valueVariant.value<CAircraftIcaoCodeList>());
                        if (icaos.size() != 1) { return; }
                        CAircraftIcaoCode icao = icaos.front();
                        if (icao.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(icao);
                    }
                    else if (valueVariant.canConvert<CLivery>())
                    {
                        CLivery livery = valueVariant.value<CLivery>();
                        if (livery.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(livery);
                    }
                    else if (valueVariant.canConvert<CLiveryList>())
                    {
                        CLiveryList liveries(valueVariant.value<CLiveryList>());
                        if (liveries.size() != 1) { return; }
                        CLivery livery = liveries.front();
                        if (livery.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(livery);
                    }
                    else if (valueVariant.canConvert<CDistributor>())
                    {
                        CDistributor distributor = valueVariant.value<CDistributor>();
                        if (distributor.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(distributor);
                    }
                    else if (valueVariant.canConvert<CDistributorList>())
                    {
                        CDistributorList distributors(valueVariant.value<CDistributorList>());
                        if (distributors.size() != 1) { return; }
                        CDistributor distributor = distributors.front();
                        if (distributor.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(distributor);
                    }
                    else if (valueVariant.canConvert<CAirlineIcaoCode>())
                    {
                        CAirlineIcaoCode airline = valueVariant.value<CAirlineIcaoCode>();
                        if (airline.validate().hasErrorMessages()) { return; }
                        emit requestHandlingOfStashDrop(airline); // I need to convert to stanard livery, which I can`t do here
                    }
                    else if (valueVariant.canConvert<CAirlineIcaoCodeList>())
                    {
                        CAirlineIcaoCodeList airlines(valueVariant.value<CAirlineIcaoCodeList>());
                        if (airlines.size() != 1) { return; }
                        CAirlineIcaoCode airline = airlines.front();
                        if (airline.validate().hasErrorMessages()) { return; }
                        emit requestHandlingOfStashDrop(airline); // I need to convert to stanard livery, which I can`t do here
                    }
                }
            } // valid mime?
        }

        void CAircraftModelView::customMenu(CMenuActions &menuActions)
        {
            bool used = false;
            if (this->m_menus.testFlag(MenuCanStashModels))
            {
                if (!this->m_menuFlagActions.contains(MenuCanStashModels))
                {
                    CMenuActions ma;
                    ma.addAction(CIcons::appDbStash16(), "Stash selected", CMenuAction::pathStash(), { this, &CAircraftModelView::ps_requestStash });
                    QAction *added = ma.addAction(CIcons::appDbStash16(), "Stashing clears selection (on/off)", CMenuAction::pathStash(), { this, &CAircraftModelView::ps_stashingClearsSelection });
                    added->setCheckable(true);
                    this->m_menuFlagActions.insert(MenuCanStashModels, ma);
                }

                // modify menu items
                const int selected = this->selectedRowCount();
                const bool tooMany = selected > 500;
                const bool canStash = selected > 0 && !tooMany;
                QAction *a = menuActions.addActions(initMenuActions(MenuCanStashModels)).first();
                a->setEnabled(canStash);
                a = menuActions.addActions(initMenuActions(MenuCanStashModels)).last();
                a->setChecked(m_stashingClearsSelection);
                used = true;
            }
            if (this->m_menus.testFlag(MenuHighlightStashed))
            {
                // this function requires that someone provides the model strings to be highlighted
                if (!this->m_menuFlagActions.contains(MenuHighlightStashed))
                {
                    CMenuActions ma;
                    QAction *added = ma.addAction(CIcons::appDbStash16(), "Highlight stashed (on/off)", CMenuAction::pathStash(), { this, &CAircraftModelView::ps_toggleHighlightStashedModels });
                    added->setCheckable(true);
                    this->m_menuFlagActions.insert(MenuHighlightStashed, ma);
                }
                QAction *a = menuActions.addActions(initMenuActions(CViewBaseNonTemplate::MenuHighlightStashed)).first();
                a->setChecked(this->derivedModel()->highlightModelStrings());
                used = true;
            }
            if (used) { menuActions.addMenuStash();}
            COrderableViewWithDbObjects::customMenu(menuActions);
        }

        CStatusMessage CAircraftModelView::modifyLoadedJsonData(CAircraftModelList &models) const
        {
            if (!this->m_jsonLoad.testFlag(ReduceToOneSimulator)) { return {}; }
            if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityDebug, "Empty models", true); }
            const CSimulatorInfo maxSims = models.simulatorsWithMaxEntries();
            if (maxSims.isNoSimulator())
            {
                return CStatusMessage(this, CStatusMessage::SeverityError, "No simulator with maximum, cannot reduce");
            }

            if (maxSims.isSingleSimulator())
            {
                int rm = models.removeIfNotMatchingSimulator(maxSims);
                return rm < 1 ?
                       CStatusMessage(this, CStatusMessage::SeverityInfo, "Now only for " + maxSims.toQString(true), true) :
                       CStatusMessage(this, CStatusMessage::SeverityInfo, "Reduced by % 1 to only use %2", true) << rm << maxSims.toQString(true);
            }

            // one simulator dominating
            if (maxSims.isSingleSimulator())
            {
                int rm = models.removeIfNotMatchingSimulator(maxSims);
                return rm < 1 ?
                       CStatusMessage(this, CStatusMessage::SeverityInfo, "Now only for " + maxSims.toQString(true), true) :
                       CStatusMessage(this, CStatusMessage::SeverityInfo, "Reduced by % 1 to only use %2", true) << rm << maxSims.toQString(true);
            }

            // multiple sims with same count
            const CSimulatorInfo first = CSimulatorInfoList::splitIntoSingleSimulators(maxSims).front();
            int d = models.removeIfNotMatchingSimulator(first);
            return d < 1 ?
                   CStatusMessage(this, CStatusMessage::SeverityInfo, "Now only for " + maxSims.toQString(true), true) :
                   CStatusMessage(this, CStatusMessage::SeverityInfo, "Reduced by % 1 to only use %2", true) << d << maxSims.toQString(true);
        }

        CStatusMessage CAircraftModelView::validateLoadedJsonData(const CAircraftModelList &models) const
        {
            static const CStatusMessage ok(this, CStatusMessage::SeverityInfo, "model validation passed", true);
            if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, "no data", true); }
            if (this->m_jsonLoad == AllowOnlySingleSimulator)
            {
                const CSimulatorInfo sim = models.simulatorsSupported();
                if (sim.isSingleSimulator()) { return ok; }
                return CStatusMessage(this, CStatusMessage::SeverityError, "data need to be from one simulator");
            }
            return COrderableViewWithDbObjects::validateLoadedJsonData(models);
        }

        void CAircraftModelView::jsonLoadedAndModelUpdated(const CAircraftModelList &models)
        {
            if (models.isEmpty())
            {
                emit jsonModelsForSimulatorLoaded(CSimulatorInfo());
            }
            else
            {
                emit jsonModelsForSimulatorLoaded(models.simulatorsWithMaxEntries());
            }
        }

        void CAircraftModelView::ps_toggleHighlightStashedModels()
        {
            bool h = derivedModel()->highlightModelStrings();
            derivedModel()->setHighlightModelStrings(!h);
            emit toggledHighlightStashedModels();
        }

        void CAircraftModelView::ps_toogleHighlightInvalidModels()
        {
            bool h = this->highlightModelStrings();
            this->setHighlightModelStrings(!h);
        }

        void CAircraftModelView::ps_stashingClearsSelection()
        {
            this->m_stashingClearsSelection = !this->m_stashingClearsSelection;
        }

        void CAircraftModelView::ps_requestStash()
        {
            if (!m_menus.testFlag(MenuCanStashModels)) { return; }
            if (!this->hasSelection()) { return; }
            const CAircraftModelList models(this->selectedObjects());
            emit requestStash(models);
            if (this->m_stashingClearsSelection)
            {
                this->clearSelection();
            }
            sGui->displayInStatusBar(CStatusMessage(CStatusMessage::SeverityInfo, "Stashed " + models.getModelStringList(true).join(" ")));
        }
    } // namespace
} // namespace
