/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "flightplanutils.h"
#include "airlineicaocode.h"
#include <QStringBuilder>
#include <QRegularExpression>

using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Aviation
    {
        CFlightPlanUtils::FlightPlanRemarks CFlightPlanUtils::parseFlightPlanRemarks(const QString &remarks, const CVoiceCapabilities voiceCapabilities)
        {
            // examples: VFPS = VATSIM Flightplan Prefile System
            // 1) RT/KESTREL OPR/MYTRAVEL REG/G-DAJC SEL/FP-ES PER/C NAV/RNP10
            // 2) OPR/UAL CALLSIGN/UNITED
            // 3) /v/FPL-VIR9-IS-A346/DEP/S-EGLL/ARR/KJFK/REG/G-VGAS/TCAS RVR/200 OPR/VIRGI

            FlightPlanRemarks ar; // marked as NULL so far
            if (remarks.isEmpty()) { return ar; }
            const QString r = remarks.toUpper();
            const QString cs = cut(r, "/REG"); // registration is what we call callsign
            if (!cs.isEmpty()) { ar.callsign = CCallsign(cs, CCallsign::Aircraft); }
            ar.voiceCapabilities = voiceCapabilities.isUnknown() ? CVoiceCapabilities(remarks) : voiceCapabilities;
            ar.flightOperator = cut(r, "/OPR"); // operator, e.g. British airways, sometimes people use ICAO code here
            ar.selcal = cut(r, "/SEL"); // SELCAL
            ar.radioTelephony = cut(r, "/CALLSIGN"); // used similar to radio telephony
            if (ar.radioTelephony.isEmpty()) { ar.radioTelephony = cut(r, "/RT"); }
            if (!ar.flightOperator.isEmpty() && CAirlineIcaoCode::isValidAirlineDesignator(ar.flightOperator))
            {
                // move to airline ICAO
                qSwap(ar.flightOperator, ar.airlineIcao);
            }
            ar.isNull = false;
            return ar;
        }

        QString CFlightPlanUtils::cut(const QString &remarks, const QString &marker)
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

        QString CFlightPlanUtils::aircraftIcaoCodeFromEquipmentCode(const QString &equipmentCodeAndAircraft)
        {
            // http://uk.flightaware.com/about/faq_aircraft_flight_plan_suffix.rvt
            // we expect something like H/B772/F B773 B773/F
            thread_local const QRegularExpression reg("/.");
            QString aircraftIcaoCode(equipmentCodeAndAircraft);
            aircraftIcaoCode = aircraftIcaoCode.replace(reg, "").trimmed().toUpper();
            return aircraftIcaoCode;
        }

        bool CFlightPlanUtils::FlightPlanRemarks::hasAnyRemarks() const
        {
            return this->hasAirlineRemarks() || !selcal.isEmpty() || !voiceCapabilities.isUnknown();
        }

        bool CFlightPlanUtils::FlightPlanRemarks::hasAirlineRemarks() const
        {
            return !radioTelephony.isEmpty() || !flightOperator.isEmpty() || !airlineIcao.isEmpty();
        }

        bool CFlightPlanUtils::FlightPlanRemarks::hasValidAirlineIcao() const
        {
            return !airlineIcao.isEmpty(); // member only set if valid
        }
    } // namespace
} // namespace
