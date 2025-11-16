// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/views/atcstationview.h"

#include <QFlags>
#include <QtGlobal>

#include "config/buildconfig.h"
#include "gui/menus/menuaction.h"
#include "gui/models/atcstationlistmodel.h"
#include "misc/aviation/callsign.h"
#include "misc/icons.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::gui::models;
using namespace swift::gui::menus;

namespace swift::gui::views
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
        if (this->rowCount() == 1) { this->fullResizeToContents(); }
    }

    void CAtcStationView::customMenu(CMenuActions &menuActions)
    {
        menuActions.addMenuCom();
        if (CBuildConfig::isDebugBuild())
        {
            if (m_debugActions.isEmpty()) { m_debugActions = QList<QAction *>({ nullptr, nullptr }); }
            m_debugActions[0] = menuActions.addAction(m_debugActions[0], CIcons::tableSheet16(),
                                                      "Test: 1k ATC online stations", CMenuAction::pathClientCom(),
                                                      { this, &CAtcStationView::emitTestRequest1kAtcOnlineDummies });
            m_debugActions[1] = menuActions.addAction(m_debugActions[1], CIcons::tableSheet16(),
                                                      "Test: 3k ATC online stations", CMenuAction::pathClientCom(),
                                                      { this, &CAtcStationView::emitTestRequest3kAtcOnlineDummies });
        }

        if (this->hasSelection())
        {
            if (m_actions.isEmpty()) { m_actions = QList<QAction *>({ nullptr, nullptr, nullptr, nullptr, nullptr }); }

            m_actions[0] = menuActions.addAction(m_actions[0], CIcons::appCockpit16(), "Tune in COM1 (active)",
                                                 CMenuAction::pathClientCom(),
                                                 { this, [this]() { tuneInAtc(CComSystem::Com1, true); } });
            m_actions[1] = menuActions.addAction(m_actions[1], CIcons::appCockpit16(), "Tune in COM2 (active)",
                                                 CMenuAction::pathClientCom(),
                                                 { this, [this]() { tuneInAtc(CComSystem::Com2, true); } });
            m_actions[2] = menuActions.addAction(m_actions[2], CIcons::appCockpit16(), "Tune in COM1 (standby)",
                                                 CMenuAction::pathClientCom(),
                                                 { this, [this]() { tuneInAtc(CComSystem::Com1, false); } });
            m_actions[3] = menuActions.addAction(m_actions[3], CIcons::appCockpit16(), "Tune in COM2 (standby)",
                                                 CMenuAction::pathClientCom(),
                                                 { this, [this]() { tuneInAtc(CComSystem::Com2, false); } });
            m_actions[4] =
                menuActions.addAction(m_actions[4], CIcons::appTextMessages16(), "Show text messages",
                                      CMenuAction::pathClientCom(), { this, &CAtcStationView::requestTextMessage });
        }
        CViewBase::customMenu(menuActions);
    }

    void CAtcStationView::tuneInAtc(const misc::aviation::CComSystem::ComUnit unit, const bool active)
    {
        const CAtcStation s(this->selectedObject());
        if (s.getCallsign().isEmpty()) { return; }
        emit this->requestComFrequency(s.getFrequency(), unit, active);
    }

    void CAtcStationView::requestTextMessage()
    {
        const CAtcStation s(this->selectedObject());
        if (s.getCallsign().isEmpty()) { return; }
        emit this->requestTextMessageWidget(s.getCallsign());
    }
} // namespace swift::gui::views
