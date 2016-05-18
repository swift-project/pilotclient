/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/serverlistselector.h"
#include "blackmisc/sequence.h"

#include <QString>

using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CServerListSelector::CServerListSelector(QWidget *parent) :
            QComboBox(parent)
        {
        }

        void CServerListSelector::setServers(const BlackMisc::Network::CServerList &servers)
        {
            if (this->m_servers == servers) { return; }
            this->setItemStrings(servers);
        }

        BlackMisc::Network::CServer CServerListSelector::currentServer() const
        {
            int i = currentIndex();
            if (i < 0 || i >= m_servers.size()) { return CServer(); }
            return m_servers[i];
        }

        void CServerListSelector::setItemStrings(const CServerList &servers)
        {
            QString currentlySelected(this->currentText());
            int index = -1;
            this->m_servers = servers;
            this->m_items.clear();
            for (const CServer &server : servers)
            {
                QString d(server.getName() + ": " + server.getDescription());
                m_items.append(d);
                if (!currentlySelected.isEmpty() && index < 0 && d == currentlySelected)
                {
                    index = m_items.size() - 1;
                }
            }
            this->clear(); // ui
            this->addItems(m_items);

            // reselect
            if (this->m_items.isEmpty()) { return; }
            if (this->m_items.size() == 1)
            {
                this->setCurrentIndex(0);
            }
            else if (index >= 0)
            {
                this->setCurrentIndex(index);
            }
        }
    } // ns
} // ns
