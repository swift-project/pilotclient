/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelview.h"
#include "blackgui/filters/aircraftmodelfilterdialog.h"
#include <QHeaderView>
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
        CAircraftModelView::CAircraftModelView(QWidget *parent) : CViewBase(parent)
        {
            // default
            this->standardInit(new CAircraftModelListModel(CAircraftModelListModel::OwnSimulatorModel, this));

            // filter
            QWidget *mainWindow = this->mainApplicationWindowWidget();
            Q_ASSERT_X(mainWindow, Q_FUNC_INFO, "no main window found");
            this->setFilterDialog(new CAircraftModelFilterDialog(mainWindow));

            // default mode
            CAircraftModelListModel::AircraftModelMode mode = derivedModel()->getModelMode();
            this->setAircraftModelMode(mode);
        }

        void CAircraftModelView::setAircraftModelMode(CAircraftModelListModel::AircraftModelMode mode)
        {
            this->m_withMenuDisplayAutomatically = false;
            this->setCustomMenu(nullptr, false); // delete everything
            switch (mode)
            {
            case CAircraftModelListModel::StashModel:
                this->m_withMenuItemClear = true;
                this->m_withMenuItemRefresh = false;
                this->m_withMenuItemBackend = false;
                this->setCustomMenu(new CHighlightDbModelsMenu(this, true));
                break;
            case CAircraftModelListModel::Database:
                this->m_withMenuItemClear = false;
                this->m_withMenuItemRefresh = false;
                this->m_withMenuItemBackend = true;
                break;
            case CAircraftModelListModel::VPilotRuleModel:
                this->m_withMenuItemClear = false;
                this->m_withMenuItemRefresh = true;
                this->m_withMenuItemBackend = false;
                this->setCustomMenu(new CHighlightDbModelsMenu(this, true));
                this->setCustomMenu(new CHighlightStashedModelsMenu(this, true));
                break;
            case CAircraftModelListModel::OwnSimulatorModelMapping:
                this->m_withMenuDisplayAutomatically = true;
                this->m_withMenuItemClear = false;
                this->m_withMenuItemRefresh = false;
                this->m_withMenuItemBackend = false;
                this->setCustomMenu(new CHighlightDbModelsMenu(this, true));
                this->setCustomMenu(new CHighlightStashedModelsMenu(this, true));
                break;
            case CAircraftModelListModel::OwnSimulatorModel:
            default:
                this->m_withMenuDisplayAutomatically = true;
                this->m_withMenuItemClear = false;
                this->m_withMenuItemRefresh = true;
                this->m_withMenuItemBackend = true;
                this->setCustomMenu(new CHighlightDbModelsMenu(this, true));
                break;
            }
        }

        void CAircraftModelView::applyToSelected(const CLivery &livery)
        {
            if (!hasSelection()) { return; }
            int c = this->updateSelected(CVariant::from(livery), CAircraftModel::IndexLivery);
            // this->updateContainer(models);
        }

        void CAircraftModelView::applyToSelected(const CAircraftIcaoCode &icao)
        {
            if (!hasSelection()) { return; }
            int c = this->updateSelected(CVariant::from(icao), CAircraftModel::IndexAircraftIcaoCode);
        }

        void CAircraftModelView::applyToSelected(const CDistributor &distributor)
        {
            if (!hasSelection()) { return; }
            int c = this->updateSelected(CVariant::from(distributor), CAircraftModel::IndexDistributor);
        }

        void CAircraftModelView::ps_toggleHighlightDbModels()
        {
            bool h = derivedModel()->highlightDbData();
            derivedModel()->setHighlightDbData(!h);
        }

        void CAircraftModelView::ps_toggleHighlightStashedModels()
        {
            bool h = derivedModel()->highlightGivenModelStrings();
            derivedModel()->setHighlightModelsStrings(!h);
        }

        void CAircraftModelView::CHighlightDbModelsMenu::customMenu(QMenu &menu) const
        {
            const CAircraftModelView *mv = qobject_cast<const CAircraftModelView *>(parent());
            Q_ASSERT_X(mv, Q_FUNC_INFO, "no view");
            QAction *a = menu.addAction(CIcons::database16(), "Highlight DB models", mv, SLOT(ps_toggleHighlightDbModels()));
            a->setCheckable(true);
            a->setChecked(mv->derivedModel()->highlightDbData());
            this->nestedCustomMenu(menu);
        }

        void CAircraftModelView::CHighlightStashedModelsMenu::customMenu(QMenu &menu) const
        {
            const CAircraftModelView *mv = qobject_cast<const CAircraftModelView *>(parent());
            Q_ASSERT_X(mv, Q_FUNC_INFO, "no view");
            QAction *a = menu.addAction(CIcons::appDbStash16(), "Highlight stashed models", mv, SLOT(ps_toggleHighlightStashedModels()));
            a->setCheckable(true);
            a->setChecked(mv->derivedModel()->highlightGivenModelStrings());
            this->nestedCustomMenu(menu);
        }

    } // namespace
} // namespace
