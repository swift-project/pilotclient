/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avflightplan.h"
#include "iconlist.h"

namespace BlackMisc
{
    namespace Aviation
    {

        QString CFlightPlan::convertToQString(bool i18n) const
        {
            QString s;
            s.append(m_equipmentIcao);
            s.append(" ").append(m_originAirportIcao.toQString(i18n));
            s.append(" ").append(m_destinationAirportIcao.toQString(i18n));
            s.append(" ").append(m_alternateAirportIcao.toQString(i18n));
            s.append(" ").append(m_takeoffTimePlanned.toString("ddhhmm"));
            s.append(" ").append(m_takeoffTimeActual.toString("ddhhmm"));
            s.append(" ").append(m_enrouteTime.toQString(i18n));
            s.append(" ").append(m_fuelTime.toQString(i18n));
            s.append(" ").append(m_cruiseAltitude.toQString(i18n));
            s.append(" ").append(m_cruiseTrueAirspeed.toQString(i18n));
            switch (m_flightRules)
            {
            case VFR:   s.append(" VFR"); break;
            case IFR:   s.append(" IFR"); break;
            case SVFR:  s.append(" SVFR"); break;
            default:    s.append(" ???"); break;
            }
            s.append(" ").append(m_route);
            s.append(" / ").append(m_remarks);
            return s;
        }

        BlackMisc::CIcon CFlightPlan::toIcon() const
        {
            return BlackMisc::CIconList::iconByIndex(CIcons::StandardIconAppFlightPlan16);
        }

    } // namespace
} // namespace
