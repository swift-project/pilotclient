/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelview.h"
#include "aircraftmodelfilterform.h"
#include <QHeaderView>
#include <iostream>
#include <memory>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CAircraftModelView::CAircraftModelView(QWidget *parent) : CViewBase(parent)
        {
            this->m_withMenuItemClear = true;
            this->m_withMenuItemRefresh = true;
            this->standardInit(new CAircraftModelListModel(CAircraftModelListModel::ModelOnly, this));

            // filter
            QWidget *mainWindow = this->mainApplicationWindowWidget();
            Q_ASSERT_X(mainWindow, Q_FUNC_INFO, "no main window found");
            this->setFilterDialog(new CAircraftModelFilterForm(mainWindow));
        }

        void CAircraftModelView::setAircraftModelMode(CAircraftModelListModel::AircraftModelMode mode)
        {
            this->m_model->setAircraftModelMode(mode);
        }

        bool CAircraftModelView::displayAutomatically() const
        {
            return m_displayAutomatically;
        }

        bool CAircraftModelView::ps_filterDialogFinished(int status)
        {
            if (CViewBase::ps_filterDialogFinished(status)) { return true; }
            if (!this->m_filterDialog) { this->derivedModel()->removeFilter(); return true; }
            std::unique_ptr<IModelFilter<CAircraftModelList>> filter(this->getFilterForm()->getFilter());
            this->derivedModel()->setFilter(filter);
            return true;
        }

        void CAircraftModelView::customMenu(QMenu &menu) const
        {
            QAction *a = menu.addAction(CIcons::appMappings16(), "Automatically display", this, SLOT(ps_toggleAutoDisplay()));
            a->setCheckable(true);
            a->setChecked(m_displayAutomatically);
            menu.addAction(CIcons::refresh16(), "Reload model data", this, SIGNAL(requestModelReload()));
            menu.addSeparator();
            CViewBase::customMenu(menu);
        }

        void CAircraftModelView::ps_toggleAutoDisplay()
        {
            QAction *a = qobject_cast<QAction *>(QObject::sender());
            if (!a) { return; }
            Q_ASSERT(a->isCheckable());
            this->m_displayAutomatically = a->isChecked();
        }

        CAircraftModelFilterForm *CAircraftModelView::getFilterForm() const
        {
            return static_cast<CAircraftModelFilterForm *>(this->m_filterDialog.data());
        }

    } // namespace
} // namespace
