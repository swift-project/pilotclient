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
        }

        CFlightPlanRemarks::CFlightPlanRemarks(const QString &remarks, CVoiceCapabilities voiceCapabilities, bool parse) :
            m_remarks(remarks), m_voiceCapabilities(voiceCapabilities)
        {
            if (parse) { this->parseFlightPlanRemarks(); }
        }

        bool CFlightPlanRemarks::hasAnyParsedRemarks() const
        {
            if (!m_isParsed) { return false; }
            return this->hasParsedAirlineRemarks() || m_selcalCode.isValid() || !m_voiceCapabilities.isUnknown();
        }

        bool CFlightPlanRemarks::hasParsedAirlineRemarks() const
        {
            if (!m_isParsed) { return false; }
            return !m_radioTelephony.isEmpty() || !m_flightOperator.isEmpty() || m_airlineIcao.hasValidDesignator();
        }

        QString CFlightPlanRemarks::convertToQString(bool i18n) const
        {
            const QString s =
                (m_registration.isEmpty() ? QStringLiteral("") : QStringLiteral("reg.: ") % m_registration.toQString(i18n))
                % (!this->hasValidAirlineIcao() ? QStringLiteral("") : QStringLiteral(" airline: ") % m_airlineIcao.getDesignator())
                % (m_radioTelephony.isEmpty() ?  QStringLiteral("") : QStringLiteral(" radio tel.:") % m_radioTelephony)
                % (m_flightOperator.isEmpty() ? QStringLiteral("") : QStringLiteral(" operator: ") % m_flightOperator)
                % (!m_selcalCode.isValid() ? QStringLiteral("") : QStringLiteral(" SELCAL: ") % m_selcalCode.getCode())
                % QStringLiteral(" voice: ") % m_voiceCapabilities.toQString(i18n);
            return s.simplified().trimmed();
        }

        QString CFlightPlanRemarks::textToVoiceCapabilities(const QString &text)
        {
            if (text.contains("TEXT", Qt::CaseInsensitive)) { return QStringLiteral("/T/"); }
            if (text.contains("RECEIVE", Qt::CaseInsensitive)) { return QStringLiteral("/R/"); }
            if (text.contains("VOICE", Qt::CaseInsensitive)) { return QStringLiteral("/V/"); }
            return QStringLiteral("");
        }

        QString CFlightPlanRemarks::replaceVoiceCapabilities(const QString &newCaps, const QString &oldRemarks)
        {
            if (newCaps.isEmpty()) { return oldRemarks; }
            if (oldRemarks.isEmpty()) { return newCaps; }

            QString r(oldRemarks);
            if (r.contains("/V/", Qt::CaseInsensitive)) { r.replace("/V/", newCaps, Qt::CaseInsensitive); return r; }
            if (r.contains("/R/", Qt::CaseInsensitive)) { r.replace("/R/", newCaps, Qt::CaseInsensitive); return r; }
            if (r.contains("/T/", Qt::CaseInsensitive)) { r.replace("/T/", newCaps, Qt::CaseInsensitive); return r; }
            return newCaps + " " + r;
        }

        void CFlightPlanRemarks::parseFlightPlanRemarks(bool force)
        {
            // examples: VFPS = VATSIM Flightplan Prefile System
            // 1) RT/KESTREL OPR/MYTRAVEL REG/G-DAJC SEL/FP-ES PER/C NAV/RNP10
            // 2) OPR/UAL CALLSIGN/UNITED
            // 3) /v/FPL-VIR9-IS-A346/DEP/S-EGLL/ARR/KJFK/REG/G-VGAS/TCAS RVR/200 OPR/VIRGIN AIRLINES

            if (!force && m_isParsed) { return; }
            m_isParsed = true;
            if (m_remarks.isEmpty()) { return; }
            const QString remarks = m_remarks.toUpper();
            const QString callsign = CCallsign::unifyCallsign(this->cut(remarks, "REG/")); // registration is a callsign
            if (CCallsign::isValidAircraftCallsign(callsign)) { m_registration = CCallsign(callsign, CCallsign::Aircraft); }
            m_voiceCapabilities = m_voiceCapabilities.isUnknown() ? CVoiceCapabilities(m_remarks) : m_voiceCapabilities;
            m_flightOperator = this->cut(remarks, "OPR/"); // operator, e.g. British airways, sometimes people use ICAO code here
            m_selcalCode = CSelcal(this->cut(remarks, "SEL/"));
            m_radioTelephony = cut(remarks, "CALLSIGN/"); // used similar to radio telephony
            if (m_radioTelephony.isEmpty()) { m_radioTelephony = cut(remarks, "RT/"); }
            if (!m_flightOperator.isEmpty() && CAirlineIcaoCode::isValidAirlineDesignator(m_flightOperator))
            {
                // if people use ICAO code as flight operator swap with airline ICAO
                m_airlineIcao = CAirlineIcaoCode(m_flightOperator);
                m_flightOperator.clear();
            }
        }


        QString CFlightPlanRemarks::cut(const QString &remarks, const QString &marker)
        {
            const int maxIndex = remarks.size() - 1;
            int f = remarks.indexOf(marker);
            if (f < 0) { return ""; }
            f += marker.length();
            if (maxIndex <= f) { return ""; }

            // the remarks are poorly formatted:
            // 1) sometimes the values are enclosed in "/", like "/REG/D-AMBZ/"
            // 2) sometimes the values are containing space, like "/OPR/DELTA AIRLINES"
            // 3) in many cases the end delimiter is a new marker or the EOL (otherwise 1 applies)

            thread_local const QRegularExpression nextMarker("\\s+\\w*/|$");
            int to1 = remarks.indexOf(nextMarker, f + 1); // for case 2,3
            if (to1 < 0) { to1 = maxIndex + 1; }
            int to2 = remarks.indexOf('/', f + 1); // for case 1
            if (to2 < 0) { to2 = maxIndex + 1; } // no more end markes, ends after last character
            const int to = qMin(to1, to2);
            const QString cut = remarks.mid(f, to - f).simplified();
            return cut;
        }

        CFlightPlan::CFlightPlan() { }

        CFlightPlan::CFlightPlan(const CCallsign &callsign, const QString &equipmentIcao, const CAirportIcaoCode &originAirportIcao, const CAirportIcaoCode &destinationAirportIcao,
                                 const CAirportIcaoCode &alternateAirportIcao, const QDateTime &takeoffTimePlanned, const QDateTime &takeoffTimeActual, const PhysicalQuantities::CTime &enrouteTime,
                                 const PhysicalQuantities::CTime &fuelTime, const CAltitude &cruiseAltitude, const PhysicalQuantities::CSpeed &cruiseTrueAirspeed, CFlightPlan::FlightRules flightRules,
                                 const QString &route, const QString &remarks)
            : m_callsign(callsign),
              m_equipmentSuffix(equipmentIcao), m_originAirportIcao(originAirportIcao), m_destinationAirportIcao(destinationAirportIcao), m_alternateAirportIcao(alternateAirportIcao),
              m_takeoffTimePlanned(takeoffTimePlanned), m_takeoffTimeActual(takeoffTimeActual), m_enrouteTime(enrouteTime), m_fuelTime(fuelTime),
              m_cruiseAltitude(cruiseAltitude), m_cruiseTrueAirspeed(cruiseTrueAirspeed), m_flightRules(flightRules),
              m_route(route.trimmed().left(MaxRouteLength).toUpper()),
              m_remarks(remarks.trimmed().left(MaxRemarksLength).toUpper())
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
            const QStringList parts = CFlightPlan::splitEquipmentCode(equipmentIcao);
            m_aircraftIcao = CAircraftIcaoCode::isValidDesignator(parts[1]) ? parts[1] : "";
            m_equipmentPrefix = parts[0];
            m_equipmentSuffix = parts[2];
        }

        void CFlightPlan::setRemarks(const QString &remarks)
        {
            m_remarks = CFlightPlanRemarks(remarks, true);
        }

        CFlightPlan::FlightRules CFlightPlan::getFlightRulesAsVFRorIFR() const
        {
            switch (this->getFlightRules())
            {
            case IFR: return IFR;
            case VFR:
            case SVFR:
            case DVFR: return VFR;
            case UNKNOWN:
            default: break;
            }
            return UNKNOWN;
        }

        QString CFlightPlan::getPrefixIcaoSuffix() const
        {
            return CFlightPlan::concatPrefixIcaoSuffix(m_equipmentPrefix, m_aircraftIcao.getDesignator(), m_equipmentSuffix);
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
            case IndexCallsign: return m_callsign.propertyByIndex(index.copyFrontRemoved());
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
                              % QLatin1Char(' ') % m_equipmentSuffix
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
            case VFR:  return QLatin1String("VFR");
            case IFR:  return QLatin1String("IFR");
            case SVFR: return QLatin1String("SVFR");
            case DVFR: return QLatin1String("DVFR");
            case UNKNOWN:
            default:   return QLatin1String("???");
            }
        }

        QString CFlightPlan::aircraftIcaoCodeFromEquipmentCode(const QString &equipmentCodeAndAircraft)
        {
            // http://uk.flightaware.com/about/faq_aircraft_flight_plan_suffix.rvt
            // we expect something like H/B772/F B773 B773/F
            thread_local const QRegularExpression reg("/.");
            QString aircraftIcaoCode(equipmentCodeAndAircraft);
            aircraftIcaoCode = aircraftIcaoCode.replace(reg, "").trimmed().toUpper();
            return aircraftIcaoCode;
        }

        QStringList CFlightPlan::splitEquipmentCode(const QString &equipmentCodeAndAircraft)
        {
            static const QStringList empty({"", "", ""});
            if (empty.isEmpty()) { return empty; }
            QStringList split = equipmentCodeAndAircraft.split('/');
            if (split.length() == 3) { return split; } // "H/B738/F"
            if (split.length() == 2)
            {
                if (split[0].length() == 1)
                {
                    // we assume prefix + ICAO
                    // e.g. "H/B748"
                    split.push_back("");
                }
                else
                {
                    // we assume ICAO + suffix
                    // e.g. "B748/F"
                    split.push_front("");
                    return split;
                }
            }

            // one part only
            if (split[0].length() > 1 && CAircraftIcaoCode::isValidDesignator(split[0]))
            {
                QStringList sl(empty);
                sl[1] = split[0]; // only ICAO
                return sl;
            }
            if (split[0].length() != 1) { return empty; } // something invalid

            // one part, one char only. hard to tell
            QStringList sl(empty);
            if (faaEquipmentCodes().contains(split[0]))
            {
                sl[2] = split[0]; // return as equipment code
                return sl;
            }
            sl[0] = split[0];
            return sl;
        }

        QString CFlightPlan::concatPrefixIcaoSuffix(const QString &prefix, const QString &icao, const QString &suffix)
        {
            QString s = prefix;
            if (!icao.isEmpty())
            {
                s += (s.isEmpty() ? QStringLiteral("") : QStringLiteral("/")) % icao;
            }
            if (suffix.isEmpty()) { return s; }
            if (s.isEmpty()) { return suffix; }
            return s % QStringLiteral("/") % suffix;
        }

        CFlightPlan::FlightRules CFlightPlan::stringToFlightRules(const QString &flightRules)
        {
            if (flightRules.length() < 3) { return UNKNOWN; }
            const QString fr(flightRules.toUpper().trimmed());
            if (fr.startsWith("DVFR")) { return DVFR; }
            if (fr.startsWith("SVFR")) { return SVFR; }
            if (fr.startsWith("VFR"))  { return VFR; }
            if (fr.startsWith("IFR"))  { return IFR; }
            return UNKNOWN;
        }

        const QStringList &CFlightPlan::faaEquipmentCodes()
        {
            // List of FAA Aircraft Equipment Codes For US Domestic Flights
            static const QStringList e({"X", "T", "U", "D", "B", "A", "M", "N", "P", "Y", "C", "I", "L", "G", "Z", "I", "W", "L"});
            return e;
        }

        const QStringList &CFlightPlan::faaEquipmentCodesInfo()
        {
            static const QStringList e(
            {
                "X No transponder",
                "T Transponder with no Mode C",
                "U Transponder with Mode C",
                "D DME: No transponder",
                "B DME: Transponder with no Mode C",
                "A DME: Transponder with Mode C",
                "M TACAN only: No transponder",
                "N TACAN only: Transponder with no Mode C",
                "P TACAN only: Transponder with Mode C",
                "Y Basic RNAV: LORAN, VOR/DME, or INS with no transponder",
                "C Basic RNAV: LORAN, VOR/DME, or INS, transponder with no Mode C",
                "I Basic RNAV: LORAN, VOR/DME, or INS, transponder with Mode C",
                "L Advanced RNAV: RNAV capability with Global Navigation Satellite System (GNSS)",
                "G Advanced RNAV: RNAV capability with GNSS and without RVSM",
                "Z Advanced RNAV: RNAV capability without GNSS and with RVSM",
                "I Advanced RNAV: RNAV capability without GNSS and without RVSM",
                "W RVSM",
                "L RVSM and /G"
            });
            return e;
        }

        const QStringList &CFlightPlan::squawkBoxEquipmentCodes()
        {
            static const QStringList e({"E", "F", "G", "R", "J", "K", "L", "Q"});
            return e;
        }

        const QStringList &CFlightPlan::squawkBoxEquipmentCodesInfo()
        {
            static const QStringList e(
            {
                "E Flight Management System (FMS) with DME/DME and IRU positioning updating",
                "F Flight Management System (FMS) with DME/DME positioning updating",
                "G Global Navigation Satellite System (GNSS), including GPS or Wide Area Augmentation System",
                "R Required navigation performance, the aircraft meets the RNP type prescribed for the route segment(s), route(s) and or area concerned",
                "J RVSM + E",
                "L RVSM + F",
                "L RVSM + G",
                "Q RVSM + E"
            });
            return e;
        }

        const QStringList &CFlightPlan::equipmentCodes()
        {
            static const QStringList e = []
            {
                QSet<QString> el(CFlightPlan::faaEquipmentCodes().toSet());
                el.unite(CFlightPlan::squawkBoxEquipmentCodes().toSet());
                return el.toList();
            }();
            return e;
        }

        const QStringList &CFlightPlan::equipmentCodesInfo()
        {
            static const QStringList e = []
            {
                QStringList info(CFlightPlan::faaEquipmentCodesInfo());
                info.append(CFlightPlan::squawkBoxEquipmentCodesInfo());
                return info;
            }();
            return e;
        }

        const QStringList &CFlightPlan::prefixCodes()
        {
            static const QStringList p({"T", "H"});
            return p;
        }

        CIcon CFlightPlan::toIcon() const
        {
            return CIcon::iconByIndex(CIcons::StandardIconAppFlightPlan16);
        }
    } // namespace
} // namespace
