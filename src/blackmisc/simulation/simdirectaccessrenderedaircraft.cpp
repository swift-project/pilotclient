/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simdirectaccessrenderedaircraft.h"

namespace BlackMisc
{
    namespace Simulation
    {
        const CSimulatedAircraftList &CRenderedAircraftProviderSupportReadOnly::renderedAircraft() const
        {
            Q_ASSERT_X(this->m_renderedAircraftProvider, "renderedAircraft", "No object available");
            return this->m_renderedAircraftProvider->renderedAircraft();
        }

        const Aviation::CAircraftSituationList &CRenderedAircraftProviderSupportReadOnly::renderedAircraftSituations() const
        {
            Q_ASSERT_X(this->m_renderedAircraftProvider, "renderedAircraftSituations", "No object available");
            return this->m_renderedAircraftProvider->renderedAircraftSituations();
        }

        const CSimulatedAircraftList &CRenderedAircraftProviderSupport::renderedAircraft() const
        {
            Q_ASSERT_X(this->m_renderedAircraftProvider, "renderedAircraft", "No object available");
            return this->m_renderedAircraftProvider->renderedAircraft();
        }

        CSimulatedAircraftList &CRenderedAircraftProviderSupport::renderedAircraft()
        {
            Q_ASSERT_X(this->m_renderedAircraftProvider, "renderedAircraft", "No object available");
            return this->m_renderedAircraftProvider->renderedAircraft();
        }

        const Aviation::CAircraftSituationList &CRenderedAircraftProviderSupport::renderedAircraftSituations() const
        {
            Q_ASSERT_X(this->m_renderedAircraftProvider, "renderedAircraftSituations", "No object available");
            return this->m_renderedAircraftProvider->renderedAircraftSituations();
        }

        Aviation::CAircraftSituationList &CRenderedAircraftProviderSupport::renderedAircraftSituations()
        {
            Q_ASSERT_X(this->m_renderedAircraftProvider, "renderedAircraftSituations", "No object available");
            return this->m_renderedAircraftProvider->renderedAircraftSituations();
        }

        bool CRenderedAircraftProviderSupport::providerUpdateAircraftModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const QString &originator)
        {
            Q_ASSERT_X(this->m_renderedAircraftProvider, "providerUpdateAircraftModel", "No object available");
            return this->m_renderedAircraftProvider->updateAircraftModel(callsign, model, originator);
        }

        bool CRenderedAircraftProviderSupport::providerUpdateAircraftEnabled(const Aviation::CCallsign &callsign, bool enabledForRedering, const QString &originator)
        {
            Q_ASSERT_X(this->m_renderedAircraftProvider, "providerUpdateAircraftEnabled", "No object available");
            return this->m_renderedAircraftProvider->updateAircraftEnabled(callsign, enabledForRedering, originator);
        }

    } // namespace
} // namespace
