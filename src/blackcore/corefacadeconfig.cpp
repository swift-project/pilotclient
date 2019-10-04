/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/corefacadeconfig.h"

namespace BlackCore
{
    bool CCoreFacadeConfig::requiresDBusSever() const
    {
        return (
                   // those 3 should decide whether we are running the server
                   this->m_network == LocalInDBusServer ||
                   this->m_ownAircraft == LocalInDBusServer ||
                   this->m_simulator == LocalInDBusServer ||

                   // added as work around
                   this->m_audio == LocalInDBusServer
               );
    }

    bool CCoreFacadeConfig::requiresDBusConnection() const
    {
        return this->any(Remote);
    }

    bool CCoreFacadeConfig::any(CCoreFacadeConfig::ContextMode mode) const
    {
        return (this->m_application == mode ||
                this->m_audio == mode ||
                this->m_network == mode ||
                this->m_ownAircraft == mode ||
                this->m_simulator == mode);
    }

    bool CCoreFacadeConfig::anyRemote() const
    {
        return this->any(Remote);
    }

    bool CCoreFacadeConfig::anyLocalInDBusServer() const
    {
        return this->any(LocalInDBusServer);
    }

    CCoreFacadeConfig CCoreFacadeConfig::forCoreAllLocalInDBus(const QString &dbusBootstrapAddress)
    {
        const CCoreFacadeConfig cfg(CCoreFacadeConfig::LocalInDBusServer, dbusBootstrapAddress);
        return cfg;
    }

    CCoreFacadeConfig CCoreFacadeConfig::local(const QString &dbusBootstrapAddress)
    {
        const CCoreFacadeConfig cfg = CCoreFacadeConfig(CCoreFacadeConfig(CCoreFacadeConfig::Local, dbusBootstrapAddress));
        return cfg;
    }

    CCoreFacadeConfig CCoreFacadeConfig::remote(const QString &dbusBootstrapAddress)
    {
        CCoreFacadeConfig cfg = CCoreFacadeConfig(CCoreFacadeConfig(CCoreFacadeConfig::Remote, dbusBootstrapAddress));
        cfg.m_audio = CCoreFacadeConfig::LocalInDBusServer;
        return cfg;
    }

    CCoreFacadeConfig CCoreFacadeConfig::allEmpty()
    {
        CCoreFacadeConfig cfg = CCoreFacadeConfig();
        cfg.m_audio = CCoreFacadeConfig::Local;
        return cfg;
    }
} // namespace
