/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simdirectaccessremoteaircraft.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {

        const CSimulatedAircraftList &CRemoteAircraftProviderSupportReadOnly::remoteAircraft() const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "renderedAircraft", "No object available");
            return this->m_remoteAircraftProvider->remoteAircraft();
        }

        const Aviation::CAircraftSituationList &CRemoteAircraftProviderSupportReadOnly::remoteAircraftSituations() const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "renderedAircraftSituations", "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftSituations();
        }

        const Aviation::CAircraftPartsList &CRemoteAircraftProviderSupportReadOnly::remoteAircraftParts() const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "renderedAircraftParts", "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftParts();
        }

        const CSimulatedAircraftList &CRemoteAircraftProviderSupport::remoteAircraft() const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "renderedAircraft", "No object available");
            return this->m_remoteAircraftProvider->remoteAircraft();
        }

        CSimulatedAircraftList &CRemoteAircraftProviderSupport::remoteAircraft()
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "renderedAircraft", "No object available");
            return this->m_remoteAircraftProvider->remoteAircraft();
        }

        const CAircraftSituationList &CRemoteAircraftProviderSupport::remoteAircraftSituations() const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "renderedAircraftSituations", "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftSituations();
        }

        CAircraftSituationList &CRemoteAircraftProviderSupport::remoteAircraftSituations()
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "renderedAircraftSituations", "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftSituations();
        }

        const CAircraftPartsList &CRemoteAircraftProviderSupport::remoteAircraftParts() const
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "renderedAircraftParts", "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftParts();
        }

        CAircraftPartsList &CRemoteAircraftProviderSupport::remoteAircraftParts()
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "renderedAircraftParts", "No object available");
            return this->m_remoteAircraftProvider->remoteAircraftParts();
        }

        bool CRemoteAircraftProviderSupport::providerUpdateAircraftModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const QString &originator)
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "providerUpdateAircraftModel", "No object available");
            return this->m_remoteAircraftProvider->updateAircraftModel(callsign, model, originator);
        }

        bool CRemoteAircraftProviderSupport::providerUpdateAircraftEnabled(const Aviation::CCallsign &callsign, bool enabledForRedering, const QString &originator)
        {
            Q_ASSERT_X(this->m_remoteAircraftProvider, "providerUpdateAircraftEnabled", "No object available");
            return this->m_remoteAircraftProvider->updateAircraftEnabled(callsign, enabledForRedering, originator);
        }

    } // namespace
} // namespace
