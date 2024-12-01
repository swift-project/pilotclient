// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_FLIGHTPLAN_H
#define SWIFT_MISC_AVIATION_FLIGHTPLAN_H

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QTime>
#include <QtGlobal>

#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/airporticaocode.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/flightplanaircraftinfo.h"
#include "misc/aviation/selcal.h"
#include "misc/logcategories.h"
#include "misc/metaclass.h"
#include "misc/network/url.h"
#include "misc/network/voicecapabilities.h"
#include "misc/pq/speed.h"
#include "misc/pq/time.h"
#include "misc/pq/units.h"
#include "misc/statusmessagelist.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::Aviation, CFlightPlanRemarks)
SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::Aviation, CFlightPlan)

namespace swift::misc::aviation
{
    //! Flight plan remarks, parsed values
    //! \remark Actually the term "remarks" is not accurate, as the FP remarks are normally only the /RMK part of a
    //! flight plan.
    //!         But on flight sim. networks "remarks" is used to fill in all parts not fitting in other fields.
    //!         The correct term would be ITEM18 ("OTHER INFORMATION") or ITEM19 ("SUPPLEMENTARY INFORMATION")
    //!         according to https://www.skybrary.aero/index.php/Flight_Plan_Filling
    class SWIFT_MISC_EXPORT CFlightPlanRemarks : public CValueObject<CFlightPlanRemarks>
    {
    public:
        //! Ctor
        CFlightPlanRemarks();

        //! Ctor
        CFlightPlanRemarks(const QString &remarks, bool parse = true);

        //! Ctor
        CFlightPlanRemarks(const QString &remarks, network::CVoiceCapabilities voiceCapabilities, bool parse = true);

        //! The unparsed remarks
        const QString &getRemarks() const { return m_remarks; }

        //! Radio telephony designator
        const QString &getRadioTelephony() const { return m_radioTelephony; }

        //! Operator, i.e. normally the airline name
        const QString &getFlightOperator() const { return m_flightOperator; }

        //! Airline ICAO if provided in flight plan
        const CAirlineIcaoCode &getAirlineIcao() const { return m_airlineIcao; }

        //! SELCAL code
        const CSelcal &getSelcalCode() const { return m_selcalCode; }

        //! SELCAL code
        bool setSelcalCode(const QString &selcal);

        //! Get registration (a callsign, but normally not the flight callsign)
        const CCallsign &getRegistration() const { return m_registration; }

        //! Voice capabilities
        const network::CVoiceCapabilities &getVoiceCapabilities() const { return m_voiceCapabilities; }

        //! Set voice capabilities
        void setVoiceCapabilitiesByText(const QString &text);

        //! Set voice capabilities
        void setVoiceCapabilities(const network::CVoiceCapabilities &capabilities);

        //! Any remarks available?
        bool hasAnyParsedRemarks() const;

        //! Airline remarks
        bool hasParsedAirlineRemarks() const;

        //! Parse remarks from a flight plan
        void parseFlightPlanRemarks(bool force = false);

        //! Valid airline ICAO?
        //! \remark valid here means valid syntax, no guarantee it really exists
        bool hasValidAirlineIcao() const { return m_airlineIcao.hasValidDesignator(); }

        //! Empty remarks?
        bool isEmpty() const { return m_remarks.isEmpty(); }

        //! Already parsed?
        bool isParsed() const { return m_isParsed; }

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! Turn text into voice capabilities for remarks
        static QString textToVoiceCapabilitiesRemarks(const QString &text);

        //! Replace the voice capabilities remarks part
        static QString replaceVoiceCapabilities(const QString &newCapRemarks, const QString &oldRemarks);

        //! Clean up remarks string
        static QString cleanRemarks(const QString &remarksIn);

    private:
        QString m_remarks; //!< the unparsed string
        QString m_radioTelephony; //!< radio telephony designator
        QString m_flightOperator; //!< operator, i.e. normally the airline name
        CCallsign m_registration; //!< callsign of pilot
        CSelcal m_selcalCode; //!< SELCAL code
        CAirlineIcaoCode m_airlineIcao; //!< airline ICAO if provided in flight plan
        network::CVoiceCapabilities m_voiceCapabilities; //!< voice capabilities
        bool m_isParsed = false; //!< marked as parsed

