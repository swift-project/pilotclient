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
            this->m_withMenuItemClear = true;
            this->m_withMenuItemRefresh = true;
        }

        void CAtcStationView::setStationMode(CAtcStationListModel::AtcStationMode stationMode)
        {
            Q_ASSERT(this->m_model);
            this->m_model->setStationMode(stationMode);
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
                menu.addAction(CIcons::appTextMessages16(), "Show text messages", this, SLOT(ps_requestTextMessage()));
                menu.addSeparator();
            }
            CViewBase::customMenu(menu);
        }

        void CAtcStationView::ps_tuneInAtcCom1()
        {
            CAtcStation s(this->selectedObject());
            if (s.getCallsign().isEmpty()) { return; }
            emit this->requestComFrequency(s.getFrequency(), CComSystem::Com1);
        }

        void CAtcStationView::ps_tuneInAtcCom2()
        {
            CAtcStation s(this->selectedObject());
            if (s.getCallsign().isEmpty()) { return; }
            emit this->requestComFrequency(s.getFrequency(), CComSystem::Com2);
        }

        void CAtcStationView::ps_requestTextMessage()
        {
            CAtcStation s(this->selectedObject());
            if (s.getCallsign().isEmpty()) { return; }
            emit this->requestTextMessage(s.getCallsign());
        }

    } // namespace
} // namespace
