/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_CLIENTPROVIDER_H
#define BLACKMISC_NETWORK_CLIENTPROVIDER_H

#include "blackmisc/network/clientlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/provider.h"
#include <QReadWriteLock>
#include <QMap>

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftSituation;
    }
    namespace Network
    {
        //! Direct in memory access to client (network client) data
        class BLACKMISC_EXPORT IClientProvider : public IProvider
        {
        public:
            //! Get other clients
            //! \threadsafe
            virtual CClientList getClients() const = 0;

            //! Set other clients
            //! \threadsafe
            virtual void setClients(const CClientList &clients) = 0;

            //! Set other clients
            //! \threadsafe
            virtual void clearClients() = 0;

            //! Returns a list of other clients corresponding to the given callsigns
            //! \threadsafe
            virtual CClientList getClientsForCallsigns(const Aviation::CCallsignSet &callsigns) const = 0;

            //! Other client for the given callsigns
            //! \threadsafe
            virtual CClient getClientOrDefaultForCallsign(const Aviation::CCallsign &callsign) const = 0;

            //! Client info for given callsign?
            //! \threadsafe
            virtual bool hasClientInfo(const Aviation::CCallsign &callsign) const = 0;

            //! Add a new client, if existing nothing will be added
            //! \threadsafe
            virtual bool addNewClient(const CClient &client) = 0;

            //! Set client for its callsign
            //! \threadsafe
            virtual bool setOtherClient(const BlackMisc::Network::CClient &client) = 0;

            //! Update or add a client
            //! \threadsafe
            virtual int updateOrAddClient(const Aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues = true) = 0;

            //! Remove client
            //! \threadsafe
            virtual int removeClient(const Aviation::CCallsign &callsign) = 0;

            //! Adjust gnd.flag capability from situation
            //! \threadsafe
            virtual bool autoAdjustCientGndCapability(const Aviation::CAircraftSituation &situation) = 0;

            //! Add gnd.flag capability
            //! \threadsafe
            virtual bool addClientGndCapability(const Aviation::CCallsign &callsign) = 0;

            //! Set gnd.flag capability
            //! \threadsafe
            virtual bool setClientGndCapability(const Aviation::CCallsign &callsign, bool supportGndFlag) = 0;

            //! Mark as other swift client
            //! \threadsafe
            virtual void markAsSwiftClient(const Aviation::CCallsign &callsign) = 0;
        };

        //! Direct in memory access to client (network client) data
        class BLACKMISC_EXPORT CClientProvider : public IClientProvider
        {
        public:
            //! clientprovider
            virtual CClientList getClients() const override;
            virtual void setClients(const CClientList &clients) override;
            virtual void clearClients() override;
            virtual CClientList getClientsForCallsigns(const Aviation::CCallsignSet &callsigns) const override;
            virtual CClient getClientOrDefaultForCallsign(const Aviation::CCallsign &callsign) const override;
            virtual bool hasClientInfo(const Aviation::CCallsign &callsign) const override;
            virtual bool addNewClient(const CClient &client) override;
            virtual bool setOtherClient(const BlackMisc::Network::CClient &client) override;
            virtual int updateOrAddClient(const Aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues = true) override;
            virtual int removeClient(const Aviation::CCallsign &callsign) override;
            virtual bool autoAdjustCientGndCapability(const Aviation::CAircraftSituation &situation) override;
            virtual bool addClientGndCapability(const Aviation::CCallsign &callsign) override;
            virtual bool setClientGndCapability(const Aviation::CCallsign &callsign, bool supportGndFlag) override;
            virtual void markAsSwiftClient(const Aviation::CCallsign &callsign) override;

        private:
            CClientPerCallsign m_clients;
            mutable QReadWriteLock m_lockClient; //!< lock clients: m_clients
        };

        //! Class which can be directly used to access an \sa IClientProvider object
        class BLACKMISC_EXPORT CClientAware : public IProviderAware<IClientProvider>
        {
            virtual void anchor();

        public:
            //! \copydoc CClientProvider::getClients
            CClientList getClients() const;

            //! \copydoc CClientProvider::setClients
            void setClients(const CClientList &clients);

            //! \copydoc CClientProvider::clearClients
            void clearClients();

            //! \copydoc CClientProvider::getClientsForCallsigns
            BlackMisc::Network::CClientList getClientsForCallsigns(const Aviation::CCallsignSet &callsigns) const;

            //! \copydoc CClientProvider::getClientOrDefaultForCallsign
            BlackMisc::Network::CClient getClientOrDefaultForCallsign(const Aviation::CCallsign &callsign) const;

            //! \copydoc CClientProvider::hasClientInfo
            bool hasClientInfo(const Aviation::CCallsign &callsign) const;

            //! \copydoc CClientProvider::addNewClient
            bool addNewClient(const CClient &client);

            //! \copydoc CClientProvider::updateOrAddClient
            int updateOrAddClient(const Aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues);

            //! \copydoc CClientProvider::removeClient
            int removeClient(const Aviation::CCallsign &callsign);

            //! \copydoc CClientProvider::autoAdjustCientGndCapability
            bool autoAdjustCientGndCapability(const Aviation::CAircraftSituation &situation);

            //! \copydoc CClientProvider::addClientGndCapability
            bool addClientGndCapability(const Aviation::CCallsign &callsign);

            //! \copydoc CClientProvider::markAsSwiftClient
            void markAsSwiftClient(const Aviation::CCallsign &callsign);

            //! Provider
            void setClientProvider(CClientProvider *provider) { this->setProvider(provider); }

        protected:
            //! Default constructor
            CClientAware() {}

            //! Constructor
            CClientAware(IClientProvider *clientProvider) : IProviderAware(clientProvider) {}
        };

        //! Client provider dummy for testing
        class BLACKMISC_EXPORT CClientProviderDummy : public CClientProvider
        {
        public:
            //! Dummy instance
            static CClientProviderDummy *instance();
        };
    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Network::IClientProvider, "org.swift-project.blackmisc::network::iclientprovider")

#endif // guard