        SWIFT_METACLASS(
            CFlightPlanRemarks,
            SWIFT_METAMEMBER(remarks, 0, DisabledForComparison),
            SWIFT_METAMEMBER(radioTelephony),
            SWIFT_METAMEMBER(flightOperator),
            SWIFT_METAMEMBER(airlineIcao),
            SWIFT_METAMEMBER(selcalCode),
            SWIFT_METAMEMBER(registration),
            SWIFT_METAMEMBER(isParsed),
            SWIFT_METAMEMBER(voiceCapabilities));

        //! Cut the remarks part
        static QString getRemark(const QString &remarks, const QString &marker);

        //! Replace a remark part
        static QString replaceRemark(const QString &remarks, const QString &marker, const QString &newRemark);
    };

    //! Value object for a flight plan
    class SWIFT_MISC_EXPORT CFlightPlan : public CValueObject<CFlightPlan>, public ITimestampBased
    {
    public:
        //! The log. catgeories
        static const QStringList &getLogCategories();

        //! Flight rules (VFR or IFR)
        enum FlightRules
        {
            VFR = 0, //!< Visual flight rules
            IFR, //!< Instrument flight rules
            SVFR, //!< Special VFR (reserved for ATC use),
            DVFR, //!< Defense VFR
            UNKNOWN //!< Unknown
        };

        //! Properties by index
        enum ColumnIndex
        {
            IndexCallsign = CPropertyIndexRef::GlobalIndexCFlightPlan,
            IndexOriginAirportIcao,
            IndexDestinationAirportIcao,
            IndexAlternateAirportIcao,
            IndexRemarks
        };

        //! \fixme max.length of complete flight plan is 768 characters, this here is an assumption and should be part
        //! of the underlying network layers
        //  https://forums.vatsim.net/viewtopic.php?f=6&t=63416
        static constexpr int MaxRemarksLength = 512; //!< Max.remarks length
        static constexpr int MaxRouteLength = 512; //!< Max.route length
        static constexpr int MaxRouteAndRemarksLength = 624; //!< Max.length for Route and Remarks

        //! Default constructor
        CFlightPlan() = default;

        //! Constructor
        CFlightPlan(const CCallsign &callsign, const CFlightPlanAircraftInfo &aircraftInfo,
                    const CAirportIcaoCode &originAirportIcao, const CAirportIcaoCode &destinationAirportIcao,
                    const CAirportIcaoCode &alternateAirportIcao, const QDateTime &takeoffTimePlanned,
                    const QDateTime &takeoffTimeActual, const physical_quantities::CTime &enrouteTime,
                    const physical_quantities::CTime &fuelTime, const CAltitude &cruiseAltitude,
                    const physical_quantities::CSpeed &cruiseTrueAirspeed, FlightRules flightRules,
                    const QString &route, const QString &remarks);

        //! Callsign (of aircraft)
        void setCallsign(const CCallsign &callsign);

        //! Set information about the aircraft used in this flightplan
        void setAircraftInfo(const CFlightPlanAircraftInfo &aircraftInfo);

        //! Set origin airport ICAO code
        void setOriginAirportIcao(const QString &originAirportIcao) { m_originAirportIcao = originAirportIcao; }

        //! Set origin airport ICAO code
        void setOriginAirportIcao(const CAirportIcaoCode &originAirportIcao)
        {
            m_originAirportIcao = originAirportIcao;
        }

        //! Set destination airport ICAO code
        void setDestinationAirportIcao(const QString &destinationAirportIcao)
        {
            m_destinationAirportIcao = destinationAirportIcao;
        }

        //! Set destination airport ICAO code
        void setDestinationAirportIcao(const CAirportIcaoCode &destinationAirportIcao)
        {
            m_destinationAirportIcao = destinationAirportIcao;
        }

        //! Set alternate destination airport ICAO code
        void setAlternateAirportIcao(const QString &alternateAirportIcao)
        {
            m_alternateAirportIcao = alternateAirportIcao;
        }

        //! Set alternate destination airport ICAO code
        void setAlternateAirportIcao(const CAirportIcaoCode &alternateAirportIcao)
        {
            m_alternateAirportIcao = alternateAirportIcao;
        }

        //! Set planned takeoff time
        void setTakeoffTimePlanned(const QDateTime &takeoffTimePlanned);

        //! Set planned takeoff time hh:mm
        void setTakeoffTimePlanned(const QString &time)
        {
            m_takeoffTimePlanned = QDateTime::currentDateTimeUtc();
            m_takeoffTimePlanned.setTime(QTime::fromString(time, "hh:mm"));
        }

        //! Set actual takeoff time (reserved for ATC use)
        void setTakeoffTimeActual(const QDateTime &takeoffTimeActual);

