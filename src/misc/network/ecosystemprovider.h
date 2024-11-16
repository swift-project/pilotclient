// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_ECOSYSTEMPROVIDER_H
#define SWIFT_MISC_NETWORK_ECOSYSTEMPROVIDER_H

#include <QReadWriteLock>

#include "misc/network/ecosystem.h"
#include "misc/provider.h"

namespace swift::misc::network
{
    //! Direct threadsafe in memory access to current ecosystem
    class SWIFT_MISC_EXPORT IEcosystemProvider : public IProvider
    {
    public:
        //! Current ecosystem
        //! \remark CEcosystem::Unspecified if not connected with any network
        //! \threadsafe
        CEcosystem getCurrentEcosystem() const;

        //! Last known ecosystem
        //! \threadsafe
        CEcosystem getLastEcosystem() const;

        //! Current ecosystem?
        //! \threadsafe
        bool isCurrentEcosystem(const CEcosystem &system) const;

        //! Current ecosystem VATSIM?
        //! \threadsafe
        bool isCurrentEcosystemVATSIM() const;

        //! Last known ecosystem?
        //! \threadsafe
        bool isLastEcosystem(const CEcosystem &system) const;

    protected:
        //! Set the current system
        //! \threadsafe
        bool setCurrentEcosystem(const CEcosystem &ecosystem);

        //! Set the last known system
        //! \threadsafe
        bool setLastEcosystem(const CEcosystem &ecosystem);

    private:
        CEcosystem m_currentEcoSystem;
        CEcosystem m_lastEcoSystem;
        mutable QReadWriteLock m_lockSystem; //!< lock
    };

    //! Delegating class which can be directly used to access an \sa IEcosystemProvider instance
    class SWIFT_MISC_EXPORT CEcosystemAware : public IProviderAware<IEcosystemProvider>
    {
        virtual void anchor();

    public:
        //! Set the provider
        void setEcosystemProvider(IEcosystemProvider *provider) { this->setProvider(provider); }

        //! \copydoc IEcosystemProvider::getCurrentEcosystem
        CEcosystem getCurrentEcosystem() const;

        //! \copydoc IEcosystemProvider::getLastEcosystem
        CEcosystem getLastEcosystem() const;

        //! \copydoc IEcosystemProvider::isCurrentEcosystem
        bool isCurrentEcosystem(const CEcosystem &system) const;

        //! \copydoc IEcosystemProvider::isCurrentEcosystemVATSIM
        bool isCurrentEcosystemVATSIM() const;

        //! Connected with other system than VATSIM?
        //! \remark use this function to skip VATSIM specific provider tasks etc.
        bool isNotVATSIMEcosystem() const;

        //! \copydoc IEcosystemProvider::isLastEcosystem
        bool isLastEcosystem(const CEcosystem &system) const;

        //! Cast as provider if possible
        static IEcosystemProvider *providerIfPossible(QObject *object);

    protected:
        //! Constructor
        CEcosystemAware(IEcosystemProvider *EcosystemProvider) : IProviderAware(EcosystemProvider) { Q_ASSERT(EcosystemProvider); }
    };
} // namespace swift::misc::network

Q_DECLARE_INTERFACE(swift::misc::network::IEcosystemProvider, "org.swift-project.misc::network::iecosystemprovider")

#endif // guard
