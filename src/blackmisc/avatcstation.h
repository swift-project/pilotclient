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
#include "vvoiceroom.h"
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
                IndexMetarMessage,
                IndexVoiceRoom,
                IndexVoiceRoomUrl
            };

            /*!
             * \brief Default constructor.
             */
            CAtcStation();

            /*!
             * \brief Simplified constructor
             */
            CAtcStation(const QString &callsign);

            /*!
             * \brief ATC station constructor
             */
            CAtcStation(const CCallsign &callsign, const BlackMisc::Network::CUser &controller,
                        const BlackMisc::PhysicalQuantities::CFrequency &frequency,
                        const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range,
                        bool isOnline = false, const QDateTime &bookedFromUtc = QDateTime(),  const QDateTime &bookedUntilUtc = QDateTime(),
                        const CInformationMessage &atis = CInformationMessage(CInformationMessage::ATIS), const CInformationMessage &metar = CInformationMessage(CInformationMessage::METAR));

            /*!
             * \copydoc CValueObject::toQVariant()
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Equal operator ==
             */
            bool operator ==(const CAtcStation &other) const;

            /*!
             * \brief Unequal operator ==
             */
            bool operator !=(const CAtcStation &other) const;

            /*!
             * \brief Has booking times?
             */
            bool hasBookingTimes() const
            {
                return !(this->m_bookedFromUtc.isNull() && this->m_bookedUntilUtc.isNull());
            }

            /*!
             * \brief Has ATIS?
             */
            bool hasAtis() const
            {
                return this->m_atis.hasMessage();
            }

            /*!
             * \brief Has Metar?
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
             * Get callsign as string.
             */
            QString getCallsignAsString() const { return m_callsign.asString(); }

            /*!
             * Set callsign
             */
            void setCallsign(const CCallsign &callsign) { this->m_callsign = callsign; this->m_controller.setCallsign(callsign);}

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
            void setController(const BlackMisc::Network::CUser &controller) { this->m_controller = controller; this->m_controller.setCallsign(this->m_callsign); }

            /*!
             * Set controller's name
             * \param controllerName
             */
            void setControllerRealName(const QString &controllerName) { this->m_controller.setRealName(controllerName); }

            /*!
             * Set controller's id
             */
            void setControllerId(const QString &controllerId) { this->m_controller.setId(controllerId); }

            /*!
             * \brief Has valid realname?
             */
            bool hasValidRealName() const { return this->m_controller.hasValidRealName(); }

            /*!
             * \brief Has valid id?
             */
            bool hasValidId() const { return this->m_controller.hasValidId(); }

            /*!
             * Get frequency.
             */
            const BlackMisc::PhysicalQuantities::CFrequency &getFrequency() const { return m_frequency; }

            /*!
             * Set frequency
             */
            void setFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) { this->m_frequency = frequency; }

            /*!
             * Get the position of the center of the controller's area of visibility.
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
             */
            const BlackMisc::PhysicalQuantities::CLength &getDistanceToPlane() const { return m_distanceToPlane; }

            /*!
             * Set distance to own plane
             */
            void setDistanceToPlane(const BlackMisc::PhysicalQuantities::CLength &distance) { this->m_distanceToPlane = distance; }

            /*!
             * \brief Valid distance?
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
             * \brief Get voice room
             * \return
             */
            const BlackMisc::Voice::CVoiceRoom &getVoiceRoom() const { return this->m_voiceRoom; }

            /*!
             * \brief Set voice room
             * \param
             */
            void setVoiceRoom(const BlackMisc::Voice::CVoiceRoom &voiceRoom) { this->m_voiceRoom = voiceRoom; }

            /*!
             * \brief Valid voice room?
             * \return
             */
            bool hasValidVoiceRoom() const { return this->m_voiceRoom.isValid(); }

            /*!
             * Booked date/time if any.
             * This represents the closest booking within a time frame as there can be multiple bookings.
             * \return
             */
            const QDateTime &getBookedFromUtc() const { return m_bookedFromUtc; }

            /*!
             * Set booked from
             */
            void setBookedFromUtc(const QDateTime &from) { this->m_bookedFromUtc = from; }

            /*!
             * Booked date/time if any.
             * This represents the closest booking within a time frame as there can be multiple bookings.
             */
            const QDateTime &getBookedUntilUtc() const { return m_bookedUntilUtc; }

            /*!
             * \brief Has valid booking times?
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
             * \brief Get ATIS
             * \return
             */
            const CInformationMessage &getAtis() const { return m_atis; }

            /*!
             * \brief Set ATIS
             */
            void setAtis(const CInformationMessage &atis) { this->m_atis = atis;}

            /*!
             * \brief Set ATIS Message
             */
            void setAtisMessage(const QString &atis) { this->m_atis.setMessage(atis); }

            /*!
             * \brief Get METAR
             */
            const CInformationMessage &getMetar() const { return m_metar; }

            /*!
             * \brief Set METAR
             */
            void setMetar(const CInformationMessage &metar) { this->m_metar = metar;}

            /*!
             * \brief Set METAR Message
             */
            void setMetarMessage(const QString &metar) { this->m_metar.setMessage(metar); }

            /*!
             * Set booked until
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
             * \copydoc CValueObject::getValueHash()
             */
            virtual uint getValueHash() const;

            /*!
             * \coypdoc CValueObject::propertyByIndex()
             */
            virtual QVariant propertyByIndex(int index) const;

            /*!
             * \copydoc CValueObject::setPropertyByIndex(const QVariant &, int)
             */
            virtual void setPropertyByIndex(const QVariant &variant, int index);

            /*!
             * \copydoc CValueObject::propertyByIndexAsString()
             */
            virtual QString propertyByIndexAsString(int index, bool i18n = false) const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

        protected:
            /*!
             * \brief Meaningful string representation
             * \param i18n
             * \return
             */
            virtual QString convertToQString(bool i18n = false) const;

            /*!
             * \copydoc CValueObject::getMetaTypeId
             */
            virtual int getMetaTypeId() const;

            /*!
             * \copydoc CValueObject::isA
             */
            virtual bool isA(int metaTypeId) const;

            /*!
             * \copydoc CValueObject::compareImpl
             */
            virtual int compareImpl(const CValueObject &other) const;

            /*!
             * \copydoc CValueObject::marshallToDbus()
             */
            virtual void marshallToDbus(QDBusArgument &argument) const;

            /*!
             * \copydoc CValueObject::unmarshallFromDbus()
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
            BlackMisc::Voice::CVoiceRoom m_voiceRoom;
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAtcStation)

#endif // guard
