/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_AIRCRAFTSITUATION_H
#define BLACKMISC_AIRCRAFTSITUATION_H

#include "coordinategeodetic.h"
#include "avaltitude.h"
#include "avheading.h"
#include "pqspeed.h"
#include "valueobject.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object encapsulating information of an aircraft's situation
         */
        class CAircraftSituation : public BlackMisc::CValueObject, public BlackMisc::Geo::ICoordinateGeodetic
        {
        public:
            /*!
             * Default constructor.
             */
            CAircraftSituation() : m_timestamp(QDateTime::currentDateTimeUtc()) {}

            /*!
             * Constructor.
             * \param position
             * \param altitude
             * \param heading
             * \param pitch
             * \param bank
             * \param gs
             * \param parent
             */
            CAircraftSituation(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude,
                               const BlackMisc::Aviation::CHeading &heading = BlackMisc::Aviation::CHeading(),
                               const BlackMisc::PhysicalQuantities::CAngle &pitch = BlackMisc::PhysicalQuantities::CAngle(),
                               const BlackMisc::PhysicalQuantities::CAngle &bank = BlackMisc::PhysicalQuantities::CAngle(),
                               const BlackMisc::PhysicalQuantities::CSpeed &gs = BlackMisc::PhysicalQuantities::CSpeed())
                : m_position(position), m_altitude(altitude), m_heading(heading), m_pitch(pitch), m_bank(bank), m_groundspeed(gs),
                m_timestamp(QDateTime::currentDateTimeUtc()) {}

            /*!
             * \brief Properties by index
             */
            enum ColumnIndex
            {
                IndexPosition = 1000, // used, so it can be chained in aircraft
                IndexPositionLatitude,
                IndexPositionLongitude,
                IndexPositionHeight,
                IndexAltitude,
                IndexHeading,
                IndexBank,
                IndexPitch,
                IndexGroundspeed,
                IndexTimeStamp,
                IndexTimeStampFormatted
            };

            /*!
             * \brief Property by index
             * \param index
             * \return
             */
            virtual QVariant propertyByIndex(int index) const;

            /*!
             * \brief Property by index as string
             * \param index
             * \param i18n
             * \return
             */
            virtual QString propertyByIndexAsString(int index, bool i18n) const;

            /*!
             * \brief Property by index
             * \param variant
             * \param index
             */
            virtual void setPropertyByIndex(const QVariant &variant, int index);

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Get position
             * \return
             */
            const BlackMisc::Geo::CCoordinateGeodetic &getPosition() const { return this->m_position; }

            /*!
             * \brief Set position
             * \param position
             */
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_position = position; }

            /*!
             * \brief Get latitude
             * \return
             */
            virtual const BlackMisc::Geo::CLatitude &latitude() const { return this->m_position.latitude(); }

            /*!
             * \brief Get longitude
             * \return
             */
            virtual const BlackMisc::Geo::CLongitude &longitude() const { return this->m_position.longitude(); }

            /*!
             * \brief Get height
             * \return
             */
            const BlackMisc::PhysicalQuantities::CLength &getHeight() const { return this->m_position.height(); }

            /*!
             * \brief Set height
             * \param
             */
            void setHeight(const BlackMisc::PhysicalQuantities::CLength &height) { this->m_position.setHeight(height); }

            /*!
             * \brief Get heading
             * \return
             */
            const BlackMisc::Aviation::CHeading &getHeading() const { return this->m_heading; }

            /*!
             * \brief Set heading
             * \param
             */
            void setHeading(const BlackMisc::Aviation::CHeading &heading) { this->m_heading = heading; }

            /*!
             * \brief Get altitude (true)
             * \return
             */
            const BlackMisc::Aviation::CAltitude &getAltitude() const { return this->m_altitude; }

            /*!
             * \brief Set altitude
             * \param
             */
            void setAltitude(const BlackMisc::Aviation::CAltitude &altitude) { this->m_altitude = altitude; }

            /*!
             * \brief Get pitch
             * \return
             */
            const BlackMisc::PhysicalQuantities::CAngle &getPitch() const { return this->m_pitch; }

            /*!
             * \brief Set pitch
             * \param
             */
            void setPitch(const BlackMisc::PhysicalQuantities::CAngle &pitch) { this->m_pitch = pitch; }

            /*!
             * \brief Get bank
             * \return
             */
            const BlackMisc::PhysicalQuantities::CAngle &getBank() const { return this->m_bank; }

            /*!
             * \brief Set bank
             * \param
             */
            void setBank(const BlackMisc::PhysicalQuantities::CAngle &bank) { this->m_bank = bank; }

            /*!
             * \brief Get groundspeed
             * \return
             */
            const BlackMisc::PhysicalQuantities::CSpeed &getGroundSpeed() const { return this->m_groundspeed; }

            /*!
             * \brief Set groundspeed
             * \param
             */
            void setGroundspeed(const BlackMisc::PhysicalQuantities::CSpeed &groundspeed) { this->m_groundspeed = groundspeed; }

            /*!
             * \brief Timestamp
             * \return
             */
            const QDateTime &getTimestamp() const { return this->m_timestamp;}

            /*!
             * \brief Equal operator ==
             * \param other
             * @return
             */
            bool operator ==(const CAircraftSituation &other) const;

            /*!
             * \brief Unequal operator ==
             * \param other
             * @return
             */
            bool operator !=(const CAircraftSituation &other) const;

            /*!
             * \brief Value hash
             */
            virtual uint getValueHash() const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();


        protected:
            /*!
             * \brief Rounded value as string
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
            BlackMisc::Geo::CCoordinateGeodetic m_position;
            BlackMisc::Aviation::CAltitude m_altitude;
            BlackMisc::Aviation::CHeading m_heading;
            BlackMisc::PhysicalQuantities::CAngle m_pitch;
            BlackMisc::PhysicalQuantities::CAngle m_bank;
            BlackMisc::PhysicalQuantities::CSpeed m_groundspeed;
            QDateTime m_timestamp;

        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation)

#endif // guard
