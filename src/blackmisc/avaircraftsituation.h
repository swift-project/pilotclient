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
#include <QDateTime>

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
            //! \brief Default constructor.
            CAircraftSituation() : m_timestamp(QDateTime::currentDateTimeUtc()) {}

            //! \brief Comprehensive Constructor
            CAircraftSituation(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude,
                               const BlackMisc::Aviation::CHeading &heading = BlackMisc::Aviation::CHeading(),
                               const BlackMisc::PhysicalQuantities::CAngle &pitch = BlackMisc::PhysicalQuantities::CAngle(),
                               const BlackMisc::PhysicalQuantities::CAngle &bank = BlackMisc::PhysicalQuantities::CAngle(),
                               const BlackMisc::PhysicalQuantities::CSpeed &gs = BlackMisc::PhysicalQuantities::CSpeed())
                : m_position(position), m_altitude(altitude), m_heading(heading), m_pitch(pitch), m_bank(bank), m_groundspeed(gs),
                  m_timestamp(QDateTime::currentDateTimeUtc()) {}

            //! \brief Properties by index
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

            //! \copydoc CValueObject::propertyByIndex(index)
            virtual QVariant propertyByIndex(int index) const override;

            //! \copydoc CValueObject::propertyByIndexAsString
            virtual QString propertyByIndexAsString(int index, bool i18n) const override;

            //! \copydoc CValueObject::setPropertyByIndex(variant,index)
            virtual void setPropertyByIndex(const QVariant &variant, int index) override;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            //! \brief Get position
            const BlackMisc::Geo::CCoordinateGeodetic &getPosition() const { return this->m_position; }

            //! \brief Set position
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_position = position; }

            //! \copydoc ICoordinateGeodetic::latitude()
            virtual const BlackMisc::Geo::CLatitude &latitude() const override { return this->m_position.latitude(); }

            //! \copydoc ICoordinateGeodetic::longitude()
            virtual const BlackMisc::Geo::CLongitude &longitude() const override { return this->m_position.longitude(); }

            //! \copydoc CCoordinateGeodetic::height
            const BlackMisc::PhysicalQuantities::CLength &getHeight() const { return this->m_position.height(); }

            //! \brief Set height
            void setHeight(const BlackMisc::PhysicalQuantities::CLength &height) { this->m_position.setHeight(height); }

            //! \brief Get heading
            const BlackMisc::Aviation::CHeading &getHeading() const { return this->m_heading; }

            //! \brief Set heading
            void setHeading(const BlackMisc::Aviation::CHeading &heading) { this->m_heading = heading; }

            //! \brief Get altitude (true)
            const BlackMisc::Aviation::CAltitude &getAltitude() const { return this->m_altitude; }

            //! \brief Set altitude
            void setAltitude(const BlackMisc::Aviation::CAltitude &altitude) { this->m_altitude = altitude; }

            //! \brief Get pitch
            const BlackMisc::PhysicalQuantities::CAngle &getPitch() const { return this->m_pitch; }

            //! \brief Set pitch
            void setPitch(const BlackMisc::PhysicalQuantities::CAngle &pitch) { this->m_pitch = pitch; }

            //! Get bank (angle)
            const BlackMisc::PhysicalQuantities::CAngle &getBank() const { return this->m_bank; }

            //! \brief Set bank (angle)
            void setBank(const BlackMisc::PhysicalQuantities::CAngle &bank) { this->m_bank = bank; }

            //! \brief Get groundspeed
            const BlackMisc::PhysicalQuantities::CSpeed &getGroundSpeed() const { return this->m_groundspeed; }

            //! \brief Set groundspeed
            void setGroundspeed(const BlackMisc::PhysicalQuantities::CSpeed &groundspeed) { this->m_groundspeed = groundspeed; }

            //! \brief Timestamp
            const QDateTime &getTimestamp() const { return this->m_timestamp;}

            //! \brief Equal operator ==
            bool operator ==(const CAircraftSituation &other) const;

            //! \brief Unequal operator !=
            bool operator !=(const CAircraftSituation &other) const;

            //! \copydoc CValueObject::getValueHash
            virtual uint getValueHash() const override;

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

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

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
