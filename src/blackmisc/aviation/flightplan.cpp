/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "flightplan.h"
#include "flightplanutils.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"
#include <QStringBuilder>

namespace BlackMisc
{
    namespace Aviation
    {
        CFlightPlan::CFlightPlan() { }

        CFlightPlan::CFlightPlan(const CCallsign &callsign, const QString &equipmentIcao, const CAirportIcaoCode &originAirportIcao, const CAirportIcaoCode &destinationAirportIcao,
                                 const CAirportIcaoCode &alternateAirportIcao, const QDateTime &takeoffTimePlanned, const QDateTime &takeoffTimeActual, const PhysicalQuantities::CTime &enrouteTime,
                                 const PhysicalQuantities::CTime &fuelTime, const CAltitude &cruiseAltitude, const PhysicalQuantities::CSpeed &cruiseTrueAirspeed, CFlightPlan::FlightRules flightRules,
                                 const QString &route, const QString &remarks)
            : m_callsign(callsign),
              m_equipmentIcao(equipmentIcao), m_originAirportIcao(originAirportIcao), m_destinationAirportIcao(destinationAirportIcao), m_alternateAirportIcao(alternateAirportIcao),
              m_takeoffTimePlanned(takeoffTimePlanned), m_takeoffTimeActual(takeoffTimeActual), m_enrouteTime(enrouteTime), m_fuelTime(fuelTime),
              m_cruiseAltitude(cruiseAltitude), m_cruiseTrueAirspeed(cruiseTrueAirspeed), m_flightRules(flightRules),
              m_route(route.trimmed().left(MaxRouteLength).toUpper()), m_remarks(remarks.trimmed().left(MaxRemarksLength).toUpper())
        {
            m_callsign.setTypeHint(CCallsign::Aircraft);
            m_enrouteTime.switchUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin());
            m_fuelTime.switchUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin());
        }

        void CFlightPlan::setCallsign(const CCallsign &callsign)
        {
            m_callsign = callsign;
            m_callsign.setTypeHint(CCallsign::Aircraft);
        }

        void CFlightPlan::setEquipmentIcao(const QString &equipmentIcao)
        {
            m_equipmentIcao = equipmentIcao;
            const QString aircraftIcao = CFlightPlanUtils::aircraftIcaoCodeFromEquipmentCode(equipmentIcao);
            m_aircraftIcao = CAircraftIcaoCode::isValidDesignator(aircraftIcao) ? aircraftIcao : "";
        }

        CFlightPlanUtils::FlightPlanRemarks CFlightPlan::getParsedRemarks() const
        {
            return CFlightPlanUtils::parseFlightPlanRemarks(this->getRemarks());
        }

        CVariant CFlightPlan::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAlternateAirportIcao: return m_alternateAirportIcao.propertyByIndex(index.copyFrontRemoved());
            case IndexDestinationAirportIcao: return m_destinationAirportIcao.propertyByIndex(index.copyFrontRemoved());
            case IndexOriginAirportIcao: return m_originAirportIcao.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign: return this->m_callsign.propertyByIndex(index.copyFrontRemoved());
            case IndexRemarks: return CVariant::from(m_remarks);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CFlightPlan::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CFlightPlan>(); return; }
            if (ITimestampBased::canHandleIndex(index))
            {
                ITimestampBased::setPropertyByIndex(index, variant);
                return;
            }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAlternateAirportIcao: m_alternateAirportIcao.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexDestinationAirportIcao: m_destinationAirportIcao.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexOriginAirportIcao: m_originAirportIcao.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexCallsign: m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexRemarks: this->setRemarks(variant.toQString()); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        QString CFlightPlan::convertToQString(bool i18n) const
        {
            const QString s = m_callsign.toQString(i18n)
                              % QLatin1Char(' ') % m_equipmentIcao
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

        const QString CFlightPlan::flightRuleToString(CFlightPlan::FlightRules rule)
        {
            switch (rule)
            {
            case VFR:   return QLatin1String("VFR");
            case IFR:   return QLatin1String("IFR");
            case SVFR:  return QLatin1String("SVFR");
            default:    return QLatin1String("???");
            }
        }

        CIcon CFlightPlan::toIcon() const
        {
            return CIcon::iconByIndex(CIcons::StandardIconAppFlightPlan16);
        }
    } // namespace
} // namespace
