// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_COREFACADECONFIG_H
#define SWIFT_CORE_COREFACADECONFIG_H

#include <QString>

#include "core/swiftcoreexport.h"

namespace swift::core
{
    //! Configuration object for the contexts
    class SWIFT_CORE_EXPORT CCoreFacadeConfig
    {
    public:
        //! How to handle a given context
        enum ContextMode
        {
            NotUsed, //!< during shutdown or not used at all
            Local, //!< context runs in same process
            LocalInDBusServer, //!< context runs in same process. It is also registered in DBus to be accessed by proxy
                               //!< objects
            Remote //!< context runs in a different process. Access will happen via proxy object
        };

    private:
        ContextMode m_contextMode; // All contexts run in the same mode
        QString m_dbusAddress; //!< for boot strapping

    public:
        //! Constructor
        explicit CCoreFacadeConfig(ContextMode contextMode, const QString &dbusBootstrapAddress = "")
            : m_contextMode(contextMode), m_dbusAddress(dbusBootstrapAddress)
        {}

        //! Mode
        ContextMode getMode() const { return m_contextMode; }

        //! Requires server (at least one in server)?
        bool requiresDBusSever() const;

        //! Requires DBus connection (at least one remote)?
        bool requiresDBusConnection() const;

        //! DBus address
        QString getDBusAddress() const { return m_dbusAddress; }

        //! DBus address?
        bool hasDBusAddress() const { return !m_dbusAddress.isEmpty(); }
    };
} // namespace swift::core
#endif // SWIFT_CORE_COREFACADECONFIG_H
