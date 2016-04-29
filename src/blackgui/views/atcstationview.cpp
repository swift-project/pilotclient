/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "atcstationview.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/testing.h"
#include "blackmisc/buildconfig.h"
#include <QHeaderView>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;
using namespace BlackGui::Menus;


namespace BlackGui
{
    namespace Views
    {
        CAtcStationView::CAtcStationView(QWidget *parent) : CViewBase(parent)
        {
            this->standardInit(new CAtcStationListModel(CAtcStationListModel::StationsOnline, this));
            this->m_menus |= (MenuClear | MenuRefresh);
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

        void CAtcStationView::customMenu(CMenuActions &menuActions)
        {
            if (BlackMisc::CBuildConfig::isDebugBuild())
            {
                if (this->m_debugActions.isEmpty()) { this->m_actions = QList<QAction *>({nullptr, nullptr}); }
                this->m_actions[0] = menuActions.addAction(this->m_actions[0], CIcons::tableSheet16(), "Test: 1k ATC online stations", CMenuAction::pathClientCom(), { this, &CAtcStationView::ps_testRequest1kAtcOnlineDummies });
                this->m_actions[1] = menuActions.addAction(this->m_actions[1], CIcons::tableSheet16(), "Test: 3k ATC online stations", CMenuAction::pathClientCom(), { this, &CAtcStationView::ps_testRequest1kAtcOnlineDummies });
            }

            if (this->hasSelection())
            {
                if (this->m_debugActions.isEmpty()) { this->m_debugActions = QList<QAction *>({nullptr, nullptr, nullptr}); }

                this->m_debugActions[0] = menuActions.addAction(this->m_debugActions[0], CIcons::appCockpit16(), "Tune in COM1", CMenuAction::pathClientCom(), { this, &CAtcStationView::ps_tuneInAtcCom1 });
                this->m_debugActions[1] = menuActions.addAction(this->m_debugActions[1], CIcons::appCockpit16(), "Tune in COM2", CMenuAction::pathClientCom(), { this, &CAtcStationView::ps_tuneInAtcCom2 });
                this->m_debugActions[2] = menuActions.addAction(this->m_debugActions[2], CIcons::appTextMessages16(), "Show text messages", CMenuAction::pathClientCom(), { this, &CAtcStationView::ps_requestTextMessage });
            }
            CViewBase::customMenu(menuActions);
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
            emit this->requestTextMessageWidget(s.getCallsign());
        }

    } // namespace
} // namespace
