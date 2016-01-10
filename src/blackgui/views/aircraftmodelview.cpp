/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelview.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/simulation//distributorlist.h"
#include "blackgui/shortcut.h"
#include "blackgui/guiutility.h"
#include "blackgui/filters/aircraftmodelfilterdialog.h"
#include <QHeaderView>
#include <QShortcut>
#include <iostream>
#include <memory>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;
using namespace BlackGui::Filters;

namespace BlackGui
{
    namespace Views
    {
        CAircraftModelView::CAircraftModelView(QWidget *parent) : CViewWithDbObjects(parent)
        {
            // default
            this->standardInit(new CAircraftModelListModel(CAircraftModelListModel::OwnSimulatorModel, this));

            // shortcut
            new QShortcut(CShortcut::keyStash(), this, SLOT(ps_requestStash()), nullptr, Qt::WidgetShortcut);

            // default mode
            CAircraftModelListModel::AircraftModelMode mode = derivedModel()->getModelMode();
            this->setAircraftModelMode(mode);
        }

        void CAircraftModelView::setAircraftModelMode(CAircraftModelListModel::AircraftModelMode mode)
        {
            this->setCustomMenu(nullptr, false); // delete everything
            derivedModel()->setAircraftModelMode(mode);
            switch (mode)
            {
            case CAircraftModelListModel::StashModel:
                this->m_menus = MenuClear | MenuHighlightDbData;
                break;
            case CAircraftModelListModel::Database:
                this->m_menus = MenuBackend;
                break;
            case CAircraftModelListModel::VPilotRuleModel:
                this->m_menus = MenuRefresh | MenuStashing | MenuToggleSelectionMode;
                break;
            case CAircraftModelListModel::OwnSimulatorModelMapping:
                this->m_menus = MenuDisplayAutomatically | MenuStashing | MenuHighlightDbData | MenuToggleSelectionMode;
                break;
            case CAircraftModelListModel::OwnSimulatorModel:
            default:
                this->m_menus = MenuDisplayAutomatically | MenuBackend | MenuRefresh | MenuHighlightDbData;
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

        bool CAircraftModelView::hasSelectedModelsToStash() const
        {
            return m_menus.testFlag(MenuCanStashModels) && hasSelection();
        }

        void CAircraftModelView::setImplementedMetaTypeIds()
        {
            this->setAcceptedMetaTypeIds(
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

        int CAircraftModelView::removeModelsWithModelString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity)
        {
            if (modelStrings.isEmpty()) { return 0; }
            CAircraftModelList copy(this->container());
            int delta = copy.removeModelsWithString(modelStrings, sensitivity);
            if (delta > 0)
            {
                this->updateContainerMaybeAsync(copy);
            }
            return delta;
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

        bool CAircraftModelView::highlightModelsStrings() const
        {
            return this->derivedModel()->highlightModelStrings();
        }

        void CAircraftModelView::dropEvent(QDropEvent *event)
        {
            if (!isDropAllowed()) { return; }
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

        void CAircraftModelView::customMenu(QMenu &menu) const
        {
            if (this->m_menus.testFlag(MenuCanStashModels))
            {
                menu.addAction(CIcons::appDbStash16(), "Stash", this, SLOT(ps_requestStash()));
                QAction *a = menu.addAction(CIcons::appDbStash16(), "Stashing clears selection", this, SLOT(ps_stashingClearsSelection()));
                a->setCheckable(true);
                a->setChecked(m_stashingClearsSelection);
            }
            if (this->m_menus.testFlag(MenuHighlightStashed))
            {
                // this function requires that someone provides the model strings to be highlighted
                QAction *a = menu.addAction(CIcons::appDbStash16(), "Highlight stashed", this, SLOT(ps_toggleHighlightStashedModels()));
                a->setCheckable(true);
                a->setChecked(this->derivedModel()->highlightDbData());
            }
            if (this->m_menus.testFlag(MenuHighlightInvalid))
            {
                // this function requires that someone provides the model strings to be highlighted
                QAction *a = menu.addAction(CIcons::appDbStash16(), "Highlight invalid models", this, SLOT(ps_to));
                a->setCheckable(true);
                a->setChecked(this->derivedModel()->highlightDbData());
            }
            CViewWithDbObjects::customMenu(menu);
        }

        void CAircraftModelView::ps_toggleHighlightStashedModels()
        {
            bool h = derivedModel()->highlightModelStrings();
            derivedModel()->setHighlightModelStrings(!h);
            emit toggledHighlightStashedModels();
        }

        void CAircraftModelView::ps_toogleHighlightInvalidModels()
        {
            bool h = this->highlightModelsStrings();
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
            emit requestStash(this->selectedObjects());
            if (this->m_stashingClearsSelection)
            {
                this->clearSelection();
            }
        }
    } // namespace
} // namespace
