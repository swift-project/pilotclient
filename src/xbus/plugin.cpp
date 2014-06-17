/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "plugin.h"

#define XBUS_SERVICE_SERVICENAME "net.vatsim.xbus"

namespace XBus
{

    CPlugin::CPlugin()
    {
        m_menu = CMenu::mainMenu().subMenu("XBus");
        m_startServerMenuItems.push_back(m_menu.item("Start server on session bus", [this]{ startServer(BlackCore::CDBusServer::sessionDBusServer()); }));
        m_startServerMenuItems.push_back(m_menu.item("Start server on system bus", [this]{ startServer(BlackCore::CDBusServer::systemDBusServer()); }));
        m_startServerMenuItems.push_back(m_menu.item("Start server on localhost P2P", [this]{ startServer(BlackCore::CDBusServer::p2pAddress()); }));
    }

    void CPlugin::startServer(const QString &address)
    {
        Q_ASSERT(! m_server);
        for (auto &item : m_startServerMenuItems) { item.setEnabled(false); }

        m_server = new BlackCore::CDBusServer(XBUS_SERVICE_SERVICENAME, address, this);
        m_service = new CService(this);
        m_server->addObject(CService::ObjectPath(), m_service);
    }

    void CPlugin::onAircraftModelChanged()
    {
        if (m_service)
        {
            m_service->onAircraftModelChanged();
        }
    }

}
