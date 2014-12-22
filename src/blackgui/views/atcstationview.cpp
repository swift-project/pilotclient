/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "atcstationview.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/testing.h"
#include <QHeaderView>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CAtcStationView::CAtcStationView(QWidget *parent) : CViewBase(parent)
        {
            this->standardInit(new CAtcStationListModel(CAtcStationListModel::StationsOnline, this));
        }

        void CAtcStationView::setStationMode(CAtcStationListModel::AtcStationMode stationMode)
        {
            Q_ASSERT(this->m_model);
            this->m_model->setStationMode(stationMode);
            this->m_withMenuItemClear = true;
            this->m_withMenuItemRefresh = true;

            switch (stationMode)
            {
            case CAtcStationListModel::NotSet:
            case CAtcStationListModel::StationsOnline:
                this->m_model->setSortColumnByPropertyIndex(BlackMisc::Aviation::CAtcStation::IndexDistance);
                break;
            case CAtcStationListModel::StationsBooked:
                this->m_model->setSortColumnByPropertyIndex(BlackMisc::Aviation::CAtcStation::IndexBookedFrom);
                break;
            default:
                break;
            }
            this->setSortIndicator();
        }

        void CAtcStationView::changedAtcStationConnectionStatus(const Aviation::CAtcStation &station, bool added)
        {
            this->m_model->changedAtcStationConnectionStatus(station, added);
            this->resizeToContents();
        }

        void CAtcStationView::customMenu(QMenu &menu) const
        {
            if (BlackMisc::CProject::isDebugBuild())
            {
                menu.addAction(CIcons::tableSheet16(), "Test: 1k ATC online stations", this, SLOT(ps_testRequest1kAtcOnlineDummies()));
                menu.addAction(CIcons::tableSheet16(), "Test: 3k ATC online stations", this, SLOT(ps_testRequest3kAtcOnlineDummies()));
                menu.addSeparator();
            }

            if (this->hasSelection())
            {
                menu.addAction(CIcons::appCockpit16(), "Tune in COM1", this, SLOT(ps_tuneInAtcCom1()));
                menu.addAction(CIcons::appCockpit16(), "Tune in COM2", this, SLOT(ps_tuneInAtcCom2()));
                menu.addSeparator();
            }
            CViewBase::customMenu(menu);
        }

        void CAtcStationView::ps_tuneInAtcCom1()
        {
            CAtcStationList l = this->selectedObjects();
            if (l.isEmpty()) { return; }
            emit this->requestComFrequency(l.front().getFrequency(), CComSystem::Com1);
        }

        void CAtcStationView::ps_tuneInAtcCom2()
        {
            CAtcStationList l = this->selectedObjects();
            if (l.isEmpty()) { return; }
            emit this->requestComFrequency(l.front().getFrequency(), CComSystem::Com2);
        }

    } // namespace
} // namespace
