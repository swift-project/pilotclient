/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/serverlistselector.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/sequence.h"
#include "blackcore/webdataservices.h"
#include "blackcore/db/icaodatareader.h"

#include <QString>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackGui;
using namespace BlackCore;
using namespace BlackCore::Db;

namespace BlackGui
{
    namespace Components
    {
        CServerListSelector::CServerListSelector(QWidget *parent) :
            QComboBox(parent)
        {
            const CServer server = m_lastServer.get();
            if (server.hasName())
            {
                m_pendingPreselect = server.getName();
            }
            connect(this, &QComboBox::currentTextChanged, this, &CServerListSelector::onServerTextChanged);
        }

        void CServerListSelector::setServers(const CServerList &servers, bool nameIsCountry)
        {
            if (m_servers == servers) { return; }
            this->setServerItems(servers, nameIsCountry);
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

        void CServerListSelector::setServerItems(const CServerList &servers, bool nameToCountry)
        {
            QString currentlySelected(this->currentText());
            int index = -1;
            m_servers = servers;
            m_items.clear();
            this->clear(); // ui

            nameToCountry = nameToCountry && knowsAllCountries();
            for (const CServer &server : servers)
            {
                const QString d(server.getName() + ": " + server.getDescription());
                m_items.append(d);
                if (!currentlySelected.isEmpty() && index < 0 && d == currentlySelected)
                {
                    index = m_items.size() - 1;
                }

                if (nameToCountry)
                {
                    const CCountry country(this->findCountry(server));
                    if (country.getName().isEmpty())
                    {
                        this->addItem(CIcons::empty16(), d);
                    }
                    else
                    {
                        this->addItem(CIcon(country.toIcon()).toPixmap(), d);
                    }
                }
                else
                {
                    this->addItem(d);
                }
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

        bool CServerListSelector::knowsAllCountries()
        {
            return (sGui && sGui->getWebDataServices() && sGui->getWebDataServices()->getCountriesCount() > 0);
        }

        CCountry CServerListSelector::findCountry(const CServer &server)
        {
            if (!CServerListSelector::knowsAllCountries()) { return CCountry(); }
            static const CCountryList countries(sGui->getWebDataServices()->getCountries());
            const CCountry ctryByName = countries.findBestMatchByCountryName(server.getName());
            if (ctryByName.isValid()) { return ctryByName; }

            // own approach, see if description contains a valid countr name
            for (const CCountry &testCtry : countries)
            {
                if (testCtry.getName().isEmpty()) { continue; }
                if (server.getDescription().contains(testCtry.getName(), Qt::CaseInsensitive))
                {
                    return testCtry;
                }
            }

            const CCountry ctryByDescription = countries.findBestMatchByCountryName(server.getDescription());
            return ctryByDescription;
        }
    } // ns
} // ns
