/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/views/atcstationview.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/icons.h"

#include <QFlags>
#include <QtGlobal>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;
using namespace BlackGui::Menus;

namespace BlackGui::Views
{
    CAtcStationView::CAtcStationView(QWidget *parent) : CViewWithCallsignObjects(parent)
    {
        this->standardInit(new CAtcStationListModel(CAtcStationListModel::StationsOnline, this));
        m_menus |= (MenuClear | MenuRefresh);
    }

    void CAtcStationView::setStationMode(CAtcStationListModel::AtcStationMode stationMode)
    {
        Q_ASSERT(m_model);
        m_model->setStationMode(stationMode);
        this->setSortIndicator();
    }

    void CAtcStationView::changedAtcStationConnectionStatus(const CAtcStation &station, bool added)
    {
        m_model->changedAtcStationConnectionStatus(station, added);

        // resize the first, rest will be resized with normal updates
        if (this->rowCount() == 1)
        {
            this->fullResizeToContents();
        }
    }

    void CAtcStationView::customMenu(CMenuActions &menuActions)
    {
        menuActions.addMenuCom();
        if (CBuildConfig::isDebugBuild())
        {
            if (m_debugActions.isEmpty()) { m_debugActions = QList<QAction *>({nullptr, nullptr}); }
            m_debugActions[0] = menuActions.addAction(m_debugActions[0], CIcons::tableSheet16(), "Test: 1k ATC online stations", CMenuAction::pathClientCom(), { this, &CAtcStationView::emitTestRequest1kAtcOnlineDummies });
            m_debugActions[1] = menuActions.addAction(m_debugActions[1], CIcons::tableSheet16(), "Test: 3k ATC online stations", CMenuAction::pathClientCom(), { this, &CAtcStationView::emitTestRequest3kAtcOnlineDummies });
        }

        if (this->hasSelection())
        {
            if (m_actions.isEmpty()) { m_actions = QList<QAction *>({nullptr, nullptr, nullptr}); }

            m_actions[0] = menuActions.addAction(m_actions[0], CIcons::appCockpit16(), "Tune in COM1", CMenuAction::pathClientCom(), { this, &CAtcStationView::tuneInAtcCom1 });
            m_actions[1] = menuActions.addAction(m_actions[1], CIcons::appCockpit16(), "Tune in COM2", CMenuAction::pathClientCom(), { this, &CAtcStationView::tuneInAtcCom2 });
            m_actions[2] = menuActions.addAction(m_actions[2], CIcons::appTextMessages16(), "Show text messages", CMenuAction::pathClientCom(), { this, &CAtcStationView::requestTextMessage });
        }
        CViewBase::customMenu(menuActions);
    }

    void CAtcStationView::tuneInAtcCom1()
    {
        const CAtcStation s(this->selectedObject());
        if (s.getCallsign().isEmpty()) { return; }
        emit this->requestComFrequency(s.getFrequency(), CComSystem::Com1);
    }

    void CAtcStationView::tuneInAtcCom2()
    {
        const CAtcStation s(this->selectedObject());
        if (s.getCallsign().isEmpty()) { return; }
        emit this->requestComFrequency(s.getFrequency(), CComSystem::Com2);
    }

    void CAtcStationView::requestTextMessage()
    {
        const CAtcStation s(this->selectedObject());
        if (s.getCallsign().isEmpty()) { return; }
        emit this->requestTextMessageWidget(s.getCallsign());
    }
} // namespace
