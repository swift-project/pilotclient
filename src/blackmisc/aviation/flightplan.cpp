/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "flightplan.h"
#include "airlineicaocode.h"
#include "flightplan.h"
#include "selcal.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/icons.h"
#include <QStringBuilder>
#include <QRegularExpression>

using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Aviation
    {
        CFlightPlanRemarks::CFlightPlanRemarks()
        { }

        CFlightPlanRemarks::CFlightPlanRemarks(const QString &remarks, bool parse) : m_remarks(remarks)
        {
            if (parse) { this->parseFlightPlanRemarks(); }
            m_isNull = false;
        }

        CFlightPlanRemarks::CFlightPlanRemarks(const QString &remarks, CVoiceCapabilities voiceCapabilities, bool parse) :
            m_remarks(remarks), m_voiceCapabilities(voiceCapabilities)
        {
            if (parse) { this->parseFlightPlanRemarks(); }
            m_isNull = false;
        }

        bool CFlightPlanRemarks::hasAnyParsedRemarks() const
        {
            return this->hasParsedAirlineRemarks() || !m_selcalCode.isEmpty() || !m_voiceCapabilities.isUnknown();
        }

        bool CFlightPlanRemarks::hasParsedAirlineRemarks() const
        {
            return !m_radioTelephony.isEmpty() || !m_flightOperator.isEmpty() || !m_airlineIcao.isEmpty();
        }

        QString CFlightPlanRemarks::convertToQString(bool i18n) const
        {
            const QString s = m_callsign.toQString(i18n)
                              % QLatin1Char(' ') % m_airlineIcao
                              % QLatin1Char(' ') % m_radioTelephony
                              % QLatin1Char(' ') % m_flightOperator
                              % QLatin1Char(' ') % m_selcalCode
                              % QLatin1Char(' ') % m_voiceCapabilities.toQString(i18n);
            return s;
        }

        void CFlightPlanRemarks::parseFlightPlanRemarks()
        {
            // examples: VFPS = VATSIM Flightplan Prefile System
            // 1) RT/KESTREL OPR/MYTRAVEL REG/G-DAJC SEL/FP-ES PER/C NAV/RNP10
            // 2) OPR/UAL CALLSIGN/UNITED
            // 3) /v/FPL-VIR9-IS-A346/DEP/S-EGLL/ARR/KJFK/REG/G-VGAS/TCAS RVR/200 OPR/VIRGI

            if (m_isParsed) { return; }
            m_isParsed = true;
            if (m_remarks.isEmpty()) { return; }
            const QString remarks = m_remarks.toUpper();
            const QString callsign = CCallsign::unifyCallsign(this->cut(remarks, "REG/")); // registration is a callsign
            if (CCallsign::isValidAircraftCallsign(callsign)) { m_registration = CCallsign(callsign, CCallsign::Aircraft); }
            m_voiceCapabilities = m_voiceCapabilities.isUnknown() ? CVoiceCapabilities(m_remarks) : m_voiceCapabilities;
            m_flightOperator = this->cut(r, "OPR/"); // operator, e.g. British airways, sometimes people use ICAO code here
            const QString selcal = this->cut(r, "SEL/"); // SELCAL
            if (CSelcal::isValidCode(selcal)) m_selcalCode = selcal;
            m_radioTelephony = cut(r, "CALLSIGN/"); // used similar to radio telephony
            if (m_radioTelephony.isEmpty()) { m_radioTelephony = cut(r, "RT/"); }
            if (!m_flightOperator.isEmpty() && CAirlineIcaoCode::isValidAirlineDesignator(m_flightOperator))
            {
                // if people use ICAO as flight operator move to airline ICAO
                qSwap(m_flightOperator, m_airlineIcao);
            }
            m_isNull = false;
        }

        QString CFlightPlanRemarks::aircraftIcaoCodeFromEquipmentCode(const QString &equipmentCodeAndAircraft)
        {
            // http://uk.flightaware.com/about/faq_aircraft_flight_plan_suffix.rvt
            // we expect something like H/B772/F B773 B773/F
            thread_local const QRegularExpression reg("/.");
            QString aircraftIcaoCode(equipmentCodeAndAircraft);
            aircraftIcaoCode = aircraftIcaoCode.replace(reg, "").trimmed().toUpper();
            return aircraftIcaoCode;
        }

        QString CFlightPlanRemarks::cut(const QString &remarks, const QString &marker)
        {
            const int maxIndex = remarks.size() - 1;
            int f = remarks.indexOf(marker);
            if (f < 0) { return ""; }
            f += marker.length();
            if (maxIndex <= f) { return ""; }
            int to = remarks.indexOf(' ', f + 1);
            if (to < 0) { to = maxIndex; } // no more spaces
            const QString cut = remarks.mid(f, to - f).replace('/', ' '); // variations like /OPR/EASYJET/
            // problem is that this cuts something like "Uzbekistan Airways"
            return cut;
        }

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
            const QString aircraftIcao = CFlightPlanRemarks::aircraftIcaoCodeFromEquipmentCode(equipmentIcao);
            m_aircraftIcao = CAircraftIcaoCode::isValidDesignator(aircraftIcao) ? aircraftIcao : "";
        }

        void CFlightPlan::setRemarks(const QString &remarks)
        {
            m_remarks = CFlightPlanRemarks(remarks, true);
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
                              % QLatin1Char(' ') % this->getRemarks();
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
