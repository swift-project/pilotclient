// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_CLIENTPROVIDER_H
#define SWIFT_MISC_NETWORK_CLIENTPROVIDER_H

#include <QMap>
#include <QReadWriteLock>

#include "misc/aviation/callsignset.h"
#include "misc/network/clientlist.h"
#include "misc/provider.h"

namespace swift::misc
{
    namespace aviation
    {
        class CAircraftSituation;
    }
    namespace network
    {
        //! Direct in memory access to client (network client) data
        class SWIFT_MISC_EXPORT IClientProvider : public IProvider
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
            virtual CClientList getClientsForCallsigns(const aviation::CCallsignSet &callsigns) const = 0;

            //! Other client for the given callsigns
            //! \threadsafe
            virtual CClient getClientOrDefaultForCallsign(const aviation::CCallsign &callsign) const = 0;

            //! Client info for given callsign?
            //! \threadsafe
            virtual bool hasClientInfo(const aviation::CCallsign &callsign) const = 0;

            //! Add a new client, if existing nothing will be added
            //! \threadsafe
            virtual bool addNewClient(const CClient &client) = 0;

            //! Set client for its callsign
            //! \threadsafe
            virtual bool setOtherClient(const swift::misc::network::CClient &client) = 0;

            //! Update or add a client
            //! \threadsafe
            virtual int updateOrAddClient(const aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues = true) = 0;

            //! Remove client
            //! \threadsafe
            virtual int removeClient(const aviation::CCallsign &callsign) = 0;

            //! Adjust gnd.flag capability from situation
            //! \threadsafe
            virtual bool autoAdjustCientGndCapability(const aviation::CAircraftSituation &situation) = 0;

            //! Add gnd.flag capability
            //! \threadsafe
            virtual bool addClientGndCapability(const aviation::CCallsign &callsign) = 0;

            //! Set gnd.flag capability
            //! \threadsafe
            virtual bool setClientGndCapability(const aviation::CCallsign &callsign, bool supportGndFlag) = 0;

            //! Mark as other swift client
            //! \threadsafe
            virtual void markAsSwiftClient(const aviation::CCallsign &callsign) = 0;
        };

        //! Direct in memory access to client (network client) data
        class SWIFT_MISC_EXPORT CClientProvider : public IClientProvider
        {
        public:
            //! clientprovider
            virtual CClientList getClients() const override;
            virtual void setClients(const CClientList &clients) override;
            virtual void clearClients() override;
            virtual CClientList getClientsForCallsigns(const aviation::CCallsignSet &callsigns) const override;
            virtual CClient getClientOrDefaultForCallsign(const aviation::CCallsign &callsign) const override;
            virtual bool hasClientInfo(const aviation::CCallsign &callsign) const override;
            virtual bool addNewClient(const CClient &client) override;
            virtual bool setOtherClient(const swift::misc::network::CClient &client) override;
            virtual int updateOrAddClient(const aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues = true) override;
            virtual int removeClient(const aviation::CCallsign &callsign) override;
            virtual bool autoAdjustCientGndCapability(const aviation::CAircraftSituation &situation) override;
            virtual bool addClientGndCapability(const aviation::CCallsign &callsign) override;
            virtual bool setClientGndCapability(const aviation::CCallsign &callsign, bool supportGndFlag) override;
            virtual void markAsSwiftClient(const aviation::CCallsign &callsign) override;

        private:
            CClientPerCallsign m_clients;
            mutable QReadWriteLock m_lockClient; //!< lock clients: m_clients
        };

        //! Class which can be directly used to access an \sa IClientProvider object
        class SWIFT_MISC_EXPORT CClientAware : public IProviderAware<IClientProvider>
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
            swift::misc::network::CClientList getClientsForCallsigns(const aviation::CCallsignSet &callsigns) const;

            //! \copydoc CClientProvider::getClientOrDefaultForCallsign
            swift::misc::network::CClient getClientOrDefaultForCallsign(const aviation::CCallsign &callsign) const;

            //! \copydoc CClientProvider::hasClientInfo
            bool hasClientInfo(const aviation::CCallsign &callsign) const;

            //! \copydoc CClientProvider::addNewClient
            bool addNewClient(const CClient &client);

            //! \copydoc CClientProvider::updateOrAddClient
            int updateOrAddClient(const aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues);

            //! \copydoc CClientProvider::removeClient
            int removeClient(const aviation::CCallsign &callsign);

            //! \copydoc CClientProvider::autoAdjustCientGndCapability
            bool autoAdjustCientGndCapability(const aviation::CAircraftSituation &situation);

            //! \copydoc CClientProvider::addClientGndCapability
            bool addClientGndCapability(const aviation::CCallsign &callsign);

            //! \copydoc CClientProvider::markAsSwiftClient
            void markAsSwiftClient(const aviation::CCallsign &callsign);

            //! Provider
            void setClientProvider(CClientProvider *provider) { this->setProvider(provider); }

        protected:
            //! Default constructor
            CClientAware() {}

            //! Constructor
            CClientAware(IClientProvider *clientProvider) : IProviderAware(clientProvider) {}
        };

        //! Client provider dummy for testing
        class SWIFT_MISC_EXPORT CClientProviderDummy : public CClientProvider
        {
        public:
            //! Dummy instance
            static CClientProviderDummy *instance();
        };
    } // namespace network
} // namespace swift::misc

Q_DECLARE_INTERFACE(swift::misc::network::IClientProvider, "org.swift-project.misc::network::iclientprovider")

#endif // guard
