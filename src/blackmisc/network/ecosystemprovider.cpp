/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ecosystemprovider.h"
#include <QReadLocker>

namespace BlackMisc
{
    namespace Network
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

        bool CEcosystemAware::isLastEcosystem(const CEcosystem &system) const
        {
            if (!this->hasProvider()) { return this->getLastEcosystem() == system; }
            return this->provider()->isLastEcosystem(system);
        }
    } // namespace
} // namespace