        //! Set actual takeoff time hh:mm
        void setTakeoffTimeActual(const QString &time)
        {
            m_takeoffTimeActual = QDateTime::currentDateTimeUtc();
            m_takeoffTimeActual.setTime(QTime::fromString(time, "hh:mm"));
        }

        //! Set planned enroute flight time
        void setEnrouteTime(const physical_quantities::CTime &enrouteTime)
        {
            m_enrouteTime = enrouteTime;
            m_enrouteTime.switchUnit(swift::misc::physical_quantities::CTimeUnit::hrmin());
        }

        //! Set amount of fuel load in time
        void setFuelTime(const physical_quantities::CTime &fuelTime)
        {
            m_fuelTime = fuelTime;
            m_fuelTime.switchUnit(swift::misc::physical_quantities::CTimeUnit::hrmin());
        }

        //! Set amount of fuel load in time hh:mm
        void setFuelTime(const QString &fuelTime) { m_fuelTime = physical_quantities::CTime(fuelTime); }

        //! Set planned cruise altitude
        void setCruiseAltitude(const CAltitude &cruiseAltitude)
        {
            m_cruiseAltitude = cruiseAltitude;
            m_cruiseAltitudeString.clear();
        }

        //! Cruising altitude already as string
        void setCruiseAltitudeString(const QString &altitudeString) { m_cruiseAltitudeString = altitudeString; }

        //! Set planned cruise TAS
        void setCruiseTrueAirspeed(const physical_quantities::CSpeed &cruiseTrueAirspeed)
        {
            m_cruiseTrueAirspeed = cruiseTrueAirspeed;
        }

        //! Set flight rules (VFR or IFR)
        void setFlightRule(FlightRules flightRule) { m_flightRules = flightRule; }

        //! Set flight rules (VFR or IFR)
        void setFlightRule(const QString &flightRule);

        //! Set route string
        void setRoute(const QString &route);

        //! Set remarks string (max 100 characters)
        void setRemarks(const QString &remarks);

        //! Set voice capabilities
        void setVoiceCapabilities(const QString &capabilities);

        //! When last sent
        void setWhenLastSentOrLoaded(const QDateTime &dateTime) { this->setUtcTimestamp(dateTime); }

        //! Aircraft callsign
        const CCallsign &getCallsign() const { return m_callsign; }

        //! Has callsign?
        bool hasCallsign() const { return !m_callsign.isEmpty(); }

        //! Get origin airport ICAO code
        const CAirportIcaoCode &getOriginAirportIcao() const { return m_originAirportIcao; }

        //! Get destination airport ICAO code
        const CAirportIcaoCode &getDestinationAirportIcao() const { return m_destinationAirportIcao; }

        //! Get alternate destination airport ICAO code
        const CAirportIcaoCode &getAlternateAirportIcao() const { return m_alternateAirportIcao; }

        //! Get planned takeoff time (planned)
        const QDateTime &getTakeoffTimePlanned() const { return m_takeoffTimePlanned; }

        //! Get planned takeoff time (planned)
        QString getTakeoffTimePlannedHourMin() const;

        //! Get actual takeoff time (actual)
        const QDateTime &getTakeoffTimeActual() const { return m_takeoffTimeActual; }

        //! Get actual takeoff time (actual)
        QString getTakeoffTimeActualHourMin() const;

        //! Get planned enroute flight time
        const physical_quantities::CTime &getEnrouteTime() const { return m_enrouteTime; }

        //! Get planned enroute flight time
        QString getEnrouteTimeHourMin() const
        {
            return m_enrouteTime.valueRoundedWithUnit(physical_quantities::CTimeUnit::hrmin());
        }

        //! Get amount of fuel load in time
        const physical_quantities::CTime &getFuelTime() const { return m_fuelTime; }

        //! Get amount of fuel load in time
        QString getFuelTimeHourMin() const
        {
            return m_fuelTime.valueRoundedWithUnit(physical_quantities::CTimeUnit::hrmin());
        }

        //! Cruising altitudes
        const CAltitude &getCruiseAltitude() const { return m_cruiseAltitude; }

        //! Cruising altitude already as string
        const QString &getCruiseAltitudeString() const { return m_cruiseAltitudeString; }

        //! Get planned cruise TAS
        const physical_quantities::CSpeed &getCruiseTrueAirspeed() const { return m_cruiseTrueAirspeed; }

        //! Get flight rules as in FlightRules
        FlightRules getFlightRules() const { return m_flightRules; }

