/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_AVFLIGHTPLAN_H
#define BLACKMISC_AVFLIGHTPLAN_H

#include "valueobject.h"
#include "avaltitude.h"
#include "pqtime.h"
#include "pqspeed.h"
#include <QDateTime>

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object for a flight plan
         */
        class CFlightPlan : public BlackMisc::CValueObject
        {
        public:
            /*!
             * Flight rules (VFR or IFR)
             */
            enum FlightRules
            {
                VFR = 0,    //!< Visual flight rules
                IFR,        //!< Instrument flight rules
                SVFR        //!< Special VFR (reserved for ATC use)
            };

            /*!
             * Default constructor
             */
            CFlightPlan() {}

            /*!
             * Constructor
             */
            CFlightPlan(const QString &equipmentIcao, const QString &originAirportIcao, const QString &destinationAirportIcao, const QString &alternateAirportIcao,
                QDateTime takeoffTimePlanned, QDateTime takeoffTimeActual, const PhysicalQuantities::CTime &enrouteTime, const PhysicalQuantities::CTime &fuelTime,
                const CAltitude &cruiseAltitude, const PhysicalQuantities::CSpeed &cruiseTrueAirspeed, FlightRules flightRules, const QString &route, const QString &remarks)
            : m_equipmentIcao(equipmentIcao), m_originAirportIcao(originAirportIcao), m_destinationAirportIcao(destinationAirportIcao), m_alternateAirportIcao(alternateAirportIcao),
              m_takeoffTimePlanned(takeoffTimePlanned), m_takeoffTimeActual(takeoffTimeActual), m_enrouteTime(enrouteTime), m_fuelTime(fuelTime),
              m_cruiseAltitude(cruiseAltitude), m_cruiseTrueAirspeed(cruiseTrueAirspeed), m_flightRules(flightRules), m_route(route), m_remarks(remarks.left(100))
            {}

            //! Set ICAO aircraft equipment code string (e.g. "T/A320/F")
            void setEquipmentIcao(const QString &equipmentIcao) { m_equipmentIcao = equipmentIcao; }

            //! Set origin airport ICAO code
            void setOriginAirportIcao(const QString &originAirportIcao) { m_originAirportIcao = originAirportIcao; }

            //! Set destination airport ICAO code
            void setDestinationAirportIcao(const QString &destinationAirportIcao) { m_destinationAirportIcao = destinationAirportIcao; }

            //! Set alternate destination airport ICAO code
            void setAlternateAirportIcao(const QString &alternateAirportIcao) { m_alternateAirportIcao = alternateAirportIcao; }

            //! Set planned takeoff time
            void setTakeoffTimePlanned(QDateTime takeoffTimePlanned) { m_takeoffTimePlanned = takeoffTimePlanned; }

            //! Set actual takeoff time (reserved for ATC use)
            void setTakeoffTimeActual(QDateTime takeoffTimeActual) { m_takeoffTimeActual = takeoffTimeActual; }

            //! Set planned enroute flight time
            void setEnrouteTime(const PhysicalQuantities::CTime &enrouteTime) { m_enrouteTime = enrouteTime; }

            //! Set amount of fuel load in time
            void setFuelTime(const PhysicalQuantities::CTime &fuelTime) { m_fuelTime = fuelTime; }

            //! Set planned cruise altitude
            void setCruiseAltitude(const CAltitude &cruiseAltitude) { m_cruiseAltitude = cruiseAltitude; }

            //! Set planned cruise TAS
            void setCruiseTrueAirspeed(const PhysicalQuantities::CSpeed &cruiseTrueAirspeed) { m_cruiseTrueAirspeed = cruiseTrueAirspeed; }

            //! Set flight rules (VFR or IFR)
            void setFlightRules(FlightRules flightRules) { m_flightRules = flightRules; }

            //! Set route string
            void setRoute(const QString &route) { m_route = route; }

            //! Set remarks string (max 100 characters)
            void setRemarks(const QString &remarks) { m_remarks = remarks.left(100); }

            //! Get ICAO aircraft equipment code string
            const QString &getEquipmentIcao() const { return m_equipmentIcao; }

            //! Get origin airport ICAO code
            const QString &getOriginAirportIcao() const { return m_originAirportIcao; }

            //! Get destination airport ICAO code
            const QString &getDestinationAirportIcao() const { return m_destinationAirportIcao; }

            //! Get alternate destination airport ICAO code
            const QString &getAlternateAirportIcao() const { return m_alternateAirportIcao; }

            //! Get planned takeoff time
            QDateTime getTakeoffTimePlanned() const { return m_takeoffTimePlanned; }

            //! Get actual takeoff time
            QDateTime getTakeoffTimeActual() const { return m_takeoffTimeActual; }

            //! Get planned enroute flight time
            const PhysicalQuantities::CTime &getEnrouteTime() const { return m_enrouteTime; }

            //! Get amount of fuel load in time
            const PhysicalQuantities::CTime &getFuelTime() const { return m_fuelTime; }

            //! Get planned cruise altitude
            const CAltitude &getCruiseAltitude() const { return m_cruiseAltitude; }

            //! Get planned cruise TAS
            const PhysicalQuantities::CSpeed &getCruiseTrueAirspeed() const { return m_cruiseTrueAirspeed; }

            //! Get flight rules (VFR or IFR)
            FlightRules getFlightRules() const { return m_flightRules; }

            //! Get route string
            const QString &getRoute() const { return m_route; }

            //! Get remarks string
            const QString &getRemarks() const { return m_remarks; }

            //! Equals operator
            bool operator ==(const CFlightPlan &other) const;

            //! Not equals operator
            bool operator !=(const CFlightPlan &other) const;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! Register metadata
            static void registerMetadata();

        protected:
            //! \copydoc CValueObject::convertToQString()
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::marshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CFlightPlan)
            QString m_equipmentIcao;
            QString m_originAirportIcao;
            QString m_destinationAirportIcao;
            QString m_alternateAirportIcao;
            QDateTime m_takeoffTimePlanned;
            QDateTime m_takeoffTimeActual;
            PhysicalQuantities::CTime m_enrouteTime;
            PhysicalQuantities::CTime m_fuelTime;
            CAltitude m_cruiseAltitude;
            PhysicalQuantities::CSpeed m_cruiseTrueAirspeed;
            FlightRules m_flightRules;
            QString m_route;
            QString m_remarks;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CFlightPlan)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CFlightPlan, (o.m_equipmentIcao, o.m_originAirportIcao, o.m_destinationAirportIcao, o.m_alternateAirportIcao,
    o.m_takeoffTimePlanned, o.m_takeoffTimeActual, o.m_enrouteTime, o.m_fuelTime, o.m_cruiseAltitude/*, tie(o.m_cruiseTrueAirspeed, o.m_flightRules, o.m_route, o.m_remarks)*/))

#endif // guard
