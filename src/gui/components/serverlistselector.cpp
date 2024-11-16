// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/serverlistselector.h"

#include <QString>

#include "core/db/icaodatareader.h"
#include "core/webdataservices.h"
#include "gui/guiapplication.h"
#include "misc/sequence.h"

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::gui;
using namespace swift::core;
using namespace swift::core::db;

namespace swift::gui::components
{
    CServerListSelector::CServerListSelector(QWidget *parent) : QComboBox(parent)
    {
        const CServer server = m_lastServer.get();
        if (server.hasName())
        {
            m_pendingPreselect = server.getName();
        }
        connect(this, &QComboBox::currentTextChanged, this, &CServerListSelector::onServerTextChanged);
    }

    void CServerListSelector::setServers(const CServerList &servers)
    {
        if (m_servers == servers) { return; }
        this->setServerItems(servers);
        if (!servers.isEmpty() && !m_pendingPreselect.isEmpty())
        {
            this->preSelect(m_pendingPreselect);
            m_pendingPreselect.clear();
        }
    }

    CServer CServerListSelector::currentServer() const
    {
        const int i = currentIndex();
        if (i < 0 || i >= m_servers.size()) { return CServer(); }
        return m_servers[i];
    }

    bool CServerListSelector::preSelect(const QString &name)
    {
        if (name.isEmpty()) { return false; }
        if (m_servers.isEmpty())
        {
            m_pendingPreselect = name; // save for later
            return false;
        }
        for (int i = 0; i < m_servers.size(); i++)
        {
            if (m_servers[i].matchesName(name))
            {
                this->setCurrentIndex(i);
                return true;
            }
        }
        return false;
    }

    void CServerListSelector::setServerItems(const CServerList &servers)
    {
        QString currentlySelected(this->currentText());
        int index = -1;
        m_servers = servers;
        m_items.clear();
        this->clear(); // ui

        for (const CServer &server : servers)
        {
            const QString d(server.getName() + ": " + server.getDescription());
            m_items.append(d);
            if (!currentlySelected.isEmpty() && index < 0 && d == currentlySelected)
            {
                index = m_items.size() - 1;
            }

            this->addItem(d);
        }

        // reselect
        if (m_items.isEmpty()) { return; }
        if (m_items.size() == 1)
        {
            this->setCurrentIndex(0);
        }
        else if (index >= 0)
        {
            this->setCurrentIndex(index);
        }
    }

    void CServerListSelector::onServerTextChanged(const QString &text)
    {
        Q_UNUSED(text);
        emit this->serverChanged(this->currentServer());
    }

} // namespace swift::gui::components
