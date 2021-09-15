/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ecosystemprovider.h"
#include <QReadLocker>

namespace BlackMisc::Network
{
    CEcosystem IEcosystemProvider::getCurrentEcosystem() const
    {
        QReadLocker l(&m_lockSystem);
        return m_currentEcoSystem;
    }

    CEcosystem IEcosystemProvider::getLastEcosystem() const
    {
        QReadLocker l(&m_lockSystem);
        return m_lastEcoSystem;
    }

    bool IEcosystemProvider::isCurrentEcosystem(const CEcosystem &system) const
    {
        return this->getCurrentEcosystem() == system;
    }

    bool IEcosystemProvider::isCurrentEcosystemVATSIM() const
    {
        return this->isCurrentEcosystem(CEcosystem::vatsim());
    }

    bool IEcosystemProvider::isLastEcosystem(const CEcosystem &system) const
    {
        return this->getLastEcosystem() == system;
    }

    bool IEcosystemProvider::setCurrentEcosystem(const CEcosystem &ecosystem)
    {
        if (this->isCurrentEcosystem(ecosystem)) { return false; }
        QReadLocker l(&m_lockSystem);
        m_currentEcoSystem = ecosystem;
        return true;
    }

    bool IEcosystemProvider::setLastEcosystem(const CEcosystem &ecosystem)
    {
        if (this->isLastEcosystem(ecosystem)) { return false; }
        QReadLocker l(&m_lockSystem);
        m_lastEcoSystem = ecosystem;
        return true;
    }

    // pin vtable to this file
    void CEcosystemAware::anchor()
    { }

    CEcosystem CEcosystemAware::getCurrentEcosystem() const
    {
        if (!this->hasProvider()) { return CEcosystem::unspecified(); }
        return this->provider()->getCurrentEcosystem();
    }

    CEcosystem CEcosystemAware::getLastEcosystem() const
    {
        if (!this->hasProvider()) { return CEcosystem::unspecified(); }
        return this->provider()->getLastEcosystem();
    }

    bool CEcosystemAware::isCurrentEcosystem(const CEcosystem &system) const
    {
        if (!this->hasProvider()) { return this->getCurrentEcosystem() == system; }
        return this->provider()->isCurrentEcosystem(system);
    }

    bool CEcosystemAware::isCurrentEcosystemVATSIM() const
    {
        return this->isCurrentEcosystem(CEcosystem::vatsim());
    }

    bool CEcosystemAware::isNotVATSIMEcosystem() const
    {
        if (!this->hasProvider()) { return false; }
        if (this->isCurrentEcosystemVATSIM()) { return false; }
        if (this->isCurrentEcosystem(CEcosystem::swiftTest())) { return false; } // our test server is supposed to be I VATSIM system
        return !this->getCurrentEcosystem().isUnspecified(); // other know system which is specified
    }

    bool CEcosystemAware::isLastEcosystem(const CEcosystem &system) const
    {
        if (!this->hasProvider()) { return this->getLastEcosystem() == system; }
        return this->provider()->isLastEcosystem(system);
    }

    IEcosystemProvider *CEcosystemAware::providerIfPossible(QObject *object)
    {
        IEcosystemProvider *p = qobject_cast<IEcosystemProvider *>(object);
        return p;
    }
} // namespace
