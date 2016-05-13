/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/corefacadeconfig.h"

namespace BlackCore
{
    bool CCoreFacadeConfig::requiresDBusSever() const
    {
        return (
                   // those 3 should decide whether we are running the server
                   this->m_network == LocalInDbusServer ||
                   this->m_ownAircraft == LocalInDbusServer ||
                   this->m_simulator == LocalInDbusServer ||

                   // added as work around
                   this->m_audio == LocalInDbusServer
               );
    }

    bool CCoreFacadeConfig::requiresDBusConnection() const
    {
        return (this->m_application == Remote ||
                this->m_audio == Remote ||
                this->m_network == Remote ||
                this->m_ownAircraft == Remote ||
                this->m_simulator == Remote);
    }

    CCoreFacadeConfig CCoreFacadeConfig::forCoreAllLocalInDBus(const QString &dbusBootstrapAddress)
    {
        const CCoreFacadeConfig cfg = CCoreFacadeConfig(CCoreFacadeConfig(CCoreFacadeConfig::LocalInDbusServer, dbusBootstrapAddress));
        return cfg;
    }

    CCoreFacadeConfig CCoreFacadeConfig::forCoreAllLocalInDBusNoAudio(const QString &dbusBootstrapAddress)
    {
        CCoreFacadeConfig cfg = CCoreFacadeConfig(CCoreFacadeConfig(CCoreFacadeConfig::LocalInDbusServer, dbusBootstrapAddress));
        cfg.m_audio = CCoreFacadeConfig::NotUsed;
        return cfg;
    }

    CCoreFacadeConfig CCoreFacadeConfig::local(const QString &dbusBootstrapAddress)
    {
        const CCoreFacadeConfig cfg = CCoreFacadeConfig(CCoreFacadeConfig(CCoreFacadeConfig::Local, dbusBootstrapAddress));
        return cfg;
    }

    CCoreFacadeConfig CCoreFacadeConfig::remote(const QString &dbusBootstrapAddress)
    {
        const CCoreFacadeConfig cfg = CCoreFacadeConfig(CCoreFacadeConfig(CCoreFacadeConfig::Remote, dbusBootstrapAddress));
        return cfg;
    }

    CCoreFacadeConfig CCoreFacadeConfig::remoteLocalAudio(const QString &dbusBootstrapAddress)
    {
        CCoreFacadeConfig cfg = CCoreFacadeConfig(CCoreFacadeConfig(CCoreFacadeConfig::Remote, dbusBootstrapAddress));
        cfg.m_audio = CCoreFacadeConfig::LocalInDbusServer;
        return cfg;
    }

    CCoreFacadeConfig CCoreFacadeConfig::allEmpty()
    {
        const CCoreFacadeConfig cfg = CCoreFacadeConfig();
        return cfg;
    }
} // namespace
