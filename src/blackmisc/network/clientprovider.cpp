/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "clientprovider.h"
#include "blackmisc/aviation/aircraftsituation.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Network
    {
        CClientList CClientProvider::getClients() const
        {
            QReadLocker l(&m_lockClient);
            return m_clients;
        }

        void CClientProvider::setClients(const CClientList &clients)
        {
            QWriteLocker l(&m_lockClient);
            m_clients = clients;
        }

        void CClientProvider::clearClients()
        {
            QWriteLocker l(&m_lockClient);
            m_clients.clear();
        }

        CClientList CClientProvider::getClientsForCallsigns(const CCallsignSet &callsigns) const
        {
            return this->getClients().findByCallsigns(callsigns);
        }

        CClient CClientProvider::getClientOrDefaultForCallsign(const CCallsign &callsign) const
        {
            const CClientList clients(this->getClients());
            return clients.findFirstByCallsign(callsign);
        }

        bool CClientProvider::setOtherClient(const CClient &client)
        {
            QWriteLocker l(&m_lockClient);
            m_clients.replaceOrAddObjectByCallsign(client);
            return true;
        }

        bool CClientProvider::hasClientInfo(const CCallsign &callsign) const
        {
            return this->getClients().containsCallsign(callsign);
        }

        bool CClientProvider::addNewClient(const CClient &client)
        {
            const CCallsign callsign = client.getCallsign();
            Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "invalid callsign");
            if (this->hasClientInfo(callsign)) { return false; }
            QWriteLocker l(&m_lockClient);
            m_clients.push_back(client);
            return true;
        }

        int CClientProvider::updateOrAddClient(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues)
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

        int CClientProvider::removeClient(const CCallsign &callsign)
        {
            QWriteLocker l(&m_lockClient);
            return m_clients.removeByCallsign(callsign);
        }

        bool CClientProvider::autoAdjustCientGndCapability(const CAircraftSituation &situation)
        {
            if (situation.getCallsign().isEmpty()) { return false; } // no callsign
            if (!situation.isOnGround()) { return false; } // nothing to adjust
            if (situation.getOnGroundDetails() != CAircraftSituation::InFromNetwork) { return false; } // not from network
            return this->addClientGndCapability(situation.getCallsign());
        }

        bool CClientProvider::addClientGndCapability(const CCallsign &callsign)
        {
            CClient client = this->getClientOrDefaultForCallsign(callsign);

            // need to change?
            if (!client.isValid()) { return false; } // no client
            if (client.hasGndFlagCapability()) { return true; } // already set, but set

            client.addCapability(CClient::FsdWithGroundFlag);
            QWriteLocker l(&m_lockClient);
            m_clients.replaceOrAddObjectByCallsign(client);
            return true;
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

        bool CClientAware::autoAdjustCientGndCapability(const CAircraftSituation &situation)
        {
            if (this->provider()) { return this->provider()->autoAdjustCientGndCapability(situation); }
            return false;
        }

        bool CClientAware::addClientGndCapability(const CCallsign &callsign)
        {
            if (this->provider()) { return this->provider()->addClientGndCapability(callsign); }
            return false;
        }

        CClientProviderDummy *CClientProviderDummy::instance()
        {
            static CClientProviderDummy *dummy = new CClientProviderDummy();
            return dummy;
        }
    }
    // namespace
} // namespace
