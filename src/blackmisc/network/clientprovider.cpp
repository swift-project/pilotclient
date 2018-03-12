/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "clientprovider.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Network
    {
        CClientList IClientProvider::getClients() const
        {
            QReadLocker l(&m_lockClient);
            return m_clients;
        }

        void IClientProvider::setClients(const CClientList &clients)
        {
            QWriteLocker l(&m_lockClient);
            m_clients = clients;
        }

        void IClientProvider::clearClients()
        {
            QWriteLocker l(&m_lockClient);
            m_clients.clear();
        }

        CClientList IClientProvider::getClientsForCallsigns(const CCallsignSet &callsigns) const
        {
            return this->getClients().findByCallsigns(callsigns);
        }

        CClient IClientProvider::getClientOrDefaultForCallsign(const CCallsign &callsign) const
        {
            const CClientList clients(this->getClients());
            return clients.findFirstByCallsign(callsign);
        }

        bool IClientProvider::hasClientInfo(const CCallsign &callsign) const
        {
            return this->getClients().containsCallsign(callsign);
        }

        bool IClientProvider::addNewClient(const CClient &client)
        {
            const CCallsign callsign = client.getCallsign();
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "invalid callsign");
            if (this->hasClientInfo(callsign)) { return false; }
            QWriteLocker l(&m_lockClient);
            m_clients.push_back(client);
            return true;
        }

        int IClientProvider::updateOrAddClient(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues)
        {
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");
            int c = 0;
            if (!this->hasClientInfo(callsign))
            {
                CClient client(callsign);
                c = client.apply(vm).size();
                this->addNewClient(client);
            }
            else
            {
                QWriteLocker l(&m_lockClient);
                c = m_clients.applyIfCallsign(callsign, vm, skipEqualValues);
            }
            return c;
        }

        int IClientProvider::removeClient(const CCallsign &callsign)
        {
            QWriteLocker l(&m_lockClient);
            return m_clients.removeByCallsign(callsign);
        }

        CClientList CClientAware::getClients() const
        {
            if (this->provider()) { return this->provider()->getClients(); }
            return CClientList();
        }

        CClient CClientAware::getClientOrDefaultForCallsign(const Aviation::CCallsign &callsign) const
        {
            if (this->provider()) { return this->provider()->getClientOrDefaultForCallsign(callsign); }
            return CClient();
        }

        bool CClientAware::hasClientInfo(const CCallsign &callsign) const
        {
            if (this->provider()) { return this->provider()->hasClientInfo(callsign); }
            return false;
        }

        bool CClientAware::addNewClient(const CClient &client)
        {
            if (this->provider()) { return this->provider()->addNewClient(client); }
            return false;
        }

        int CClientAware::updateOrAddClient(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues)
        {
            if (this->provider()) { return this->provider()->updateOrAddClient(callsign, vm, skipEqualValues); }
            return 0;
        }

        int CClientAware::removeClient(const CCallsign &callsign)
        {
            if (this->provider()) { return this->provider()->removeClient(callsign); }
            return 0;
        }

        CClientProviderDummy *CClientProviderDummy::instance()
        {
            static CClientProviderDummy *dummy = new CClientProviderDummy();
            return dummy;
        }
    }
    // namespace
} // namespace
