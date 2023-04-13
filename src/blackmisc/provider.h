/* Copyright (C) 2018
 * Swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PROVIDER_H
#define BLACKMISC_PROVIDER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/connectionguard.h"
#include <QObject>
#include <QMetaObject>

namespace BlackMisc
{
    //! Base class for providers
    class BLACKMISC_EXPORT IProvider
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
    class BLACKMISC_EXPORT IProviderAware
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
                QMetaObject::Connection con = QObject::connect(iProvider->asQObject(), &QObject::destroyed, [=](QObject *obj) { this->onProviderDestroyed(obj); });
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
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::IProvider, "org.swift-project.blackmisc.iprovider")

#endif // guard
