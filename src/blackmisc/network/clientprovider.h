/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_CLIENTPROVIDER_H
#define BLACKMISC_NETWORK_CLIENTPROVIDER_H

#include "clientlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/provider.h"
#include <QReadWriteLock>

namespace BlackMisc
{
    namespace Network
    {
        //! Direct in memory access to client (network client) data
        class BLACKMISC_EXPORT IClientProvider : public IProvider
        {
        public:
            //! Get other clients
            //! \threadsafe
            CClientList getClients() const;

            //! Set other clients
            //! \threadsafe
            void setClients(const CClientList &clients);

            //! Set other clients
            //! \threadsafe
            void clearClients();

            //! Returns a list of other clients corresponding to the given callsigns
            //! \threadsafe
            BlackMisc::Network::CClientList getClientsForCallsigns(const Aviation::CCallsignSet &callsigns) const;

            //! Other client for the given callsigns
            //! \threadsafe
            BlackMisc::Network::CClient getClientOrDefaultForCallsign(const Aviation::CCallsign &callsign) const;

            //! Client info for given callsign?
            //! \threadsafe
            bool hasClientInfo(const Aviation::CCallsign &callsign) const;

            //! Add a new client, if existing nothing will be added
            //! \threadsafe
            bool addNewClient(const CClient &client);

            //! Update or add a client
            //! \threadsafe
            int updateOrAddClient(const Aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues = true);

            //! Remove client
            //! \threadsafe
            int removeClient(const Aviation::CCallsign &callsign);

        private:
            CClientList m_clients;
            mutable QReadWriteLock m_lockClient; //!< lock clients: m_clients
        };

        //! Class which can be directly used to access an \sa IClientProvider object
        class BLACKMISC_EXPORT CClientAware : public IProviderAware<IClientProvider>
        {
        public:
            //! \copydoc IClientProvider::getClients
            CClientList getClients() const;

            //! \copydoc IClientProvider::setClients
            void setClients(const CClientList &clients);

            //! \copydoc IClientProvider::clearClients
            void clearClients();

            //! \copydoc IClientProvider::getClientsForCallsigns
            BlackMisc::Network::CClientList getClientsForCallsigns(const Aviation::CCallsignSet &callsigns) const;

            //! \copydoc IClientProvider::getClientOrDefaultForCallsign
            BlackMisc::Network::CClient getClientOrDefaultForCallsign(const Aviation::CCallsign &callsign) const;

            //! \copydoc IClientProvider::hasClientInfo
            bool hasClientInfo(const Aviation::CCallsign &callsign) const;

            //! \copydoc IClientProvider::addNewClient
            bool addNewClient(const CClient &client);

            //! \copydoc IClientProvider::updateOrAddClient
            int updateOrAddClient(const Aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues);

            //! \copydoc IClientProvider::removeClient
            int removeClient(const Aviation::CCallsign &callsign);

            //! Provider
            void setClientProvider(IClientProvider *provider) { this->setProvider(provider); }

        protected:
            //! Default constructor
            CClientAware() {}

            //! Constructor
            CClientAware(IClientProvider *clientProvider) : m_clientProvider(clientProvider) { }
            IClientProvider *m_clientProvider = nullptr; //!< access to object
        };

        //! Client provider dummy for testing
        class BLACKMISC_EXPORT CClientProviderDummy: public IClientProvider
        {
        public:
            //! Dummy instance
            static CClientProviderDummy *instance();
        };
    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Network::IClientProvider, "BlackMisc::Network::IClientProvider")

#endif // guard
