// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/corefacadeconfig.h"

namespace swift::core
{
    bool CCoreFacadeConfig::requiresDBusSever() const
    {
        return (
            // those 3 should decide whether we are running the server
            m_network == LocalInDBusServer || m_ownAircraft == LocalInDBusServer || m_simulator == LocalInDBusServer ||

            // added as work around
            m_audio == LocalInDBusServer);
    }

    bool CCoreFacadeConfig::requiresDBusConnection() const { return this->any(Remote); }

    bool CCoreFacadeConfig::any(CCoreFacadeConfig::ContextMode mode) const
    {
        return (m_application == mode || m_audio == mode || m_network == mode || m_ownAircraft == mode ||
                m_simulator == mode);
    }

    bool CCoreFacadeConfig::all(CCoreFacadeConfig::ContextMode mode) const
    {
        return (m_application == mode && m_audio == mode && m_network == mode && m_ownAircraft == mode &&
                m_simulator == mode);
    }

    bool CCoreFacadeConfig::anyRemote() const { return this->any(Remote); }

    bool CCoreFacadeConfig::anyLocalInDBusServer() const { return this->any(LocalInDBusServer); }

    CCoreFacadeConfig CCoreFacadeConfig::forCoreAllLocalInDBus(const QString &dbusBootstrapAddress)
    {
        return { LocalInDBusServer, dbusBootstrapAddress };
    }

    CCoreFacadeConfig CCoreFacadeConfig::local(const QString &dbusBootstrapAddress)
    {
        return { Local, dbusBootstrapAddress };
    }

    CCoreFacadeConfig CCoreFacadeConfig::remote(const QString &dbusBootstrapAddress)
    {
        return { Remote, dbusBootstrapAddress };
    }

    CCoreFacadeConfig CCoreFacadeConfig::allEmpty() { return {}; }
} // namespace swift::core
