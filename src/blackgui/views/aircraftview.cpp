/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftview.h"
#include <QHeaderView>

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CAircraftView::CAircraftView(QWidget *parent) : CViewBase(parent)
        {
            this->m_model = new CAircraftListModel(this);
            this->setModel(this->m_model); // via QTableView
            this->m_model->setSortColumnByPropertyIndex(BlackMisc::Aviation::CAircraft::IndexDistance);
            if (this->m_model->hasValidSortColumn())
                this->horizontalHeader()->setSortIndicator(
                    this->m_model->getSortColumn(),
                    this->m_model->getSortOrder());
        }
    }
}
