/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVFLIGHTPLAN_H
#define BLACKMISC_AVFLIGHTPLAN_H

#include "valueobject.h"
#include "avaltitude.h"
#include "avairporticao.h"
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

            static const int MaxRemarksLength = 150; //!< Max remarks length
            static const int MaxRouteLength = 150; //!< Max route length

            //! Default constructor
            CFlightPlan() = default;

            //! Constructor
            CFlightPlan(const QString &equipmentIcao, const CAirportIcao &originAirportIcao, const CAirportIcao &destinationAirportIcao, const CAirportIcao &alternateAirportIcao,
                        QDateTime takeoffTimePlanned, QDateTime takeoffTimeActual, const PhysicalQuantities::CTime &enrouteTime, const PhysicalQuantities::CTime &fuelTime,
                        const CAltitude &cruiseAltitude, const PhysicalQuantities::CSpeed &cruiseTrueAirspeed, FlightRules flightRules, const QString &route, const QString &remarks)
                : m_equipmentIcao(equipmentIcao), m_originAirportIcao(originAirportIcao), m_destinationAirportIcao(destinationAirportIcao), m_alternateAirportIcao(alternateAirportIcao),
                  m_takeoffTimePlanned(takeoffTimePlanned), m_takeoffTimeActual(takeoffTimeActual), m_enrouteTime(enrouteTime), m_fuelTime(fuelTime),
                  m_cruiseAltitude(cruiseAltitude), m_cruiseTrueAirspeed(cruiseTrueAirspeed), m_flightRules(flightRules),
                  m_route(route.trimmed().left(MaxRouteLength).toUpper()), m_remarks(remarks.trimmed().left(MaxRemarksLength).toUpper())
            {
                m_enrouteTime.switchUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin());
                m_fuelTime.switchUnit(BlackMisc::PhysicalQuantities::CTimeUnit::hrmin());
            }

            //! Set ICAO aircraft equipment code string (e.g. "T/A320/F")
            void setEquipmentIcao(const QString &equipmentIcao) { m_equipmentIcao = equipmentIcao; }

            //! Set origin airport ICAO code
            void setOriginAirportIcao(const QString &originAirportIcao) { m_originAirportIcao = originAirportIcao; }

            //! Set origin airport ICAO code
            void setOriginAirportIcao(const CAirportIcao &originAirportIcao) { m_originAirportIcao = originAirportIcao; }

            //! Set destination airport ICAO code
            void setDestinationAirportIcao(const QString &destinationAirportIcao) { m_destinationAirportIcao = destinationAirportIcao; }

            //! Set destination airport ICAO code
            void setDestinationAirportIcao(const CAirportIcao &destinationAirportIcao) { m_destinationAirportIcao = destinationAirportIcao; }

            //! Set alternate destination airport ICAO code
            void setAlternateAirportIcao(const QString &alternateAirportIcao) { m_alternateAirportIcao = alternateAirportIcao; }

            //! Set alternate destination airport ICAO code
            void setAlternateAirportIcao(const CAirportIcao &alternateAirportIcao) { m_alternateAirportIcao = alternateAirportIcao; }

            //! Set planned takeoff time
            void setTakeoffTimePlanned(QDateTime takeoffTimePlanned) { m_takeoffTimePlanned = takeoffTimePlanned; }

            //! Set planned takeoff time hh:mm
            void setTakeoffTimePlanned(QString time) { m_takeoffTimePlanned = QDateTime::currentDateTimeUtc(); m_takeoffTimePlanned.setTime(QTime::fromString(time, "hh:mm"));}

            //! Set actual takeoff time (reserved for ATC use)
            void setTakeoffTimeActual(QDateTime takeoffTimeActual) { m_takeoffTimeActual = takeoffTimeActual; }

            //! Set actual takeoff time hh:mm
            void setTakeoffTimeActual(QString time) { m_takeoffTimeActual = QDateTime::currentDateTimeUtc(); m_takeoffTimeActual.setTime(QTime::fromString(time, "hh:mm"));}

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
            void setRemarks(const QString &remarks) { m_remarks = remarks.trimmed().left(MaxRemarksLength).toUpper(); }

            //! When last sent
            void setWhenLastSentOrLoaded(const QDateTime &dateTime) { m_lastSentOrLoaded = dateTime; }

            //! Get ICAO aircraft equipment code string
            const QString &getEquipmentIcao() const { return m_equipmentIcao; }

            //! Get origin airport ICAO code
            const CAirportIcao &getOriginAirportIcao() const { return m_originAirportIcao; }

            //! Get destination airport ICAO code
            const CAirportIcao &getDestinationAirportIcao() const { return m_destinationAirportIcao; }

            //! Get alternate destination airport ICAO code
            const CAirportIcao &getAlternateAirportIcao() const { return m_alternateAirportIcao; }

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
            const QDateTime &whenLastSentOrLoaded() const { return m_lastSentOrLoaded; }

            //! Flight plan already sent
            bool wasSentOrLoaded() const { return m_lastSentOrLoaded.isValid() && !m_lastSentOrLoaded.isNull(); }

            //! \brief Received before n ms
            qint64 timeDiffSentOrLoadedMs() const
            {
                return this->m_lastSentOrLoaded.msecsTo(QDateTime::currentDateTimeUtc());
            }

            //! Get remarks string
            const QString &getRemarks() const { return m_remarks; }

            //! Equals operator
            bool operator ==(const CFlightPlan &other) const;

            //! Not equals operator
            bool operator !=(const CFlightPlan &other) const;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

            //! \copydoc CValueObject::toIcon
            virtual CIcon toIcon() const override;

            //! Register metadata
            static void registerMetadata();

            //! Members
            static const QStringList &jsonMembers();

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
            CAirportIcao m_originAirportIcao;
            CAirportIcao m_destinationAirportIcao;
            CAirportIcao m_alternateAirportIcao;
            QDateTime m_takeoffTimePlanned;
            QDateTime m_takeoffTimeActual;
            PhysicalQuantities::CTime m_enrouteTime;
            PhysicalQuantities::CTime m_fuelTime;
            CAltitude m_cruiseAltitude;
            PhysicalQuantities::CSpeed m_cruiseTrueAirspeed;
            FlightRules m_flightRules;
            QString m_route;
            QString m_remarks;
            QDateTime m_lastSentOrLoaded;
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CFlightPlan)
BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CFlightPlan, (o.m_equipmentIcao, o.m_originAirportIcao, o.m_destinationAirportIcao, o.m_alternateAirportIcao,
                               o.m_takeoffTimePlanned, o.m_takeoffTimeActual, o.m_enrouteTime, o.m_fuelTime, o.m_cruiseAltitude, o.m_cruiseTrueAirspeed, o.m_flightRules, o.m_route, o.m_remarks, o.m_lastSentOrLoaded))

#endif // guard