        //! Get flight rules as in FlightRules as string
        QString getFlightRulesAsString() const { return CFlightPlan::flightRulesToString(this->getFlightRules()); }

        //! Get route string
        const QString &getRoute() const { return m_route; }

        //! When last sent
        const QDateTime whenLastSentOrLoaded() const { return this->getUtcTimestamp(); }

        //! Flight plan already sent
        bool wasSentOrLoaded() const { return this->hasValidTimestamp(); }

        //! Received before n ms
        qint64 timeDiffSentOrLoadedMs() const { return this->msecsToNow(); }

        //! Get remarks string
        const QString &getRemarks() const { return m_remarks.getRemarks(); }

        //! Get the parsable remarks
        const CFlightPlanRemarks &getFlightPlanRemarks() const { return m_remarks; }

        //! Set FP remarks
        void setFlightPlanRemarks(const CFlightPlanRemarks &remarks) { m_remarks = remarks; }

        //! Get ICAO aircraft NAV/COM equipment
        CFlightPlanAircraftInfo getAircraftInfo() const { return m_aircraftInfo; }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! \copydoc swift::misc::mixin::Icon::toIcon
        CIcons::IconIndex toIcon() const;

        //! \copydoc swift::misc::mixin::String::toQString()
        QString convertToQString(bool i18n = false) const;

        //! As HTML
        QString asHTML(bool i18n = false) const;

        //! From SB4 data
        static CFlightPlan fromSB4Format(const QString &sbData);

        //! From SimBrief format (XML)
        static CFlightPlan fromSimBriefFormat(const QString &simBrief);

        //! From multiple formats
        static CFlightPlan fromMultipleFormats(const QString &data, const QString &fileSuffix);

        //! Load from multiple formats
        static CFlightPlan loadFromMultipleFormats(const QString &fileName, CStatusMessageList *msgs = nullptr);

        //! Rules to string
        static const QString &flightRulesToString(FlightRules rules);

        //! String to flight rules
        static FlightRules stringToFlightRules(const QString &flightRules);

        //! All rules as string
        static const QStringList &flightRules();

        //! @{
        //! Is rule a VFR rule?
        static bool isVFRRules(FlightRules rule);
        static bool isVFRRules(const QString &rule);
        //! @}

        //! @{
        //! Is rule a IFR rule?
        static bool isIFRRules(FlightRules rule);
        static bool isIFRRules(const QString &rule);
        //! @}

    private:
        CCallsign m_callsign; //!< aircraft callsign
        CFlightPlanAircraftInfo m_aircraftInfo; //!< Aircraft information
        CAirportIcaoCode m_originAirportIcao;
        CAirportIcaoCode m_destinationAirportIcao;
        CAirportIcaoCode m_alternateAirportIcao;
        QDateTime m_takeoffTimePlanned;
        QDateTime m_takeoffTimeActual;
        physical_quantities::CTime m_enrouteTime;
        physical_quantities::CTime m_fuelTime;
        CAltitude m_cruiseAltitude;
        QString m_cruiseAltitudeString;
        physical_quantities::CSpeed m_cruiseTrueAirspeed;
        FlightRules m_flightRules {};
        QString m_route;
        CFlightPlanRemarks m_remarks;

        //! As string
        QString buildString(bool i18n = false, const QString &separator = " ") const;

        SWIFT_METACLASS(
            CFlightPlan,
            // callsign will be current flight
            SWIFT_METAMEMBER(aircraftInfo),
            SWIFT_METAMEMBER(originAirportIcao),
            SWIFT_METAMEMBER(destinationAirportIcao),
            SWIFT_METAMEMBER(alternateAirportIcao),
            SWIFT_METAMEMBER(takeoffTimePlanned),
            SWIFT_METAMEMBER(takeoffTimeActual),
            SWIFT_METAMEMBER(enrouteTime, 0, LosslessMarshalling),
            SWIFT_METAMEMBER(fuelTime, 0, LosslessMarshalling),
            SWIFT_METAMEMBER(cruiseAltitude, 0, LosslessMarshalling),
            SWIFT_METAMEMBER(cruiseAltitudeString),
            SWIFT_METAMEMBER(cruiseTrueAirspeed, 0, LosslessMarshalling),
            SWIFT_METAMEMBER(flightRules),
            SWIFT_METAMEMBER(route),
            SWIFT_METAMEMBER(remarks),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch));
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CFlightPlanRemarks)
Q_DECLARE_METATYPE(swift::misc::aviation::CFlightPlan)

#endif // SWIFT_MISC_AVIATION_FLIGHTPLAN_H
