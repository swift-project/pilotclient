/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
            if (!m_setupsPerCallsign.contains(callsign)) { return CInterpolationAndRenderingSetupPerCallsign(callsign, m_globalSetup); }
            return m_setupsPerCallsign.value(callsign);
        }

        CInterpolationSetupList IInterpolationSetupProvider::getInterpolationSetupsPerCallsign() const
        {
            const SetupsPerCallsign setups = this->getSetupsPerCallsign();
            return CInterpolationSetupList(setups.values());
        }

        bool IInterpolationSetupProvider::hasSetupsPerCallsign() const
        {
            QReadLocker l(&m_lockSetup);
            return !m_setupsPerCallsign.isEmpty();
        }

        bool IInterpolationSetupProvider::setInterpolationSetupsPerCallsign(const CInterpolationSetupList &setups, bool ignoreSameAsGlobal)
        {
            const CInterpolationAndRenderingSetupGlobal gs = this->getInterpolationSetupGlobal();
            SetupsPerCallsign setupsPerCs;
            for (const CInterpolationAndRenderingSetupPerCallsign &setup : setups)
            {
                if (ignoreSameAsGlobal && setup.isEqualToGlobal(gs)) { continue; }
                setupsPerCs.insert(setup.getCallsign(), setup);
            }
            {
                QWriteLocker l(&m_lockSetup);
                if (m_setupsPerCallsign.isEmpty() && setupsPerCs.isEmpty()) { return false; }
                m_setupsPerCallsign = setupsPerCs;
            }
            this->emitInterpolationSetupChanged();
            return true;
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
            for (const auto [callsign, setup] : makePairsRange(setups))
            {
                if (setup.logInterpolation()) { callsigns.insert(callsign); }
            }
            return callsigns;
        }

        bool IInterpolationSetupProvider::isLogCallsign(const CCallsign &callsign) const
        {
            QReadLocker l(&m_lockSetup);
            if (!m_setupsPerCallsign.contains(callsign)) { return false; }
            return m_setupsPerCallsign[callsign].logInterpolation();
        }

        bool IInterpolationSetupProvider::setInterpolationMode(const QString &modeAsString, const CCallsign &callsign)
        {
            CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(callsign);
            if (!setup.setInterpolatorMode(modeAsString)) { return false; }

            // changed value
            return this->setInterpolationSetupPerCallsign(setup, callsign, true);
        }

        bool IInterpolationSetupProvider::setLogInterpolation(bool log, const CCallsign &callsign)
        {
            CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(callsign);
            if (!setup.setLogInterpolation(log)) { return false; }

            // changed value
            return this->setInterpolationSetupPerCallsign(setup, callsign, true);
        }

        bool IInterpolationSetupProvider::setInterpolationSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup)
        {
            {
                QWriteLocker l(&m_lockSetup);
                if (m_globalSetup == setup) { return false; }
                m_globalSetup = setup;
            }
            this->emitInterpolationSetupChanged();
            return true;
        }

        bool IInterpolationSetupProvider::setInterpolationSetupPerCallsign(const CInterpolationAndRenderingSetupPerCallsign &setup, const CCallsign &callsign, bool removeGlobalSetup)
        {
            if (removeGlobalSetup)
            {
                const CInterpolationAndRenderingSetupGlobal gs = this->getInterpolationSetupGlobal();
                if (setup.isEqualToGlobal(gs))
                {
                    QWriteLocker l(&m_lockSetup);
                    m_setupsPerCallsign.remove(callsign);
                    return false;
                }
            }
            {
                QWriteLocker l(&m_lockSetup);
                m_setupsPerCallsign[callsign] = setup;
            }
            this->emitInterpolationSetupChanged();
            return true;
        }

        bool IInterpolationSetupProvider::removeInterpolationSetupPerCallsign(const CCallsign &callsign)
        {
            bool removed = false;
            {
                QWriteLocker l(&m_lockSetup);
                removed = m_setupsPerCallsign.remove(callsign) > 0;
            }
            if (removed) { this->emitInterpolationSetupChanged(); }
            return removed;
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
            for (auto [callsign, setup] : makePairsRange(setupsCopy))
            {
                setup.setLogInterpolation(false);
                if (setup.isEqualToGlobal(global)) { continue; }
                setupsToKeep.insert(callsign, setup);
            }
            {
                QWriteLocker l(&m_lockSetup);
                m_setupsPerCallsign = setupsToKeep;
            }
            this->emitInterpolationSetupChanged();
        }

        int IInterpolationSetupProvider::clearInterpolationSetupsPerCallsign()
        {
            int r = 0;
            {
                QWriteLocker l(&m_lockSetup);
                r = m_setupsPerCallsign.size();
                m_setupsPerCallsign.clear();
            }

            if (r > 0) { this->emitInterpolationSetupChanged(); }
            return r;
        }

        bool IInterpolationSetupProvider::logAnyCallsign() const
        {
            const SetupsPerCallsign setupsCopy = this->getSetupsPerCallsign();
            if (setupsCopy.isEmpty()) { return false; }
            for (const CInterpolationAndRenderingSetupPerCallsign &setup : setupsCopy)
            {
                if (setup.logInterpolation()) { return true; }
            }
            return false;
        }

        IInterpolationSetupProvider::SetupsPerCallsign IInterpolationSetupProvider::getSetupsPerCallsign() const
        {
            QReadLocker l(&m_lockSetup);
            return m_setupsPerCallsign;
        }

        // pin vtables to this file
        void CInterpolationSetupAware::anchor()
        { }

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
