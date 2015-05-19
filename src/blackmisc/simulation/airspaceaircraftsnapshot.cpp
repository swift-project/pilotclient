/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airspaceaircraftsnapshot.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Simulation
    {
        CAirspaceAircraftSnapshot::CAirspaceAircraftSnapshot()
        { }

        CAirspaceAircraftSnapshot::CAirspaceAircraftSnapshot(
            const CSimulatedAircraftList &allAircraft,
            bool restricted, int maxAircraft, const CLength &maxRenderedDistance, const CLength &maxRenderedBoundary) :
            m_timestampMsSinceEpoch(QDateTime::currentMSecsSinceEpoch()),
            m_restricted(restricted),
            m_threadName(QThread::currentThread()->objectName())
        {
            m_renderingEnabled = !restricted || (
                                     maxAircraft > 0 &&
                                     (maxRenderedBoundary.isNull() || maxRenderedBoundary.isPositiveWithEpsilonConsidered()) &&
                                     (maxRenderedDistance.isNull() || maxRenderedDistance.isPositiveWithEpsilonConsidered())
                                 );
            if (allAircraft.isEmpty()) { return; }

            CSimulatedAircraftList aircraft(allAircraft);
            aircraft.sortByDistanceToOwnAircraft();
            CSimulatedAircraftList vtolAircraft(aircraft.findByVtol(true));
            m_aircraftCallsignsByDistance = aircraft.getCallsigns();
            m_vtolAircraftCallsignsByDistance = vtolAircraft.getCallsigns();

            // no restrictions, just find by attributes
            if (!restricted)
            {
                m_enabledAircraftCallsignsByDistance = aircraft.findByEnabled(true).getCallsigns();
                m_disabledAircraftCallsignsByDistance = aircraft.findByEnabled(false).getCallsigns();
                m_enabledVtolAircraftCallsignsByDistance = vtolAircraft.findByEnabled(true).getCallsigns();
                return;
            }

            // no rendering, this means all aircraft are disabled
            if (!m_renderingEnabled)
            {
                m_disabledAircraftCallsignsByDistance = aircraft.getCallsigns();
                return;
            }

            // restricted
            int count = 0; // when max. aircraft reached?
            for (const CSimulatedAircraft &currentAircraft : aircraft)
            {
                CCallsign cs(currentAircraft.getCallsign());
                if (currentAircraft.isEnabled())
                {
                    CLength distance(currentAircraft.getDistanceToOwnAircraft());
                    if (count >= maxAircraft ||
                            (!maxRenderedDistance.isNull() && distance >= maxRenderedBoundary) ||
                            (!maxRenderedBoundary.isNull() && distance >= maxRenderedBoundary))
                    {
                        m_disabledAircraftCallsignsByDistance.push_back(cs);
                    }
                    else
                    {
                        count++;
                        m_enabledAircraftCallsignsByDistance.push_back(cs);
                        if (currentAircraft.isVtol()) { m_enabledVtolAircraftCallsignsByDistance.push_back(cs); }
                    }
                }
                else
                {
                    m_disabledAircraftCallsignsByDistance.push_back(cs);
                }
            }
        }

        bool CAirspaceAircraftSnapshot::isValidSnapshot() const
        {
            return m_timestampMsSinceEpoch > 0;
        }

        void CAirspaceAircraftSnapshot::setRestrictionChanged(const CAirspaceAircraftSnapshot &snapshot)
        {
            if (this->isValidSnapshot() == snapshot.isValidSnapshot())
            {
                this->m_restrictionChanged = (snapshot.m_restricted != this->m_restricted);
            }
            else
            {
                this->m_restrictionChanged = true;
            }
        }

        CVariant CAirspaceAircraftSnapshot::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            return CValueObject::propertyByIndex(index);
        }

        void CAirspaceAircraftSnapshot::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }
            CValueObject::setPropertyByIndex(variant, index);
        }

        QString CAirspaceAircraftSnapshot::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return this->getTimestamp().toString();
        }

    } // ns
} // ns
