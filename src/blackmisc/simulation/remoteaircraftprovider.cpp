/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "remoteaircraftprovider.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {

        const CSimulatedAircraftList &CRemoteAircraftAwareReadOnly::remoteAircraft() const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraft();
        }

        Aviation::CAircraftSituationList CRemoteAircraftAwareReadOnly::remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftSituations(callsign);
        }

        int CRemoteAircraftAwareReadOnly::remoteAircraftSituationsCount(const CCallsign &callsign) const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftSituationsCount(callsign);
        }

        CAircraftPartsList CRemoteAircraftAwareReadOnly::remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeBefore) const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftParts(callsign, cutoffTimeBefore);
        }

        CCallsignSet CRemoteAircraftAwareReadOnly::remoteAircraftSupportingParts() const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftSupportingParts();
        }

        bool CRemoteAircraftAwareReadOnly::isRemoteAircraftSupportingParts(const CCallsign &callsign) const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->isRemoteAircraftSupportingParts(callsign);
        }

        const CSimulatedAircraftList &CRemoteAircraftAware::remoteAircraft() const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraft();
        }

        CSimulatedAircraftList &CRemoteAircraftAware::remoteAircraft()
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraft();
        }

        CAircraftSituationList CRemoteAircraftAware::remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftSituations(callsign);
        }

        CAircraftPartsList CRemoteAircraftAware::remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign) const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftParts(callsign);
        }

        CCallsignSet CRemoteAircraftAware::remoteAircraftSupportingParts() const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftSupportingParts();
        }

        int CRemoteAircraftAware::remoteAircraftSituationsCount(const CCallsign &callsign) const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftSituationsCount(callsign);
        }

        bool CRemoteAircraftAware::providerUpdateAircraftModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const QString &originator)
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->updateAircraftModel(callsign, model, originator);
        }

        bool CRemoteAircraftAware::isRemoteAircraftSupportingParts(const CCallsign &callsign) const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->isRemoteAircraftSupportingParts(callsign);
        }

        bool CRemoteAircraftAware::providerUpdateAircraftEnabled(const Aviation::CCallsign &callsign, bool enabledForRedering, const QString &originator)
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, Q_FUNC_INFO, "No object available");
            return this->m_remoteAircraftProvider->updateAircraftEnabled(callsign, enabledForRedering, originator);
        }

    } // namespace
} // namespace
