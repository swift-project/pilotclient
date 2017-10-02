/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_FLIGHTPLAN_H
#define BLACKMISC_AVIATION_FLIGHTPLAN_H

#include "airporticaocode.h"
#include "aircrafticaocode.h"
#include "altitude.h"
#include "callsign.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/icon.h"
#include "blackmisc/blackmiscexport.h"

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QTime>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Flight plan remarks, parsed values
        class BLACKMISC_EXPORT CFlightPlanRemarks : public CValueObject<CFlightPlanRemarks>
        {
        public:
            //! Ctor
            CFlightPlanRemarks();

            //! Ctor
            CFlightPlanRemarks(const QString &remarks, bool parse = true);

            //! Ctor
            CFlightPlanRemarks(const QString &remarks, Network::CVoiceCapabilities voiceCapabilities, bool parse = true);

            //! The unparsed remarks
            const QString &getRemarks() const { return m_remarks; }

            //! Radio telephony designator
            const QString &getRadioTelephony() const { return m_radioTelephony; }

            //! Operator, i.e. normally the airline name
            const QString &getFlightOperator() const { return m_flightOperator; }

            //! Airline ICAO if provided in flight plan
            const QString &getAirlineIcao() const { return m_airlineIcao; }

            //! SELCAL code
            const QString &getSelcalCode() const { return m_selcalCode; }

            //! Get SELCAL
            const CCallsign &getCallsign() const { return m_callsign; }

            //! Voice capabilities
            const Network::CVoiceCapabilities &getVoiceCapabilities() const { return m_voiceCapabilities; }

            //! Any remarks available?
            bool hasAnyParsedRemarks() const;

            //! Airline remarks
            bool hasParsedAirlineRemarks() const;

            //! Parse remarks from a flight plan
            void parseFlightPlanRemarks();

            //! Valid airline ICAO?
            //! \remark valid here means valid syntax, no guarantee it really exists
            bool hasValidAirlineIcao() const { return !m_airlineIcao.isEmpty(); }

            //! Not initialized
            bool isNull() const { return m_isNull; }

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

            //! Get aircraft ICAO code from equipment code like
            //! \remark we expect something like H/B772/F B773 B773/F
            static QString aircraftIcaoCodeFromEquipmentCode(const QString &equipmentCodeAndAircraft);

        private:
            QString m_remarks;        //!< the unparsed string
            QString m_radioTelephony; //!< radio telephony designator
            QString m_flightOperator; //!< operator, i.e. normally the airline name
            QString m_airlineIcao;    //!< airline ICAO if provided in flight plan
            QString m_selcalCode;     //!< SELCAL code
            CCallsign m_callsign;     //!< callsign of other pilot
            bool m_isNull = true;     //!< marked as NULL
            Network::CVoiceCapabilities m_voiceCapabilities; //!< voice capabilities

            BLACK_METACLASS(
                CFlightPlanRemarks,
                BLACK_METAMEMBER(radioTelephony),
                BLACK_METAMEMBER(flightOperator),
                BLACK_METAMEMBER(airlineIcao),
                BLACK_METAMEMBER(selcalCode),
                BLACK_METAMEMBER(callsign),
                BLACK_METAMEMBER(isNull),
                BLACK_METAMEMBER(voiceCapabilities)
            );

            //! Cut the remarks part
            static QString cut(const QString &remarks, const QString &marker);
        };

        //! Value object for a flight plan
        class BLACKMISC_EXPORT CFlightPlan :
            public CValueObject<CFlightPlan>,
            public ITimestampBased
        {
        public:
            //! Flight rules (VFR or IFR)
            enum FlightRules
            {
                VFR = 0,    //!< Visual flight rules
                IFR,        //!< Instrument flight rules
                SVFR,       //!< Special VFR (reserved for ATC use),
                DVFR        //!< Defense VFR
            };

            //! Properties by index
            enum ColumnIndex
            {
                IndexCallsign = CPropertyIndex::GlobalIndexCFlightPlan,
                IndexOriginAirportIcao,
                IndexDestinationAirportIcao,
                IndexAlternateAirportIcao,
                IndexRemarks
            };

            static constexpr int MaxRemarksLength = 150; //!< Max remarks length
            static constexpr int MaxRouteLength = 150; //!< Max route length

            //! Default constructor
            CFlightPlan();

            //! Constructor
            CFlightPlan(const CCallsign &callsign,
                        const QString &equipmentIcao, const CAirportIcaoCode &originAirportIcao, const CAirportIcaoCode &destinationAirportIcao, const CAirportIcaoCode &alternateAirportIcao,
                        const QDateTime &takeoffTimePlanned, const QDateTime &takeoffTimeActual, const PhysicalQuantities::CTime &enrouteTime, const PhysicalQuantities::CTime &fuelTime,
                        const CAltitude &cruiseAltitude, const PhysicalQuantities::CSpeed &cruiseTrueAirspeed, FlightRules flightRules, const QString &route, const QString &remarks);

            //! Callsign (of aircraft)
            void setCallsign(const CCallsign &callsign);

            //! Set ICAO aircraft equipment code string (e.g. "T/A320/F")
            void setEquipmentIcao(const QString &equipmentIcao);

            //! Set origin airport ICAO code
            void setOriginAirportIcao(const QString &originAirportIcao) { m_originAirportIcao = originAirportIcao; }

            //! Set origin airport ICAO code
            void setOriginAirportIcao(const CAirportIcaoCode &originAirportIcao) { m_originAirportIcao = originAirportIcao; }

            //! Set destination airport ICAO code
            void setDestinationAirportIcao(const QString &destinationAirportIcao) { m_destinationAirportIcao = destinationAirportIcao; }

            //! Set destination airport ICAO code
            void setDestinationAirportIcao(const CAirportIcaoCode &destinationAirportIcao) { m_destinationAirportIcao = destinationAirportIcao; }

            //! Set alternate destination airport ICAO code
            void setAlternateAirportIcao(const QString &alternateAirportIcao) { m_alternateAirportIcao = alternateAirportIcao; }

            //! Set alternate destination airport ICAO code
            void setAlternateAirportIcao(const CAirportIcaoCode &alternateAirportIcao) { m_alternateAirportIcao = alternateAirportIcao; }

            //! Set planned takeoff time
            void setTakeoffTimePlanned(const QDateTime &takeoffTimePlanned) { m_takeoffTimePlanned = takeoffTimePlanned; }

            //! Set planned takeoff time hh:mm
            void setTakeoffTimePlanned(const QString &time) { m_takeoffTimePlanned = QDateTime::currentDateTimeUtc(); m_takeoffTimePlanned.setTime(QTime::fromString(time, "hh:mm"));}

            //! Set actual takeoff time (reserved for ATC use)
            void setTakeoffTimeActual(const QDateTime &takeoffTimeActual) { m_takeoffTimeActual = takeoffTimeActual; }

            //! Set actual takeoff time hh:mm
            void setTakeoffTimeActual(const QString &time) { m_takeoffTimeActual = QDateTime::currentDateTimeUtc(); m_takeoffTimeActual.setTime(QTime::fromString(time, "hh:mm"));}

            //! Set planned enroute flight time
            void setEnrouteTime(const PhysicalQuantities::CTime &enrouteTime) { m_enrouteTime = enrouteTime; m_enrouteTime.switchUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin());}

            //! Set amount of fuel load in time
            void setFuelTime(const PhysicalQuantities::CTime &fuelTime) { m_fuelTime = fuelTime; m_fuelTime.switchUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin());}

            //! Set amount of fuel load in time hh:mm
            void setFuelTime(const QString &fuelTime) { m_fuelTime = PhysicalQuantities::CTime(fuelTime); }

            //! Set planned cruise altitude
            void setCruiseAltitude(const CAltitude &cruiseAltitude) { m_cruiseAltitude = cruiseAltitude; }

            //! Set planned cruise TAS
            void setCruiseTrueAirspeed(const PhysicalQuantities::CSpeed &cruiseTrueAirspeed) { m_cruiseTrueAirspeed = cruiseTrueAirspeed; }

            //! Set flight rules (VFR or IFR)
            void setFlightRule(FlightRules flightRules) { m_flightRules = flightRules; }

            //! Set route string
            void setRoute(const QString &route) { m_route = route.trimmed().left(MaxRouteLength).toUpper(); }

            //! Set remarks string (max 100 characters)
            void setRemarks(const QString &remarks);

            //! When last sent
            void setWhenLastSentOrLoaded(const QDateTime &dateTime) { this->setUtcTimestamp(dateTime); }

            //! Aircraft callsign
            const CCallsign &getCallsign() const { return m_callsign; }

            //! Has callsign?
            bool hasCallsign() const { return !m_callsign.isEmpty(); }

            //! Get ICAO aircraft equipment code string
            const QString &getEquipmentIcao() const { return m_equipmentIcao; }

            //! Get origin airport ICAO code
            const CAirportIcaoCode &getOriginAirportIcao() const { return m_originAirportIcao; }

            //! Get destination airport ICAO code
            const CAirportIcaoCode &getDestinationAirportIcao() const { return m_destinationAirportIcao; }

            //! Get alternate destination airport ICAO code
            const CAirportIcaoCode &getAlternateAirportIcao() const { return m_alternateAirportIcao; }

            //! Get planned takeoff time (planned)
            const QDateTime &getTakeoffTimePlanned() const { return m_takeoffTimePlanned; }

            //! Get planned takeoff time (planned)
            QString getTakeoffTimePlannedHourMin() const { return m_takeoffTimePlanned.toString("hh:mm"); }

            //! Get actual takeoff time (actual)
            const QDateTime &getTakeoffTimeActual() const { return m_takeoffTimeActual; }

            //! Get actual takeoff time (actual)
            QString getTakeoffTimeActualHourMin() const { return m_takeoffTimeActual.toString("hh:mm"); }

            //! Get planned enroute flight time
            const PhysicalQuantities::CTime &getEnrouteTime() const { return m_enrouteTime; }

            //! Get planned enroute flight time
            QString getEnrouteTimeHourMin() const { return m_enrouteTime.valueRoundedWithUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin()); }

            //! Get amount of fuel load in time
            const PhysicalQuantities::CTime &getFuelTime() const { return m_fuelTime; }

            //! Get amount of fuel load in time
            QString getFuelTimeHourMin() const { return m_fuelTime.valueRoundedWithUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin()); }

            //! Cruising altitudes
            const BlackMisc::Aviation::CAltitude &getCruiseAltitude() const { return m_cruiseAltitude; }

            //! Get planned cruise TAS
            const PhysicalQuantities::CSpeed &getCruiseTrueAirspeed() const { return m_cruiseTrueAirspeed; }

            //! Get flight rules (VFR or IFR)
            FlightRules getFlightRules() const { return m_flightRules; }

            //! Get route string
            const QString &getRoute() const { return m_route; }

            //! When last sent
            const QDateTime whenLastSentOrLoaded() const { return this->getUtcTimestamp() ; }

            //! Flight plan already sent
            bool wasSentOrLoaded() const { return this->hasValidTimestamp(); }

            //! Received before n ms
            qint64 timeDiffSentOrLoadedMs() const { return this->msecsToNow(); }

            //! Get remarks string
            const QString &getRemarks() const { return m_remarks.getRemarks(); }

            //! Get the parsable remarks
            const CFlightPlanRemarks &getFlightPlanRemarks() const { return m_remarks; }

            //! Get aircraft ICAO, derived from equipment ICAO as in getEquipmentIcao()
            const CAircraftIcaoCode &getAircraftIcao() const { return m_aircraftIcao; }

            //! Has aircraft ICAO?
            bool hasAircraftIcao() const { return m_aircraftIcao.hasDesignator(); }

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! \copydoc BlackMisc::Mixin::Icon::toIcon
            CIcon toIcon() const;

            //! \copydoc BlackMisc::Mixin::String::toQString()
            QString convertToQString(bool i18n = false) const;

            //! Rules to string
            static const QString flightRuleToString(FlightRules rule);

        private:
            CCallsign m_callsign;
            CFlightPlanRemarks m_remarks;
            QString m_equipmentIcao; //!< e.g. "T/A320/F"
            CAircraftIcaoCode m_aircraftIcao; //!< Aircraft ICAO code derived from equipment ICAO
            CAirportIcaoCode m_originAirportIcao;
            CAirportIcaoCode m_destinationAirportIcao;
            CAirportIcaoCode m_alternateAirportIcao;
            QDateTime m_takeoffTimePlanned;
            QDateTime m_takeoffTimeActual;
            PhysicalQuantities::CTime m_enrouteTime;
            PhysicalQuantities::CTime m_fuelTime;
            CAltitude m_cruiseAltitude;
            PhysicalQuantities::CSpeed m_cruiseTrueAirspeed;
            FlightRules m_flightRules;
            QString m_route;

            BLACK_METACLASS(
                CFlightPlan,
                BLACK_METAMEMBER(equipmentIcao),
                BLACK_METAMEMBER(originAirportIcao),
                BLACK_METAMEMBER(destinationAirportIcao),
                BLACK_METAMEMBER(alternateAirportIcao),
                BLACK_METAMEMBER(takeoffTimePlanned),
                BLACK_METAMEMBER(takeoffTimeActual),
                BLACK_METAMEMBER(enrouteTime),
                BLACK_METAMEMBER(fuelTime),
                BLACK_METAMEMBER(cruiseAltitude),
                BLACK_METAMEMBER(cruiseTrueAirspeed),
                BLACK_METAMEMBER(flightRules),
                BLACK_METAMEMBER(route),
                BLACK_METAMEMBER(remarks),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CFlightPlanRemarks)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CFlightPlan)

#endif // guard
