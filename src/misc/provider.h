// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PROVIDER_H
#define SWIFT_MISC_PROVIDER_H

#include <QMetaObject>
#include <QObject>

#include "misc/connectionguard.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    //! Base class for providers
    class SWIFT_MISC_EXPORT IProvider
    {
    public:
        //! Dtor
        virtual ~IProvider();

        //! Copy constructor
        IProvider(const IProvider &) = delete;

        //! Copy assignment operator
        IProvider &operator=(const IProvider &) = delete;

        //! Return as QObject
        virtual QObject *asQObject() { return nullptr; }

    protected:
        //! Constructor
        IProvider() {}
    };

    //! Base class of provider aware classes
    template <class PROVIDER>
    class SWIFT_MISC_EXPORT IProviderAware
    {
    protected:
        //! Ctor
        IProviderAware(PROVIDER *provider = nullptr) { this->setProvider(provider); }

        //! Dtor
        virtual ~IProviderAware() {}

        //! Has provider?
        bool hasProvider() const { return m_provider; }

        //! Provider
        PROVIDER *getProvider() const { return m_provider; }

        //! Set the provider
        //! \remark use individual names to disambiguate
        void setProvider(PROVIDER *provider)
        {
            if (m_provider == provider) { return; }
            if (m_provider) { m_lastProviderConnections.disconnectAll(); }
            m_provider = provider; // new provider
            IProvider *iProvider = dynamic_cast<IProvider *>(provider);
            if (iProvider && iProvider->asQObject())
            {
                QMetaObject::Connection con =
                    QObject::connect(iProvider->asQObject(), &QObject::destroyed,
                                     [=, this](QObject *obj) { this->onProviderDestroyed(obj); });
                m_lastProviderConnections.append(con);
            }
        }

        //! Provider
        PROVIDER *provider() { return m_provider; }

        //! Const provider
        const PROVIDER *provider() const { return m_provider; }

    private:
        CConnectionGuard m_lastProviderConnections; //!< provider signal connections
        PROVIDER *m_provider = nullptr; //!< access to provider object

        //! Provider was deleted
        virtual void onProviderDestroyed(QObject *obj)
        {
            if (!m_provider) { return; } // already cleaned relationship
            PROVIDER *provider = qobject_cast<PROVIDER *>(obj); // provider is implemented by a QObject
            if (provider != m_provider) { return; } // not our business
            m_provider = nullptr;
        }
    };
} // namespace swift::misc

Q_DECLARE_INTERFACE(swift::misc::IProvider, "org.swift-project.misc.iprovider")

#endif // SWIFT_MISC_PROVIDER_H
