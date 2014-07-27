/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "atcstationview.h"
#include <QHeaderView>

using namespace BlackMisc;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CAtcStationView::CAtcStationView(QWidget *parent) : CViewBase(parent)
        {
            this->m_model = new CAtcStationListModel(CAtcStationListModel::StationsOnline, this);
            this->setModel(this->m_model); // via QTableView
            this->m_model->setSortColumnByPropertyIndex(BlackMisc::Aviation::CAtcStation::IndexDistance);
            if (this->m_model->hasValidSortColumn())
                this->horizontalHeader()->setSortIndicator(
                    this->m_model->getSortColumn(),
                    this->m_model->getSortOrder());
        }

        void CAtcStationView::setStationMode(CAtcStationListModel::AtcStationMode stationMode)
        {
            Q_ASSERT(this->m_model);
            this->m_model->setStationMode(stationMode);
        }

        void CAtcStationView::changedAtcStationConnectionStatus(const Aviation::CAtcStation &station, bool added)
        {
            this->m_model->changedAtcStationConnectionStatus(station, added);
            this->resizeColumnsToContents();
            this->resizeRowsToContents();
        }
    }
}
