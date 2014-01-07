/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_ATCSTATION_H
#define BLACKMISC_ATCSTATION_H
#include "valuemap.h"
#include "pqfrequency.h"
#include "pqlength.h"
#include "pqtime.h"
#include "nwuser.h"
#include "coordinategeodetic.h"
#include "avcallsign.h"
#include "avinformationmessage.h"
#include <QDateTime>
#include <QMap>
#include <functional>

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object encapsulating information about an ATC station.
         */
        class CAtcStation : public BlackMisc::CValueObject, public BlackMisc::Geo::ICoordinateGeodetic
        {

        public:
            /*!
             * \brief Properties by index
             */
            enum ColumnIndex
            {
                IndexCallsign = 0,
                IndexCallsignAsString,
                IndexCallsignAsStringAsSet,
                IndexController,
                IndexControllerRealName,
                IndexControllerId,
                IndexFrequency,
                IndexPosition,
                IndexRange,
                IndexDistance,
                IndexIsOnline,
                IndexBookedFrom,
                IndexBookedUntil,
                IndexLatitude,
                IndexLongitude,
                IndexAtis,
                IndexAtisMessage,
                IndexMetar,
                IndexMetarMessage
            };

            /*!
             * Default constructor.
             */
            CAtcStation();

            /*!
             * \brief Simplified constructor
             * \param callsign
             */
            CAtcStation(const QString &callsign);

            /*!
             * \brief ATC station
             * \param callsign
             * \param controllerName
             * \param freq
             * \param pos
             * \param range
             * \param isOnline
             * \param bookedFromUtc
             * \param bookedUntilUtc
             * \param atis
             */
            CAtcStation(const CCallsign &callsign, const BlackMisc::Network::CUser &controller,
                        const BlackMisc::PhysicalQuantities::CFrequency &frequency,
                        const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range,
                        bool isOnline = false, const QDateTime &bookedFromUtc = QDateTime(),  const QDateTime &bookedUntilUtc = QDateTime(),
                        const CInformationMessage &atis = CInformationMessage(CInformationMessage::ATIS), const CInformationMessage &metar = CInformationMessage(CInformationMessage::METAR));

            /*!
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Equal operator ==
             * \param other
             * @return
             */
            bool operator ==(const CAtcStation &other) const;

            /*!
             * \brief Unequal operator ==
             * \param other
             * @return
             */
            bool operator !=(const CAtcStation &other) const;

            /*!
             * \brief Has booking times?
             * \return
             */
            bool hasBookingTimes() const
            {
                return !(this->m_bookedFromUtc.isNull() && this->m_bookedUntilUtc.isNull());
            }

            /*!
             * \brief Has ATIS?
             * \return
             */
            bool hasAtis() const
            {
                return this->m_atis.hasMessage();
            }

            /*!
             * \brief Has Metar?
             * \return
             */
            bool hasMetar() const
            {
                return this->m_metar.hasMessage();
            }

            /*!
             * Get callsign.
             * \return
             */
            const CCallsign &getCallsign() const { return m_callsign; }

            /*!
             * Get callsign.
             * \return
             */
            QString getCallsignAsString() const { return m_callsign.asString(); }

            /*!
             * Set callsign
             * \param callsign
             */
            void setCallsign(const CCallsign &callsign) { this->m_callsign = callsign; }

            /*!
             * Get controller
             * \return
             */
            const BlackMisc::Network::CUser &getController() const { return m_controller; }

            /*!
             * Get controller name.
             * \return
             */
            QString getControllerRealName() const { return m_controller.getRealName(); }

            /*!
             * Get controller name.
             * \return
             */
            QString getControllerId() const { return m_controller.getId(); }

            /*!
             * Set controller
             * \param controller
             */
            void setController(const BlackMisc::Network::CUser &controller) { this->m_controller = controller; }

            /*!
             * Set controller's name
             * \param controllerName
             */
            void setControllerRealName(const QString &controllerName) { this->m_controller.setRealName(controllerName); }

            /*!
             * Set controller's id
             * \param controllerId
             */
            void setControllerId(const QString &controllerId) { this->m_controller.setId(controllerId); }

            /*!
             * \brief Has valid realname?
             * \return
             */
            bool hasValidRealName() const { return this->m_controller.hasValidRealName(); }

            /*!
             * \brief Has valid id?
             * \return
             */
            bool hasValidId() const { return this->m_controller.hasValidId(); }

            /*!
             * Get frequency.
             * \return
             */
            const BlackMisc::PhysicalQuantities::CFrequency &getFrequency() const { return m_frequency; }

            /*!
             * Set frequency
             * \param frequency
             */
            void setFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) { this->m_frequency = frequency; }

            /*!
             * Get the position of the center of the controller's area of visibility.
             * \return
             */
            const BlackMisc::Geo::CCoordinateGeodetic &getPosition() const { return m_position; }

            /*!
             * Set position
             * \param position
             */
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_position = position; }

            /*!
             * \brief Syncronize controller data
             * Updates two stations (namely a booked and online ATC station) with complementary data
             * \param otherStation
             */
            void syncronizeControllerData(CAtcStation &otherStation);

            /*!
             * Get the radius of the controller's area of visibility.
             * \return
             */
            const BlackMisc::PhysicalQuantities::CLength &getRange() const { return m_range; }

            /*!
             * Set range
             * \param range
             */
            void setRange(const BlackMisc::PhysicalQuantities::CLength &range) { this->m_range = range; }

            /*!
             * Get the distance to own plane
             * \return
             */
            const BlackMisc::PhysicalQuantities::CLength &getDistanceToPlane() const { return m_distanceToPlane; }

            /*!
             * Set distance to own plane
             * \param range
             */
            void setDistanceToPlane(const BlackMisc::PhysicalQuantities::CLength &distance) { this->m_distanceToPlane = distance; }

            /*!
             * \brief Valid distance?
             * \return
             */
            bool hasValidDistance() const { return !this->m_distanceToPlane.isNegativeWithEpsilonConsidered();}

            /*!
             * \brief Calculcate distance to plane, set it, and also return it
             * \param position
             * \return
             */
            const BlackMisc::PhysicalQuantities::CLength &calculcateDistanceToPlane(const BlackMisc::Geo::CCoordinateGeodetic &position);

            /*!
             * \brief Is station online (or just booked)
             * \return
             */
            bool isOnline() const { return m_isOnline; }

            /*!
             * Set online
             * \param online
             */
            void setOnline(bool online) { this->m_isOnline = online; }

            /*!
             * Booked date/time if any.
             * This represents the closest booking within a time frame as there can be multiple bookings.
             * \return
             */
            const QDateTime &getBookedFromUtc() const { return m_bookedFromUtc; }

            /*!
             * Set booked from
             * \param until
             */
            void setBookedFromUtc(const QDateTime &from) { this->m_bookedFromUtc = from; }

            /*!
             * Booked date/time if any.
             * This represents the closest booking within a time frame as there can be multiple bookings.
             * \return
             */
            const QDateTime &getBookedUntilUtc() const { return m_bookedUntilUtc; }

            /*!
             * \brief Has valid booking times?
             * \return
             */
            bool hasValidBookingTimes() const
            {
                return !this->m_bookedFromUtc.isNull() && this->m_bookedFromUtc.isValid() &&
                       !this->m_bookedUntilUtc.isNull() && this->m_bookedUntilUtc.isValid();
            }

            /*!
             * \brief Transfer booking times
             * \param otherStation
             */
            void setBookedFromUntil(const CAtcStation &otherStation)
            {
                this->setBookedFromUtc(otherStation.getBookedFromUtc());
                this->setBookedUntilUtc(otherStation.getBookedUntilUtc());
            }

            /*!
             * \brief Booked now
             * \return
             */
            bool isBookedNow() const;

            /*!
             * When booked, 0 means now,
             * negative values mean booking in past,
             * positive values mean booking in future,
             * no booking dates will result in - 1 year
             * \return
             */
            BlackMisc::PhysicalQuantities::CTime bookedWhen() const;

            /*!
             * \brief Get atis
             * \return
             */
            const CInformationMessage &getAtis() const { return m_atis; }

            /*!
             * \brief Set ATIS
             * \param msg
             */
            void setAtis(const CInformationMessage &atis) { this->m_atis = atis;}

            /*!
             * \brief Set ATIS Message
             * \param atis
             */
            void setAtisMessage(const QString &atis) { this->m_atis.setMessage(atis); }

            /*!
             * \brief Get METAR
             * \return
             */
            const CInformationMessage &getMetar() const { return m_metar; }

            /*!
             * \brief Set METAR
             * \param msg
             */
            void setMetar(const CInformationMessage &metar) { this->m_metar = metar;}

            /*!
             * \brief Set METAR Message
             * \param msg
             */
            void setMetarMessage(const QString &metar) { this->m_metar.setMessage(metar); }

            /*!
             * Set booked until
             * \param until
             */
            void setBookedUntilUtc(const QDateTime &until) { this->m_bookedUntilUtc = until; }

            /*!
             * \brief Latitude
             * \return
             */
            virtual const BlackMisc::Geo::CLatitude &latitude() const
            {
                return this->getPosition().latitude();
            }

            /*!
             * \brief Longitude
             * \return
             */
            virtual const BlackMisc::Geo::CLongitude &longitude() const
            {
                return this->getPosition().longitude();
            }

            /*!
             * \brief Hash value
             * \return
             */
            virtual uint getValueHash() const;

            /*!
             * \brief Property by index
             * \param index
             * \return
             */
            virtual QVariant propertyByIndex(int index) const;

            /*!
             * \brief Property by index (setting)
             * \param variant
             * \return
             */
            virtual void setPropertyByIndex(const QVariant &variant, int index);

            /*!
             * \brief Property by index as String
             * \param index
             * \param i18n
             * \return
             */
            virtual QString propertyByIndexAsString(int index, bool i18n = false) const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

            /*!
             * \copydoc BlackObject::compare
             */
            virtual int compare(const QVariant &qv) const;

        protected:
            /*!
             * \brief Meaningful string representation
             * \param i18n
             * \return
             */
            virtual QString convertToQString(bool i18n = false) const;

            /*!
             * \brief Stream to DBus <<
             * \param argument
             */
            virtual void marshallToDbus(QDBusArgument &argument) const;

            /*!
             * \brief Stream from DBus >>
             * \param argument
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument);

        private:
            CCallsign m_callsign;
            BlackMisc::Network::CUser m_controller;
            BlackMisc::PhysicalQuantities::CFrequency m_frequency;
            BlackMisc::Geo::CCoordinateGeodetic m_position;
            BlackMisc::PhysicalQuantities::CLength m_range;
            BlackMisc::PhysicalQuantities::CLength m_distanceToPlane;
            bool m_isOnline;
            QDateTime m_bookedFromUtc;
            QDateTime m_bookedUntilUtc;
            CInformationMessage m_atis;
            CInformationMessage m_metar;
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAtcStation)

#endif // guard
