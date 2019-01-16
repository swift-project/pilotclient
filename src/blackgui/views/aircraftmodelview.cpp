/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelview.h"
#include "viewbase.h"
#include "aircraftmodelstatisticsdialog.h"
#include "blackgui/filters/aircraftmodelfilterdialog.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/shortcut.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatorinfolist.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/directories.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/icons.h"
#include "blackmisc/variant.h"

#include <QAction>
#include <QDropEvent>
#include <QMap>
#include <QShortcut>
#include <QString>
#include <QWidget>
#include <QtGlobal>
#include <QStringBuilder>

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
            this->setSettingsDirectoryIndex(CDirectories::IndexDirLastModelJsonOrDefault);

            // shortcut
            QShortcut *stashShortcut = new QShortcut(CShortcut::keyStash(), this);
            stashShortcut->setContext(Qt::WidgetShortcut);
            connect(stashShortcut, &QShortcut::activated, this, &CAircraftModelView::requestedStash);

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
            case CAircraftModelListModel::StashModel: m_menus = MenuDefaultNoClear; break;
            case CAircraftModelListModel::Database: m_menus = MenuDefaultDbViews; break;
            case CAircraftModelListModel::VPilotRuleModel: m_menus = MenuDefaultNoClear | MenuStashing; break;
            case CAircraftModelListModel::OwnAircraftModelMappingTool: m_menus = MenuDefaultNoClear | MenuStashing | MenuLoadAndSave; break;
            case CAircraftModelListModel::OwnAircraftModelClient:
            default:
                m_menus = MenuDefaultNoClear | MenuBackend;
                break;
            }
        }

        int CAircraftModelView::applyToSelected(const CLivery &livery)
        {
            if (!hasSelection()) { return 0; }
            const int c = this->updateSelected(CVariant::from(livery), CAircraftModel::IndexLivery);
            return c;
        }

        int CAircraftModelView::applyToSelected(const CAircraftIcaoCode &icao)
        {
            if (!hasSelection()) { return 0; }
            const int c = this->updateSelected(CVariant::from(icao), CAircraftModel::IndexAircraftIcaoCode);
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
            const int c = this->updateSelected(vm);
            return c;
        }

        bool CAircraftModelView::hasSelectedModelsToStash() const
        {
            return m_menus.testFlag(MenuCanStashModels) && hasSelection();
        }

        void CAircraftModelView::setAcceptedMetaTypeIds()
        {
            Q_ASSERT(m_model);
            m_model->setAcceptedMetaTypeIds(
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
                const CVariant valueVariant(CGuiUtility::fromSwiftDragAndDropData(mime));
                if (valueVariant.isValid())
                {
                    if (valueVariant.canConvert<CAircraftModel>())
                    {
                        const CAircraftModel model = valueVariant.value<CAircraftModel>();
                        if (!model.hasModelString()) { return; }
                        const CAircraftModelList models({model});
                        this->derivedModel()->replaceOrAddByModelString(models);
                        return;
                    }
                    else if (valueVariant.canConvert<CAircraftModelList>())
                    {
                        const CAircraftModelList models(valueVariant.value<CAircraftModelList>());
                        if (models.isEmpty()) { return; }
                        this->derivedModel()->replaceOrAddByModelString(models);
                        return;
                    }

                    // only for selected members
                    if (!this->hasSelection()) { return; }
                    if (valueVariant.canConvert<CAircraftIcaoCode>())
                    {
                        const CAircraftIcaoCode icao = valueVariant.value<CAircraftIcaoCode>();
                        if (icao.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(icao);
                    }
                    else if (valueVariant.canConvert<CAircraftIcaoCodeList>())
                    {
                        const CAircraftIcaoCodeList icaos(valueVariant.value<CAircraftIcaoCodeList>());
                        if (icaos.size() != 1) { return; }
                        const CAircraftIcaoCode icao = icaos.front();
                        if (icao.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(icao);
                    }
                    else if (valueVariant.canConvert<CLivery>())
                    {
                        const CLivery livery = valueVariant.value<CLivery>();
                        if (livery.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(livery);
                    }
                    else if (valueVariant.canConvert<CLiveryList>())
                    {
                        const CLiveryList liveries(valueVariant.value<CLiveryList>());
                        if (liveries.size() != 1) { return; }
                        const CLivery livery = liveries.front();
                        if (livery.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(livery);
                    }
                    else if (valueVariant.canConvert<CDistributor>())
                    {
                        const CDistributor distributor = valueVariant.value<CDistributor>();
                        if (distributor.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(distributor);
                    }
                    else if (valueVariant.canConvert<CDistributorList>())
                    {
                        const CDistributorList distributors(valueVariant.value<CDistributorList>());
                        if (distributors.size() != 1) { return; }
                        const CDistributor distributor = distributors.front();
                        if (distributor.validate().hasErrorMessages()) { return; }
                        this->applyToSelected(distributor);
                    }
                    else if (valueVariant.canConvert<CAirlineIcaoCode>())
                    {
                        const CAirlineIcaoCode airline = valueVariant.value<CAirlineIcaoCode>();
                        if (airline.validate().hasErrorMessages()) { return; }
                        emit requestHandlingOfStashDrop(airline); // I need to convert to stanard livery, which I can`t do here
                    }
                    else if (valueVariant.canConvert<CAirlineIcaoCodeList>())
                    {
                        const CAirlineIcaoCodeList airlines(valueVariant.value<CAirlineIcaoCodeList>());
                        if (airlines.size() != 1) { return; }
                        const CAirlineIcaoCode airline = airlines.front();
                        if (airline.validate().hasErrorMessages()) { return; }
                        emit requestHandlingOfStashDrop(airline); // I need to convert to stanard livery, which I can`t do here
                    }
                }
            }
            else
            {
                CViewBase::dropEvent(event);
            }
        }

        void CAircraftModelView::customMenu(CMenuActions &menuActions)
        {
            // Statistics
            if (!this->isEmpty())
            {
                menuActions.addAction(CIcons::appAircraft16(), "Model statistics", CMenuAction::pathModel(), { this, &CAircraftModelView::displayModelStatisticsDialog });
            }

            // Stash menus
            bool addStashMenu = false;
            if (m_menus.testFlag(MenuCanStashModels))
            {
                if (!m_menuFlagActions.contains(MenuCanStashModels))
                {
                    CMenuActions ma;
                    ma.addAction(CIcons::appDbStash16(), "Stash selected", CMenuAction::pathModelStash(), { this, &CAircraftModelView::requestedStash });
                    QAction *added = ma.addAction(CIcons::appDbStash16(), "Stashing clears selection (on/off)", CMenuAction::pathModelStash(), { this, &CAircraftModelView::stashingClearsSelection });
                    added->setCheckable(true);
                    m_menuFlagActions.insert(MenuCanStashModels, ma);
                }

                // modify menu items
                const int selected = this->selectedRowCount();
                const bool tooMany = selected > 500;
                const bool canStash = selected > 0 && !tooMany;
                QAction *a = menuActions.addActions(initMenuActions(MenuCanStashModels)).first();
                a->setEnabled(canStash);
                a = menuActions.addActions(initMenuActions(MenuCanStashModels)).last();
                a->setChecked(m_stashingClearsSelection);
                addStashMenu = true;
            }
            if (m_menus.testFlag(MenuHighlightStashed))
            {
                // this function requires that someone provides the model strings to be highlighted
                if (!m_menuFlagActions.contains(MenuHighlightStashed))
                {
                    CMenuActions ma;
                    QAction *added = ma.addAction(CIcons::appDbStash16(), "Highlight stashed (on/off)", CMenuAction::pathModelStash(), { this, &CAircraftModelView::toggleHighlightStashedModels });
                    added->setCheckable(true);
                    m_menuFlagActions.insert(MenuHighlightStashed, ma);
                }
                QAction *a = menuActions.addActions(initMenuActions(CViewBaseNonTemplate::MenuHighlightStashed)).first();
                a->setChecked(this->derivedModel()->highlightModelStrings());
                addStashMenu = true;
            }

            // client specific
            if (m_menus.testFlag(MenuDisableModelsTemp) && this->hasSelection())
            {
                if (!m_menuFlagActions.contains(MenuDisableModelsTemp))
                {
                    CMenuActions ma;
                    ma.addAction(CIcons::delete16(), "Temp.disable model", CMenuAction::pathModel(), { this, &CAircraftModelView::requestTempDisable });
                    m_menuFlagActions.insert(MenuDisableModelsTemp, ma);
                }
                menuActions.addActions(initMenuActions(CViewBaseNonTemplate::MenuDisableModelsTemp));
            }

            if (addStashMenu) { menuActions.addMenuStash(); }

            // base class menus
            COrderableViewWithDbObjects::customMenu(menuActions);
        }

        CStatusMessage CAircraftModelView::modifyLoadedJsonData(CAircraftModelList &models) const
        {
            if (m_loadingRequiresSimulator.isNoSimulator()) { return {}; }
            if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityDebug, "Empty models", true); }

            // multiple sims with same count
            const int removed = models.removeIfNotMatchingSimulator(m_loadingRequiresSimulator);
            if (removed < 1) { return {}; }
            return CStatusMessage(this, CStatusMessage::SeverityWarning, "Reduced by %1 model(s) to only use %2 models", true) << removed << m_loadingRequiresSimulator.toQString(true);
        }

        CStatusMessage CAircraftModelView::validateLoadedJsonData(const CAircraftModelList &models) const
        {
            if (models.isEmpty()) { return COrderableViewWithDbObjects::validateLoadedJsonData(models); }
            if (m_loadingRequiresSimulator.isNoSimulator()) { return COrderableViewWithDbObjects::validateLoadedJsonData(models); }
            if (models.containsNotMatchingSimulator(m_loadingRequiresSimulator))
            {
                return CStatusMessage(this, CStatusMessage::SeverityError, "Found entry not matching %1 in model data", true) << m_loadingRequiresSimulator.toQString();
            }
            return COrderableViewWithDbObjects::validateLoadedJsonData(models);
        }

        void CAircraftModelView::jsonLoadedAndModelUpdated(const CAircraftModelList &models)
        {
            if (models.isEmpty())
            {
                emit this->jsonModelsForSimulatorLoaded(CSimulatorInfo());
            }
            else
            {
                emit this->jsonModelsForSimulatorLoaded(models.simulatorsWithMaxEntries());
            }
        }

        void CAircraftModelView::toggleHighlightStashedModels()
        {
            const bool h = derivedModel()->highlightModelStrings();
            derivedModel()->setHighlightModelStrings(!h);
            emit toggledHighlightStashedModels();
        }

        void CAircraftModelView::toggleHighlightInvalidModels()
        {
            const bool h = this->highlightModelStrings();
            this->setHighlightModelStrings(!h);
        }

        void CAircraftModelView::stashingClearsSelection()
        {
            m_stashingClearsSelection = !m_stashingClearsSelection;
        }

        void CAircraftModelView::requestedStash()
        {
            if (!m_menus.testFlag(MenuCanStashModels)) { return; }
            if (!this->hasSelection()) { return; }
            const CAircraftModelList models(this->selectedObjects());
            emit this->requestStash(models);
            if (m_stashingClearsSelection)
            {
                this->clearSelection();
            }
            sGui->displayInStatusBar(CStatusMessage(CStatusMessage::SeverityInfo, "Stashed " + models.getModelStringList(true).join(" ")));
        }

        void CAircraftModelView::requestTempDisable()
        {
            if (!m_menus.testFlag(MenuDisableModelsTemp)) { return; }
            if (!this->hasSelection()) { return; }
            const CAircraftModelList models(this->selectedObjects());
            emit this->requestTempDisableModelsForMatching(models);
            sGui->displayInStatusBar(CStatusMessage(CStatusMessage::SeverityInfo, "Temp.disabled " + models.getModelStringList(true).join(" ")));
        }

        void CAircraftModelView::displayModelStatisticsDialog()
        {
            if (!m_statisticsDialog)
            {
                m_statisticsDialog = new CAircraftModelStatisticsDialog(this);
            }

            m_statisticsDialog->analyzeModels(this->container());
            m_statisticsDialog->exec();
        }
    } // namespace
} // namespace
