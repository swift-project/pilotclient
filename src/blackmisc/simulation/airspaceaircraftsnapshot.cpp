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

namespace BlackMisc
{
    namespace Simulation
    {
        CAirspaceAircraftSnapshot::CAirspaceAircraftSnapshot()
        { }

        CAirspaceAircraftSnapshot::CAirspaceAircraftSnapshot(const CSimulatedAircraftList &allAircraft)
        {
            if (!allAircraft.isEmpty())
            {
                CSimulatedAircraftList aircraft(allAircraft);
                aircraft.sortByDistanceToOwnAircraft();
                CSimulatedAircraftList vtolAircraft(aircraft.findByVtol(true));
                m_aircraftCallsignsByDistance = aircraft.getCallsigns();
                m_enabledAircraftCallsignsByDistance = aircraft.findByEnabled(true).getCallsigns();
                m_disabledAircraftCallsignsByDistance = aircraft.findByEnabled(false).getCallsigns();
                m_vtolAircraftCallsignsByDistance = vtolAircraft.getCallsigns();
                m_enabledVtolAircraftCallsignsByDistance = vtolAircraft.findByEnabled(true).getCallsigns();
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
