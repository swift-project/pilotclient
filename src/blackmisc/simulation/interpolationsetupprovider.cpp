/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationsetupprovider.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {
        CInterpolationAndRenderingSetupPerCallsign IInterpolationSetupProvider::getInterpolationSetupPerCallsignOrDefault(const CCallsign &callsign) const
        {
            QReadLocker l(&m_lockSetup);
            return m_setups.contains(callsign) ? m_setups.value(callsign) : CInterpolationAndRenderingSetupPerCallsign(callsign, m_globalSetup);
        }

        CInterpolationSetupList IInterpolationSetupProvider::getInterpolationSetupsPerCallsign() const
        {
            const SetupsPerCallsign setups = this->getSetupsPerCallsign();
            return CInterpolationSetupList(setups.values());
        }

        void IInterpolationSetupProvider::setInterpolationSetupsPerCallsign(const CInterpolationSetupList &setups)
        {
            SetupsPerCallsign setupsPerCs;
            for (const CInterpolationAndRenderingSetupPerCallsign &setup : setups)
            {
                setupsPerCs.insert(setup.getCallsign(), setup);
            }

            QWriteLocker l(&m_lockSetup);
            m_setups = setupsPerCs;
        }

        CInterpolationAndRenderingSetupGlobal IInterpolationSetupProvider::getInterpolationSetupGlobal() const
        {
            QReadLocker l(&m_lockSetup);
            return m_globalSetup;
        }

        CCallsignSet IInterpolationSetupProvider::getLogCallsigns() const
        {
            const SetupsPerCallsign setups = this->getSetupsPerCallsign();
            CCallsignSet callsigns;
            for (const CCallsign &cs : setups.keys())
            {
                if (setups.value(cs).logInterpolation()) { callsigns.insert(cs); }
            }
            return callsigns;
        }

        bool IInterpolationSetupProvider::setInterpolationSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup)
        {
            {
                QReadLocker l(&m_lockSetup);
                if (m_globalSetup == setup) { return false; }
            }

            QWriteLocker l(&m_lockSetup);
            m_globalSetup = setup;
            return true;
        }

        bool IInterpolationSetupProvider::setInterpolationSetupPerCallsign(const CInterpolationAndRenderingSetupPerCallsign &setup, const CCallsign &callsign, bool removeGlobalSetup)
        {
            if (removeGlobalSetup)
            {
                if (setup.isEqualToGlobal(this->getInterpolationSetupGlobal()))
                {
                    QWriteLocker l(&m_lockSetup);
                    m_setups.remove(callsign);
                    return false;
                }
            }
            QWriteLocker l(&m_lockSetup);
            m_setups[callsign] = setup;
            return true;
        }

        void IInterpolationSetupProvider::setLogCallsign(bool log, const CCallsign &callsign)
        {
            CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(callsign);
            if (setup.logInterpolation() == log) { return; }
            setup.setLogInterpolation(log);
            this->setInterpolationSetupPerCallsign(setup, callsign);
        }

        void IInterpolationSetupProvider::clearInterpolationLogCallsigns()
        {
            SetupsPerCallsign setupsCopy = this->getSetupsPerCallsign();
            if (setupsCopy.isEmpty()) { return; }

            // potential risk, changes inbetween in another thread are missed now
            // on the other side, we keep locks for a minimal time frame
            SetupsPerCallsign setupsToKeep;
            CInterpolationAndRenderingSetupGlobal global = this->getInterpolationSetupGlobal();
            for (const CCallsign &cs : setupsCopy.keys())
            {
                CInterpolationAndRenderingSetupPerCallsign setup = setupsCopy.value(cs);
                setup.setLogInterpolation(false);
                if (setup.isEqualToGlobal(global)) { continue; }
                setupsToKeep.insert(cs, setup);
            }

            QWriteLocker l(&m_lockSetup);
            m_setups = setupsToKeep;
        }

        bool IInterpolationSetupProvider::logAnyCallsign() const
        {
            const SetupsPerCallsign setupsCopy = this->getSetupsPerCallsign();
            if (setupsCopy.isEmpty()) { return false; }
            for (const CCallsign &cs : setupsCopy.keys())
            {
                if (setupsCopy.value(cs).logInterpolation()) { return true; }
            }
            return false;
        }

        IInterpolationSetupProvider::SetupsPerCallsign IInterpolationSetupProvider::getSetupsPerCallsign() const
        {
            QReadLocker l(&m_lockSetup);
            return m_setups;
        }

        CInterpolationAndRenderingSetupPerCallsign CInterpolationSetupAware::getInterpolationSetupPerCallsignOrDefault(const CCallsign &callsign) const
        {
            if (!this->hasProvider()) { return CInterpolationAndRenderingSetupPerCallsign(); }
            return this->provider()->getInterpolationSetupPerCallsignOrDefault(callsign);
        }

        CInterpolationAndRenderingSetupGlobal CInterpolationSetupAware::getInterpolationSetupGlobal() const
        {
            if (!this->hasProvider()) { return CInterpolationAndRenderingSetupGlobal(); }
            return this->provider()->getInterpolationSetupGlobal();
        }
    } // namespace
} // namespace
