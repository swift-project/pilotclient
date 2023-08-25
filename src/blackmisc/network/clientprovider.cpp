// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/network/clientprovider.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/verify.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc::Network
{
    CClientList CClientProvider::getClients() const
    {
        QList<CClient> clients;
        {
            QReadLocker l(&m_lockClient);
            clients = m_clients.values();
        }
        return CClientList(clients);
    }

    void CClientProvider::setClients(const CClientList &clients)
    {
        const CClientPerCallsign perCallsign(clients.asCallsignHash());
        QWriteLocker l(&m_lockClient);
        m_clients = perCallsign;
    }

    void CClientProvider::clearClients()
    {
        QWriteLocker l(&m_lockClient);
        m_clients.clear();
    }

    CClientList CClientProvider::getClientsForCallsigns(const CCallsignSet &callsigns) const
    {
        if (callsigns.isEmpty()) { return {}; }
        return this->getClients().findByCallsigns(callsigns);
    }

    CClient CClientProvider::getClientOrDefaultForCallsign(const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return {}; }
        {
            QReadLocker l(&m_lockClient);
            if (m_clients.contains(callsign)) { return m_clients.value(callsign); }
        }
        return CClient();
    }

    bool CClientProvider::setOtherClient(const CClient &client)
    {
        const bool hasCallsign = !client.getCallsign().isEmpty();
        BLACK_VERIFY_X(hasCallsign, Q_FUNC_INFO, "Need callsign in client");
        if (!hasCallsign) { return false; }
        QWriteLocker l(&m_lockClient);
        m_clients[client.getCallsign()] = client;
        return true;
    }

    bool CClientProvider::hasClientInfo(const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return false; }
        QReadLocker l(&m_lockClient);
        return m_clients.contains(callsign);
    }

    bool CClientProvider::addNewClient(const CClient &client)
    {
        const CCallsign callsign = client.getCallsign();
        BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing client callsign");
        if (callsign.isEmpty()) { return false; }
        if (this->hasClientInfo(callsign)) { return false; }
        QWriteLocker l(&m_lockClient);
        m_clients[callsign] = client;
        return true;
    }

    int CClientProvider::updateOrAddClient(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues)
    {
        BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing client callsign");
        if (callsign.isEmpty()) { return 0; }
        int c = 0;
        if (this->hasClientInfo(callsign))
        {
            QWriteLocker l(&m_lockClient);
            CClient &client = m_clients[callsign];
            c = client.apply(vm, skipEqualValues).size();
        }
        else
        {
            CClient client(callsign);
            c = client.apply(vm).size();
            QWriteLocker l(&m_lockClient);
            m_clients[callsign] = client;
        }
        return c;
    }

    int CClientProvider::removeClient(const CCallsign &callsign)
    {
        QWriteLocker l(&m_lockClient);
        return m_clients.remove(callsign);
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
        return this->setClientGndCapability(callsign, true);
    }

    bool CClientProvider::setClientGndCapability(const CCallsign &callsign, bool supportGndFlag)
    {
        BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing client callsign");
        if (callsign.isEmpty()) { return 0; }

        CClient client = this->getClientOrDefaultForCallsign(callsign);

        // need to change?
        if (!client.isValid()) { return false; } // no client
        if (client.hasGndFlagCapability() == supportGndFlag) { return true; } // already set, but set
        if (supportGndFlag)
        {
            client.addCapability(CClient::FsdWithGroundFlag);
        }
        else
        {
            client.removeCapability(CClient::FsdWithGroundFlag);
        }
        QWriteLocker l(&m_lockClient);
        m_clients[callsign] = client;
        return true;
    }

    void CClientProvider::markAsSwiftClient(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return; }
        QWriteLocker l(&m_lockClient);
        if (!m_clients.contains(callsign)) { return; }
        m_clients[callsign].setSwiftClient(true);
    }

    // Pin the vtable to this file
    void CClientAware::anchor()
    {}

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

    void CClientAware::markAsSwiftClient(const CCallsign &callsign)
    {
        if (!this->provider()) { return; }
        this->provider()->markAsSwiftClient(callsign);
    }

    CClientProviderDummy *CClientProviderDummy::instance()
    {
        static CClientProviderDummy *dummy = new CClientProviderDummy();
        return dummy;
    }
} // namespace
