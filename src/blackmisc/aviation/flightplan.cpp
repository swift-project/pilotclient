/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"
#include <QStringBuilder>

namespace BlackMisc
{
    namespace Aviation
    {
        CFlightPlan::CFlightPlan() { }

        CFlightPlan::CFlightPlan(const QString &equipmentIcao, const CAirportIcaoCode &originAirportIcao, const CAirportIcaoCode &destinationAirportIcao, const CAirportIcaoCode &alternateAirportIcao, const QDateTime &takeoffTimePlanned, const QDateTime &takeoffTimeActual, const PhysicalQuantities::CTime &enrouteTime, const PhysicalQuantities::CTime &fuelTime, const CAltitude &cruiseAltitude, const PhysicalQuantities::CSpeed &cruiseTrueAirspeed, CFlightPlan::FlightRules flightRules, const QString &route, const QString &remarks)
            : m_equipmentIcao(equipmentIcao), m_originAirportIcao(originAirportIcao), m_destinationAirportIcao(destinationAirportIcao), m_alternateAirportIcao(alternateAirportIcao),
              m_takeoffTimePlanned(takeoffTimePlanned), m_takeoffTimeActual(takeoffTimeActual), m_enrouteTime(enrouteTime), m_fuelTime(fuelTime),
              m_cruiseAltitude(cruiseAltitude), m_cruiseTrueAirspeed(cruiseTrueAirspeed), m_flightRules(flightRules),
              m_route(route.trimmed().left(MaxRouteLength).toUpper()), m_remarks(remarks.trimmed().left(MaxRemarksLength).toUpper())
        {
            m_enrouteTime.switchUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin());
            m_fuelTime.switchUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin());
        }

        QString CFlightPlan::convertToQString(bool i18n) const
        {
            const QString s = m_equipmentIcao
                              % QLatin1Char(' ') % m_originAirportIcao.toQString(i18n)
                              % QLatin1Char(' ') % m_destinationAirportIcao.toQString(i18n)
                              % QLatin1Char(' ') % m_alternateAirportIcao.toQString(i18n)
                              % QLatin1Char(' ') % m_takeoffTimePlanned.toString("ddhhmm")
                              % QLatin1Char(' ') % m_takeoffTimeActual.toString("ddhhmm")
                              % QLatin1Char(' ') % m_enrouteTime.toQString(i18n)
                              % QLatin1Char(' ') % m_fuelTime.toQString(i18n)
                              % QLatin1Char(' ') % m_cruiseAltitude.toQString(i18n)
                              % QLatin1Char(' ') % m_cruiseTrueAirspeed.toQString(i18n)
                              % QLatin1Char(' ') % m_route
                              % QLatin1Char(' ') % m_remarks;
            return s;
        }

        const QString CFlightPlan::flightruleToString(CFlightPlan::FlightRules rule)
        {
            switch (rule)
            {
            case VFR:   return QLatin1String("VFR");
            case IFR:   return QLatin1String("IFR");
            case SVFR:  return QLatin1String("SVFR");
            default:    return QLatin1String("???");
            }
        }

        BlackMisc::CIcon CFlightPlan::toIcon() const
        {
            return BlackMisc::CIcon::iconByIndex(CIcons::StandardIconAppFlightPlan16);
        }
    } // namespace
} // namespace
