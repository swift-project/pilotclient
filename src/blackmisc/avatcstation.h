/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_ATCSTATION_H
#define BLACKMISC_ATCSTATION_H

#include "vvoiceroom.h"
#include "aviocomsystem.h"
#include "avinformationmessage.h"
#include "avcallsign.h"
#include "nwuser.h"
#include "coordinategeodetic.h"
#include "pqfrequency.h"
#include "pqlength.h"
#include "pqtime.h"
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

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \brief Equal operator ==
            bool operator ==(const CAtcStation &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CAtcStation &other) const;

            //! \brief Has booking times?
            bool hasBookingTimes() const
            {
                return !(this->m_bookedFromUtc.isNull() && this->m_bookedUntilUtc.isNull());
            }

            //! \brief Has ATIS?
            bool hasAtis() const
            {
                return this->m_atis.hasMessage();
            }

            //! \brief Has METAR?
            bool hasMetar() const
            {
                return this->m_metar.hasMessage();
            }

            //! Get callsign.
            const CCallsign &getCallsign() const { return m_callsign; }

            //! Get callsign as string.
            QString getCallsignAsString() const { return m_callsign.asString(); }

            //! Set callsign
            void setCallsign(const CCallsign &callsign) { this->m_callsign = callsign; this->m_controller.setCallsign(callsign);}

            //! Get controller
            const BlackMisc::Network::CUser &getController() const { return m_controller; }

            //! Get controller name.
            QString getControllerRealName() const { return m_controller.getRealName(); }

            //! Get controller name.
            QString getControllerId() const { return m_controller.getId(); }

            //! Set controller
            void setController(const BlackMisc::Network::CUser &controller) { this->m_controller = controller; this->m_controller.setCallsign(this->m_callsign); }

            //! Set controller's name
            void setControllerRealName(const QString &controllerName) { this->m_controller.setRealName(controllerName); }

            //! Set controller's id
            void setControllerId(const QString &controllerId) { this->m_controller.setId(controllerId); }

            //! \brief Has valid realname?
            bool hasValidRealName() const { return this->m_controller.hasValidRealName(); }

            //! \brief Has valid id?
            bool hasValidId() const { return this->m_controller.hasValidId(); }

            //! Get frequency.
            const BlackMisc::PhysicalQuantities::CFrequency &getFrequency() const { return m_frequency; }

            //! Set frequency
            void setFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency) { this->m_frequency = frequency; }

            //! Get the position of the center of the controller's area of visibility.
            const BlackMisc::Geo::CCoordinateGeodetic &getPosition() const { return m_position; }

            //! Set position
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_position = position; }

            /*!
             * \brief Syncronize controller data
             * Updates two stations (namely a booked and online ATC station) with complementary data
             * \param otherStation
             */
            void syncronizeControllerData(CAtcStation &otherStation);

            //! \brief Get the radius of the controller's area of visibility.
            const BlackMisc::PhysicalQuantities::CLength &getRange() const { return m_range; }

            //! \brief Set range
            void setRange(const BlackMisc::PhysicalQuantities::CLength &range) { this->m_range = range; }

            //! Get the distance to own plane
            const BlackMisc::PhysicalQuantities::CLength &getDistanceToPlane() const { return m_distanceToPlane; }

            //! Set distance to own plane
            void setDistanceToPlane(const BlackMisc::PhysicalQuantities::CLength &distance) { this->m_distanceToPlane = distance; }

            //! \brief Valid distance?
            bool hasValidDistance() const { return !this->m_distanceToPlane.isNegativeWithEpsilonConsidered();}

            /*!
             * \brief Calculcate distance to plane, set it, and also return it
             * \param position other position
             * \return
             */
            const BlackMisc::PhysicalQuantities::CLength &calculcateDistanceToPlane(const BlackMisc::Geo::CCoordinateGeodetic &position);

            //! \brief Is station online (or just booked)?
            bool isOnline() const { return m_isOnline; }

            //! Set online
            void setOnline(bool online) { this->m_isOnline = online; }

            //! \brief Get voice room
            const BlackMisc::Voice::CVoiceRoom &getVoiceRoom() const { return this->m_voiceRoom; }

            //! \brief Set voice room
            void setVoiceRoom(const BlackMisc::Voice::CVoiceRoom &voiceRoom) { this->m_voiceRoom = voiceRoom; }

            //! \brief Valid voice room?
            bool hasValidVoiceRoom() const { return this->m_voiceRoom.isValid(); }

            /*!
             * Booked date/time if any.
             * This represents the closest booking within a time frame as there can be multiple bookings.
             */
            const QDateTime &getBookedFromUtc() const { return m_bookedFromUtc; }

            //! Set booked from
            void setBookedFromUtc(const QDateTime &from) { this->m_bookedFromUtc = from; }

            /*!
             * Booked date/time if any.
             * This represents the closest booking within a time frame as there can be multiple bookings.
             */
            const QDateTime &getBookedUntilUtc() const { return m_bookedUntilUtc; }

            //! \brief Has valid booking times?
            bool hasValidBookingTimes() const
            {
                return !this->m_bookedFromUtc.isNull() && this->m_bookedFromUtc.isValid() &&
                       !this->m_bookedUntilUtc.isNull() && this->m_bookedUntilUtc.isValid();
            }

            //! \brief Transfer booking times
            void setBookedFromUntil(const CAtcStation &otherStation)
            {
                this->setBookedFromUtc(otherStation.getBookedFromUtc());
                this->setBookedUntilUtc(otherStation.getBookedUntilUtc());
            }

            //! \brief Booked now?
            bool isBookedNow() const;

            //! \brief Tuned in within 25KHz channel spacing
            bool isComUnitTunedIn25KHz(const BlackMisc::Aviation::CComSystem &comUnit) const
            {
                return comUnit.isActiveFrequencyWithin25kHzChannel(this->getFrequency());
            }

            /*!
             * When booked, 0 means now,
             * negative values mean booking in past,
             * positive values mean booking in future,
             * no booking dates will result in - 1 year
             * \return
             */
            BlackMisc::PhysicalQuantities::CTime bookedWhen() const;

            //! \brief Get ATIS
            const CInformationMessage &getAtis() const { return m_atis; }

            //! \brief Set ATIS
            void setAtis(const CInformationMessage &atis) { this->m_atis = atis;}

            //! \brief Set ATIS Message
            void setAtisMessage(const QString &atis) { this->m_atis.setMessage(atis); }

            //! \brief Get METAR
            const CInformationMessage &getMetar() const { return m_metar; }

            //! \brief Set METAR
            void setMetar(const CInformationMessage &metar) { this->m_metar = metar;}

            //! \brief Set METAR Message
            void setMetarMessage(const QString &metar) { this->m_metar.setMessage(metar); }

            //! Set booked until
            void setBookedUntilUtc(const QDateTime &until) { this->m_bookedUntilUtc = until; }

            //! \copydoc ICoordinateGeodetic::latitude
            virtual const BlackMisc::Geo::CLatitude &latitude() const override
            {
                return this->getPosition().latitude();
            }

            //! \copydoc ICoordinateGeodetic::longitude
            virtual const BlackMisc::Geo::CLongitude &longitude() const override
            {
                return this->getPosition().longitude();
            }

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::propertyByIndex()
            virtual QVariant propertyByIndex(int index) const override;

            //! \copydoc CValueObject::setPropertyByIndex(variant, index)
            virtual void setPropertyByIndex(const QVariant &variant, int index) override;

            //! \copydoc CValueObject::propertyByIndexAsString()
            virtual QString propertyByIndexAsString(int index, bool i18n = false) const override;

            //! \brief Register metadata
            static void registerMetadata();

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

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
