/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/iconlist.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/json.h"

#include <QFile>
#include <QDateTime>
#include <QTimeZone>
#include <QStringBuilder>
#include <QRegularExpression>
#include <QDomDocument>

using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc::Aviation
{
    CFlightPlanRemarks::CFlightPlanRemarks()
    { }

    CFlightPlanRemarks::CFlightPlanRemarks(const QString &remarks, bool parse) : m_remarks(cleanRemarks(remarks))
    {
        if (parse) { this->parseFlightPlanRemarks(); }
    }

    CFlightPlanRemarks::CFlightPlanRemarks(const QString &remarks, CVoiceCapabilities voiceCapabilities, bool parse) :
        m_remarks(cleanRemarks(remarks)), m_voiceCapabilities(voiceCapabilities)
    {
        if (parse) { this->parseFlightPlanRemarks(); }
    }

    bool CFlightPlanRemarks::setSelcalCode(const QString &selcal)
    {
        if (m_selcalCode == selcal || selcal.length() != 4) { return false; }
        const QString r = CFlightPlanRemarks::replaceRemark(m_remarks, QStringLiteral("SEL/"), QStringLiteral("SEL/%1").arg(selcal));
        m_remarks = r;
        return true;
    }

    void CFlightPlanRemarks::setVoiceCapabilities(const CVoiceCapabilities &capabilities)
    {
        m_voiceCapabilities = capabilities;
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
            (m_registration.isEmpty() ? QString() : u"reg.: " % m_registration.toQString(i18n))
            % (!this->hasValidAirlineIcao() ? QString() : u" airline: " % m_airlineIcao.getDesignator())
            % (m_radioTelephony.isEmpty()   ? QString() : u" radio tel.:" % m_radioTelephony)
            % (m_flightOperator.isEmpty()   ? QString() : u" operator: "  % m_flightOperator)
            % (!m_selcalCode.isValid()      ? QString() : u" SELCAL: " % m_selcalCode.getCode())
            % u" voice: " % m_voiceCapabilities.toQString(i18n);
        return s.simplified().trimmed();
    }

    QString CFlightPlanRemarks::textToVoiceCapabilitiesRemarks(const QString &text)
    {
        const CVoiceCapabilities vc = CVoiceCapabilities::fromText(text);
        return vc.toFlightPlanRemarks();
    }

    QString CFlightPlanRemarks::replaceVoiceCapabilities(const QString &newCapRemarks, const QString &oldRemarks)
    {
        if (newCapRemarks.isEmpty()) { return oldRemarks; }
        if (oldRemarks.isEmpty()) { return newCapRemarks; }

        QString r(oldRemarks);
        if (r.contains("/V/", Qt::CaseInsensitive)) { r.replace("/V/", newCapRemarks, Qt::CaseInsensitive); return r; }
        if (r.contains("/R/", Qt::CaseInsensitive)) { r.replace("/R/", newCapRemarks, Qt::CaseInsensitive); return r; }
        if (r.contains("/T/", Qt::CaseInsensitive)) { r.replace("/T/", newCapRemarks, Qt::CaseInsensitive); return r; }
        return newCapRemarks % u' ' % r;
    }

    QString CFlightPlanRemarks::cleanRemarks(const QString &remarksIn)
    {
        QString r = remarksIn;
        r.replace(':', ' ');
        r = asciiOnlyString(simplifyAccents(r));
        return r;
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
        const QString callsign = CCallsign::unifyCallsign(this->getRemark(remarks, "REG/")); // registration is a callsign
        if (CCallsign::isValidAircraftCallsign(callsign)) { m_registration = CCallsign(callsign, CCallsign::Aircraft); }
        m_voiceCapabilities = m_voiceCapabilities.isUnknown() ? CVoiceCapabilities(m_remarks) : m_voiceCapabilities;
        m_flightOperator = this->getRemark(remarks, "OPR/"); // operator, e.g. British airways, sometimes people use ICAO code here
        m_selcalCode = CSelcal(this->getRemark(remarks, "SEL/"));
        m_radioTelephony = getRemark(remarks, "CALLSIGN/"); // used similar to radio telephony
        if (m_radioTelephony.isEmpty()) { m_radioTelephony = getRemark(remarks, "RT/"); }
        if (!m_flightOperator.isEmpty() && CAirlineIcaoCode::isValidAirlineDesignator(m_flightOperator))
        {
            // if people use ICAO code as flight operator swap with airline ICAO
            m_airlineIcao = CAirlineIcaoCode(m_flightOperator);
            m_flightOperator.clear();
        }
    }

    void CFlightPlanRemarks::setIcaoEquipmentCodes(const QString &eq)
    {
        // https://en.wikipedia.org/wiki/Equipment_codes
        const QString r = eq.toUpper().trimmed();
        QStringList remarks;
        if (r.isEmpty()) { return; }
        if (r.contains('W'))
        {
            if (r.contains('G')) { remarks << "/L"; }
            else if (r.contains('D') || r.contains('L') || r.contains('O')) { remarks << "/Z"; }
            else if (r.contains('A')) { remarks << "/W"; }
        }
        else
        {
            if (r.contains('G')) { remarks << "/G"; }
            else if (r.contains('D') || r.contains('L') || r.contains('O')) { remarks << "/I"; }
        }

        if (r.contains('G')) { remarks << "NAV/GPSRNAV"; }
        else if (r.contains('D') || r.contains('L') || r.contains('O')) { remarks << "NAV/VORNDB"; }

        m_remarks = remarks.join(" ");
        this->parseFlightPlanRemarks(true);
    }

    QString CFlightPlanRemarks::getRemark(const QString &remarks, const QString &marker)
    {
        const int maxIndex = remarks.size() - 1;
        int f = remarks.indexOf(marker);
        if (f < 0) { return {}; }
        f += marker.length();
        if (maxIndex <= f) { return {}; }

        // the remarks are poorly formatted:
        // 1) sometimes the values are enclosed in "/", like "/REG/D-AMBZ/"
        // 2) sometimes the values are containing space, like "/OPR/DELTA AIRLINES"
        // 3) in many cases the end delimiter is a new marker or the EOL (otherwise 1 applies)

        thread_local const QRegularExpression nextMarker("\\s+\\w*/|$");
        int to1 = remarks.indexOf(nextMarker, f + 1); // for case 2,3
        if (to1 < 0) { to1 = maxIndex + 1; }
        int to2 = remarks.indexOf('/', f + 1); // for case 1
        if (to2 < 0) { to2 = maxIndex + 1; }   // no more end markers, ends after last character
        const int to = qMin(to1, to2);
        const QString cut = remarks.mid(f, to - f).simplified();
        return cut;
    }

    QString CFlightPlanRemarks::replaceRemark(const QString &remarks, const QString &marker, const QString &newRemark)
    {
        QString r(remarks);
        const int maxIndex = remarks.size() - 1;
        int f = remarks.indexOf(marker);
        if (f >= 0)
        {
            f += marker.length();
            if (maxIndex <= f) { return remarks; }
            thread_local const QRegularExpression nextMarker("\\s+\\w*/|$");
            int to1 = remarks.indexOf(nextMarker, f + 1); // for case 2,3
            if (to1 < 0) { to1 = maxIndex + 1; }
            int to2 = remarks.indexOf('/', f + 1); // for case 1
            if (to2 < 0) { to2 = maxIndex + 1; }   // no more end markers, ends after last character
            const int to = qMin(to1, to2);
            r.remove(f, to - f);
        }
        return r.isEmpty() ? newRemark : r % u" " % newRemark;
    }

    const QStringList &CFlightPlan::getLogCategories()
    {
        static const QStringList cats { CLogCategories::flightPlan() };
        return cats;
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
        m_prefix = parts[0];
        m_equipmentSuffix = parts[2];
    }

    void CFlightPlan::setTakeoffTimePlanned(const QDateTime &takeoffTimePlanned)
    {
        m_takeoffTimePlanned = takeoffTimePlanned.toUTC();
    }

    void CFlightPlan::setTakeoffTimeActual(const QDateTime &takeoffTimeActual)
    {
        m_takeoffTimeActual = takeoffTimeActual.toUTC();
    }

    void CFlightPlan::setFlightRule(const QString &flightRule)
    {
        const CFlightPlan::FlightRules r = CFlightPlan::stringToFlightRules(flightRule);
        this->setFlightRule(r);
    }

    void CFlightPlan::setRoute(const QString &route)
    {
        QString r = route;
        r.replace(':', ' ');
        m_route = asciiOnlyString(r).left(MaxRouteLength).toUpper();
    }

    void CFlightPlan::setRemarks(const QString &remarks)
    {
        m_remarks = CFlightPlanRemarks(remarks, true);
    }

    void CFlightPlan::setVoiceCapabilities(const QString &capabilities)
    {
        const CVoiceCapabilities vc = CVoiceCapabilities::fromText(capabilities);
        m_remarks.setVoiceCapabilities(vc);
    }

    QString CFlightPlan::getTakeoffTimePlannedHourMin() const
    {
        return m_takeoffTimePlanned.toString("hh:mm");
    }

    QString CFlightPlan::getTakeoffTimeActualHourMin() const
    {
        return m_takeoffTimeActual.toString("hh:mm");
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

    void CFlightPlan::setPrefix(const QString &prefix)
    {
        m_prefix = prefix;
        m_prefix.remove('/');
    }

    void CFlightPlan::setHeavy()
    {
        this->setPrefix("H");
    }

    void CFlightPlan::setEquipmentSuffix(const QString &suffix)
    {
        m_equipmentSuffix = suffix;
        m_equipmentSuffix.remove('/');
    }

    QString CFlightPlan::getCombinedPrefixIcaoSuffix() const
    {
        return CFlightPlan::concatPrefixIcaoSuffix(m_prefix, m_aircraftIcao.getDesignator(), m_equipmentSuffix);
    }

    QVariant CFlightPlan::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAlternateAirportIcao: return m_alternateAirportIcao.propertyByIndex(index.copyFrontRemoved());
        case IndexDestinationAirportIcao: return m_destinationAirportIcao.propertyByIndex(index.copyFrontRemoved());
        case IndexOriginAirportIcao: return m_originAirportIcao.propertyByIndex(index.copyFrontRemoved());
        case IndexCallsign: return m_callsign.propertyByIndex(index.copyFrontRemoved());
        case IndexRemarks: return QVariant::fromValue(m_remarks);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CFlightPlan::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CFlightPlan>(); return; }
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
        case IndexRemarks: this->setRemarks(variant.toString()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    QString CFlightPlan::convertToQString(bool i18n) const
    {
        return this->buildString(i18n, " ");
    }

    QString CFlightPlan::asHTML(bool i18n) const
    {
        return this->buildString(i18n, "<br>");
    }

    QString CFlightPlan::buildString(bool i18n, const QString &separator) const
    {
        const QString s = m_callsign.toQString(i18n)
                            % u" aircraft: " % m_equipmentSuffix
                            % separator
                            % u"origin: "       % m_originAirportIcao.toQString(i18n)
                            % u" destination: " % m_destinationAirportIcao.toQString(i18n)
                            % u" alternate: "   % m_alternateAirportIcao.toQString(i18n)
                            % separator
                            % u"takeoff planed: " % m_takeoffTimePlanned.toString("ddhhmm")
                            % u" actual: " % m_takeoffTimeActual.toString("ddhhmm")
                            % separator
                            % u"enroute time: " % m_enrouteTime.toQString(i18n)
                            % u" fuel time:" % m_fuelTime.toQString(i18n)
                            % separator
                            % u"altitude: " % m_cruiseAltitude.toQString(i18n)
                            % u" speed: "   % m_cruiseTrueAirspeed.toQString(i18n)
                            % separator
                            % u"route: " % m_route
                            % separator
                            % u"remarks: " % this->getRemarks();
        return s;
    }

    CFlightPlan CFlightPlan::fromVPilotFormat(const QString &vPilotData)
    {
        if (vPilotData.isEmpty()) { return CFlightPlan(); }
        QDomDocument doc;
        doc.setContent(vPilotData);
        const QDomElement fpDom = doc.firstChildElement();
        const QString type = fpDom.attribute("FlightType");

        CFlightPlan fp;
        fp.setFlightRule(CFlightPlan::stringToFlightRules(type));

        const int airspeedKts = fpDom.attribute("CruiseSpeed").toInt();
        const CSpeed airspeed(airspeedKts, CSpeedUnit::kts());
        fp.setCruiseTrueAirspeed(airspeed);

        fp.setOriginAirportIcao(fpDom.attribute("DepartureAirport"));
        fp.setDestinationAirportIcao(fpDom.attribute("DestinationAirport"));
        fp.setAlternateAirportIcao(fpDom.attribute("AlternateAirport"));
        fp.setRemarks(fpDom.attribute("Remarks"));
        fp.setRoute(fpDom.attribute("Route"));

        const QString voice = fpDom.attribute("VoiceType");
        fp.setVoiceCapabilities(voice);

        const QString prefix = fpDom.attribute("EquipmentPrefix");
        if (prefix.isEmpty())
        {
            const bool heavy = stringToBool(fpDom.attribute("IsHeavy"));
            if (heavy) { fp.setHeavy(); }
        }
        else
        {
            fp.setPrefix(prefix);
        }
        fp.setEquipmentSuffix(fpDom.attribute("EquipmentSuffix"));

        const int fuelMins = fpDom.attribute("FuelMinutes").toInt() + 60 * fpDom.attribute("FuelHours").toInt();
        const CTime fuelTime(fuelMins, CTimeUnit::min());

        const int enrouteMins = fpDom.attribute("EnrouteMinutes").toInt() + 60 * fpDom.attribute("EnrouteHours").toInt();
        const CTime enrouteTime(enrouteMins, CTimeUnit::min());

        const QString altStr = fpDom.attribute("CruiseAltitude");
        CAltitude alt(altStr.length() < 4 ? "FL" + altStr : altStr + "ft");
        alt.toFlightLevel();

        fp.setCruiseAltitude(alt);
        fp.setFuelTime(fuelTime);
        fp.setEnrouteTime(enrouteTime);

        const QString departureTime = fpDom.attribute("DepartureTime");
        if (departureTime.length() == 4)
        {
            CTime depTime;
            if (depTime.parseFromString_hhmm(departureTime))
            {
                fp.setTakeoffTimePlanned(depTime.toQDateTime());
            }
        }

        return fp;
    }

    CFlightPlan CFlightPlan::fromSB4Format(const QString &sbData)
    {
        if (sbData.isEmpty()) { return CFlightPlan(); }
        CFlightPlan fp;
        const QMap<QString, QString> values = parseIniValues(sbData);
        const QString altStr = values.value("Altitude");
        const CAltitude alt(altStr.length() < 4 ? "FL" + altStr : altStr + "ft");

        const QString type = values.value("Type"); // IFR/VFR
        fp.setFlightRule(type == "0" ? IFR : VFR);

        const int fuelMins = values.value("FuelMinutes").toInt() + 60 * values.value("FuelHours").toInt();
        const CTime fuelTime(fuelMins, CTimeUnit::min());

        const int enrouteMins = values.value("FlightMinutes").toInt() + 60 * values.value("FlightHours").toInt();
        const CTime enrouteTime(enrouteMins, CTimeUnit::min());

        fp.setOriginAirportIcao(values.value("Departure"));
        fp.setDestinationAirportIcao(values.value("Arrival"));
        fp.setAlternateAirportIcao(values.value("Alternate"));
        fp.setRemarks(values.value("Remarks"));
        fp.setRoute(values.value("Route"));

        fp.setCruiseAltitude(alt);
        fp.setFuelTime(fuelTime);
        fp.setEnrouteTime(enrouteTime);

        fp.setTakeoffTimePlanned(QDateTime::currentDateTimeUtc());

        int airspeedKts = values.value("Airspeed").toInt();
        const CSpeed airspeed(airspeedKts, CSpeedUnit::kts());
        fp.setCruiseTrueAirspeed(airspeed);

        const bool heavy = stringToBool(values.value("Heavy"));
        if (heavy) { fp.setHeavy(); }

        return fp;
    }

    CFlightPlan CFlightPlan::fromSimBriefFormat(const QString &simBrief)
    {
        if (simBrief.isEmpty()) { return CFlightPlan(); }
        CFlightPlan fp;
        QDomDocument doc;
        doc.setContent(simBrief);
        const QDomNodeList originList = doc.elementsByTagName("origin");
        if (!originList.isEmpty())
        {
            const QDomNode origin = originList.at(0);
            const QString icao = origin.firstChildElement("icao_code").text();
            fp.setOriginAirportIcao(icao);
        }
        const QDomNodeList destList = doc.elementsByTagName("destination");
        if (!destList.isEmpty())
        {
            const QDomNode dest = destList.at(0);
            const QString icao = dest.firstChildElement("icao_code").text();
            fp.setDestinationAirportIcao(icao);
        }
        const QDomNodeList altList = doc.elementsByTagName("alternate");
        if (!altList.isEmpty())
        {
            const QDomNode alternate = altList.at(0);
            const QString icao = alternate.firstChildElement("icao_code").text();
            fp.setAlternateAirportIcao(icao);
        }
        const QDomNodeList generalList = doc.elementsByTagName("general");
        if (!generalList.isEmpty())
        {
            bool ok;
            const QDomNode general = generalList.at(0);
            QString route = general.firstChildElement("route").text();
            fp.setRoute(route.remove("DCT").simplified().trimmed());
            const QString airline = general.firstChildElement("icao_airline").text();
            const QString flightNumber = general.firstChildElement("flight_number").text();
            fp.setCallsign(CCallsign(airline + flightNumber, CCallsign::Aircraft));
            const QString cruiseAlt = general.firstChildElement("initial_altitude").text();
            const int cruiseAltFt = cruiseAlt.toInt(&ok);
            if (ok)
            {
                CAltitude ca(cruiseAltFt, CAltitude::MeanSeaLevel, CLengthUnit::ft());
                if (cruiseAlt.endsWith("00") && cruiseAltFt > 5000)
                {
                    ca.toFlightLevel();
                }
                fp.setCruiseAltitude(ca);
                if (cruiseAltFt >= 10000) { fp.setFlightRule(CFlightPlan::IFR); } // good guess
            }
            else
            {
                fp.setCruiseAltitudeString(cruiseAlt);
            }
            const QString tas = general.firstChildElement("cruise_tas").text();
            const int tasKts = tas.toInt(&ok);
            if (ok) { fp.setCruiseTrueAirspeed(CSpeed(tasKts, CSpeedUnit::kts())); }
        }

        const QDomNodeList timeList = doc.elementsByTagName("times");
        if (!timeList.isEmpty())
        {
            bool ok;
            const QDomNode times = timeList.at(0);
            const QString enroute = times.firstChildElement("est_time_enroute").text();
            const int enrouteSecs = enroute.toInt(&ok);
            if (ok) { fp.setEnrouteTime(CTime(enrouteSecs, CTimeUnit::s())); }
            const QString endurance = times.firstChildElement("endurance").text();
            const int enduranceSecs = endurance.toInt(&ok);
            if (ok) { fp.setFuelTime(CTime(enduranceSecs, CTimeUnit::s())); }
            const QString depTime = times.firstChildElement("sched_out").text();
            const int depTimeUnixTs = depTime.toInt(&ok);
            if (ok)
            {
                QDateTime depTs = QDateTime::fromSecsSinceEpoch(depTimeUnixTs, QTimeZone::utc());
                depTs.setTimeZone(QTimeZone::utc());
                fp.setTakeoffTimePlanned(depTs);
            }
        }

        const QDomNodeList aircraftList = doc.elementsByTagName("aircraft");
        if (!aircraftList.isEmpty())
        {
            const QDomNode aircraft = aircraftList.at(0);
            const QString equipment = aircraft.firstChildElement("equip").text();
            // H-SDE2E3GHIJ1J3J4J5LM1ORWXY/LB1D1
            const int b = equipment.indexOf('-');
            const int e = equipment.indexOf('/');

            CFlightPlanRemarks r = fp.getFlightPlanRemarks();
            bool remarksChanged = false;
            if (e > b && e >= 0 && b >= 0 && equipment.size() > e)
            {
                const QString icao = equipment.mid(b + 1, e - b - 1);
                r.setIcaoEquipmentCodes(icao);
                remarksChanged = true;
            }

            const QString selcal = aircraft.firstChildElement("selcal").text();
            if (selcal.length() == 4)
            {
                const bool c = r.setSelcalCode(selcal);
                remarksChanged = c || remarksChanged;
            }

            if (remarksChanged)
            {
                fp.setFlightPlanRemarks(r);
            }
        }

        // read FP
        return fp;
    }

    CFlightPlan CFlightPlan::fromMultipleFormats(const QString &data, const QString &fileSuffix)
    {
        if (data.isEmpty()) { return CFlightPlan(); }
        if (fileSuffix.contains("xml", Qt::CaseInsensitive))
        {
            if (data.contains("<OFP>", Qt::CaseInsensitive) && data.contains("<general>", Qt::CaseInsensitive)) { return CFlightPlan::fromSimBriefFormat(data); }
        }

        if (data.contains("[SBFlightPlan]", Qt::CaseInsensitive)) { return CFlightPlan::fromSB4Format(data); }
        if (data.contains("<FlightPlan", Qt::CaseInsensitive) && data.contains("<?xml", Qt::CaseInsensitive)) { return CFlightPlan::fromVPilotFormat(data); }
        return CFlightPlan::fromJson(data);
    }

    CFlightPlan CFlightPlan::fromMultipleFormatsNoThrow(const QString &data, const QString &fileSuffix)
    {
        CFlightPlan fp;
        try
        {
            fp = CFlightPlan::fromMultipleFormats(data, fileSuffix);
        }
        catch (const CJsonException &ex)
        {
            const CStatusMessage m = CStatusMessage::fromJsonException(ex, &fp, QString("Parsing flight plan from failed."));
            Q_UNUSED(m)
        }
        return fp;
    }

    CFlightPlan CFlightPlan::loadFromMultipleFormats(const QString &fileName, CStatusMessageList *msgs)
    {
        try
        {
            QFileInfo fi(fileName);
            if (fileName.isEmpty())
            {
                if (msgs) { msgs->push_back(CStatusMessage(static_cast<CFlightPlan *>(nullptr)).validationError(u"No file name")); }
                return CFlightPlan();
            }
            else
            {
                if (!fi.exists())
                {
                    if (msgs) { msgs->push_back(CStatusMessage(static_cast<CFlightPlan *>(nullptr)).validationError(u"File '%1' does not exist") << fileName); }
                    return CFlightPlan();
                }
            }

            const QString data = CFileUtils::readFileToString(fileName);
            if (data.isEmpty())
            {
                if (msgs) { msgs->push_back(CStatusMessage(static_cast<CFlightPlan *>(nullptr)).validationError(u"File '%1' does not contain data") << fileName); }
                return CFlightPlan();
            }

            if (fileName.endsWith(".sfp", Qt::CaseInsensitive))  { return CFlightPlan::fromSB4Format(data); }
            if (fileName.endsWith(".vfp", Qt::CaseInsensitive))  { return CFlightPlan::fromVPilotFormat(data); }
            if (fileName.endsWith(".json", Qt::CaseInsensitive))
            {
                do
                {
                    CStatusMessage m;
                    if (!Json::looksLikeSwiftJson(data))
                    {
                        m = CStatusMessage(static_cast<CFlightPlan *>(nullptr), CStatusMessage::SeverityWarning, u"Reading '%1' yields no data", true) << fileName;
                        if (msgs) { msgs->push_back(m); }
                        break;
                    }

                    try
                    {
                        const QJsonObject jsonObject = Json::jsonObjectFromString(data);
                        if (Json::looksLikeSwiftTypeValuePairJson(jsonObject))
                        {
                            // CVariant format
                            CVariant variant;
                            variant.convertFromJson(jsonObject);
                            if (variant.canConvert<CFlightPlan>())
                            {
                                const CFlightPlan fp = variant.value<CFlightPlan>();
                                return fp;
                            }
                            else
                            {
                                m = CStatusMessage(static_cast<CFlightPlan *>(nullptr), CStatusMessage::SeverityWarning, u"Wrong format for flight plan in '%1'") << fileName;
                                if (msgs) { msgs->push_back(m); }
                            }
                        }
                        else
                        {
                            const CFlightPlan fp = CFlightPlan::fromJson(jsonObject);
                            return fp;
                        }
                    }
                    catch (const CJsonException &ex)
                    {
                        m = CStatusMessage::fromJsonException(ex, static_cast<CFlightPlan *>(nullptr), "Parse error in " + fileName);
                        if (msgs) { msgs->push_back(m); }
                        break;
                    }
                }
                while (false);
            }

            return CFlightPlan::fromMultipleFormats(data, fi.suffix());
        }
        catch (const CJsonException &ex)
        {
            if (msgs)
            {
                msgs->push_back(CStatusMessage::fromJsonException(ex, static_cast<CFlightPlan *>(nullptr), QStringLiteral("Parsing flight plan from '%1' failed.").arg(fileName)));
            }
        }
        return CFlightPlan();
    }

    const QString &CFlightPlan::flightRulesToString(CFlightPlan::FlightRules rules)
    {
        static const QString v("VFR");
        static const QString i("IFR");
        static const QString s("SVFR");
        static const QString d("DVFR");
        static const QString unknown("???");

        switch (rules)
        {
        case VFR:  return v;
        case IFR:  return i;
        case SVFR: return s;
        case DVFR: return d;
        case UNKNOWN:
        default: break;
        }
        return unknown;
    }

    QString CFlightPlan::aircraftIcaoCodeFromEquipmentCode(const QString &equipmentCodeAndAircraft)
    {
        return splitEquipmentCode(equipmentCodeAndAircraft)[1].trimmed().toUpper();
    }

    QStringList CFlightPlan::splitEquipmentCode(const QString &equipmentCodeAndAircraft)
    {
        static const QStringList empty({"", "", ""});
        if (empty.isEmpty()) { return empty; }
        QStringList firstSplit = equipmentCodeAndAircraft.split('-');
        if (firstSplit.size() >= 2)
        {
            // format like B789/H-SDE1E2E3FGHIJ2J3J4J5M1RWXY/LB1D1
            QString equipment = firstSplit.size() >= 2 ? firstSplit[1] : "";
            QStringList split = firstSplit[0].split('/');
            if (split.size() >= 3)
            {
                return { split[2], split[1], equipment.isEmpty() ? split[0] : equipment }; // "F/B789/H"
            }
            else if (split.size() >= 2)
            {
                if (split[0].size() <= 1) // "H/B789"
                {
                    return { split[0], split[1], equipment };
                }
                else // "B789/H"
                {
                    return { split[1], split[0], equipment };
                }
            }
            else // "B789"
            {
                return { {}, split[0], equipment };
            }
        }
        QStringList split = equipmentCodeAndAircraft.split('/');
        if (split.length() >= 3)
        {
            if (split[1].size() == 1 && CAircraftIcaoCode::isValidDesignator(split[0]))
            {
                using std::swap;
                swap(split[0], split[1]); // "A359/H/L"
            }
            return split; // "H/B738/F"
        }
        if (split.length() == 2)
        {
            if (split[0].length() == 1)
            {
                // we assume prefix + ICAO
                // e.g. "H/B748"
                split.push_back("");
                return split;
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
            s += (s.isEmpty() ? u"" : u"/") % icao;
        }
        if (suffix.isEmpty()) { return s; }
        if (s.isEmpty()) { return suffix; }
        return s % u'/' % suffix;
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

    const QStringList &CFlightPlan::flightRules()
    {
        static const QStringList r({"VFR", "IFR", "SVFR", "DVFR" });
        return r;
    }

    bool CFlightPlan::isVFRRules(CFlightPlan::FlightRules rule)
    {
        return rule == CFlightPlan::VFR || rule == CFlightPlan::DVFR || rule == CFlightPlan::SVFR;
    }

    bool CFlightPlan::isVFRRules(const QString &rule)
    {
        const CFlightPlan::FlightRules r = CFlightPlan::stringToFlightRules(rule);
        return CFlightPlan::isVFRRules(r);
    }

    bool CFlightPlan::isIFRRules(CFlightPlan::FlightRules rule)
    {
        return rule == CFlightPlan::IFR;
    }

    bool CFlightPlan::isIFRRules(const QString &rule)
    {
        const CFlightPlan::FlightRules r = CFlightPlan::stringToFlightRules(rule);
        return CFlightPlan::isIFRRules(r);
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
            QSet<QString> el(CFlightPlan::faaEquipmentCodes().begin(), CFlightPlan::faaEquipmentCodes().end());
            el.unite(QSet<QString>(CFlightPlan::squawkBoxEquipmentCodes().begin(), CFlightPlan::squawkBoxEquipmentCodes().end()));
            return el.values();
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

    CIcons::IconIndex CFlightPlan::toIcon() const
    {
        return CIcons::StandardIconAppFlightPlan16;
    }

} // namespace
