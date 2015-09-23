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
using namespace BlackGui::Models;
using namespace BlackGui::Filters;

namespace BlackGui
{
    namespace Views
    {
        CAircraftModelView::CAircraftModelView(QWidget *parent) : CViewBase(parent)
        {
            this->standardInit(new CAircraftModelListModel(CAircraftModelListModel::OwnSimulatorModel, this));

            // filter
            QWidget *mainWindow = this->mainApplicationWindowWidget();
            Q_ASSERT_X(mainWindow, Q_FUNC_INFO, "no main window found");
            this->setFilterDialog(new CAircraftModelFilterDialog(mainWindow));
        }

        void CAircraftModelView::setAircraftModelMode(CAircraftModelListModel::AircraftModelMode mode)
        {
            if (mode == CAircraftModelListModel::Database)
            {
                this->m_withMenuItemClear = false;
                this->m_withMenuItemRefresh = false;
                this->m_withMenuItemBackend = true;
            }
            else if (mode == CAircraftModelListModel::OwnSimulatorModel)
            {
                this->m_withMenuItemClear = false;
                this->m_withMenuItemRefresh = false;
                this->m_withMenuItemBackend = true;
            }
            else if (mode == CAircraftModelListModel::VPilotRuleModel)
            {
                this->m_withMenuItemClear = true;
                this->m_withMenuItemRefresh = false;
                this->m_withMenuItemBackend = false;
            }
            else
            {
                this->m_withMenuItemClear = true;
                this->m_withMenuItemRefresh = true;
                this->m_withMenuItemBackend = true;
            }
            this->m_model->setAircraftModelMode(mode);
        }

        bool CAircraftModelView::displayAutomatically() const
        {
            return m_displayAutomatically;
        }

        void CAircraftModelView::customMenu(QMenu &menu) const
        {
            CAircraftModelListModel::AircraftModelMode mode =  this->m_model->getModelMode();
            if (mode == CAircraftModelListModel::VPilotRuleModel || mode == CAircraftModelListModel::OwnSimulatorModel)
            {
                QAction *a = menu.addAction(CIcons::appMappings16(), "Automatically display", this, SLOT(ps_toggleAutoDisplay()));
                a->setCheckable(true);
                a->setChecked(m_displayAutomatically);
                menu.addSeparator();
                a = menu.addAction(CIcons::database16(), "Highlight DB items", this, SLOT(ps_toggleHighlightDbModels()));
                a->setCheckable(true);
                a->setChecked(derivedModel()->highlightDbData());
            }
            CViewBase::customMenu(menu);
        }

        void CAircraftModelView::ps_toggleAutoDisplay()
        {
            QAction *a = qobject_cast<QAction *>(QObject::sender());
            if (!a) { return; }
            Q_ASSERT_X(a->isCheckable(), Q_FUNC_INFO, "object not checkable");
            this->m_displayAutomatically = a->isChecked();
        }

        void CAircraftModelView::ps_toggleHighlightDbModels()
        {
            bool h = derivedModel()->highlightDbData();
            derivedModel()->setHighlightDbData(!h);
        }

    } // namespace
} // namespace
