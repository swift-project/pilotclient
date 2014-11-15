/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

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
        class CAircraftSituation : public CValueObjectStdTuple<CAircraftSituation>, public Geo::ICoordinateGeodetic
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexPosition = BlackMisc::CPropertyIndex::GlobalIndexCAircraftSituation,
                IndexLatitude,
                IndexLongitude,
                IndexAltitude,
                IndexHeading,
                IndexBank,
                IndexPitch,
                IndexGroundspeed,
                IndexTimeStamp,
                IndexTimeStampFormatted
            };

            //! Default constructor.
            CAircraftSituation() : m_timestamp(QDateTime::currentDateTimeUtc()) {}

            //! Comprehensive Constructor
            CAircraftSituation(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude,
                               const BlackMisc::Aviation::CHeading &heading = BlackMisc::Aviation::CHeading(),
                               const BlackMisc::PhysicalQuantities::CAngle &pitch = BlackMisc::PhysicalQuantities::CAngle(),
                               const BlackMisc::PhysicalQuantities::CAngle &bank = BlackMisc::PhysicalQuantities::CAngle(),
                               const BlackMisc::PhysicalQuantities::CSpeed &gs = BlackMisc::PhysicalQuantities::CSpeed())
                : m_position(position), m_altitude(altitude), m_heading(heading), m_pitch(pitch),
                  m_bank(bank), m_groundspeed(gs), m_timestamp(QDateTime::currentDateTimeUtc()) {}

            //! \copydoc CValueObject::propertyByIndex
            virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! Get position
            const BlackMisc::Geo::CCoordinateGeodetic &getPosition() const { return this->m_position; }

            //! Set position
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_position = position; }

            //! \copydoc ICoordinateGeodetic::latitude()
            virtual const BlackMisc::Geo::CLatitude &latitude() const override { return this->m_position.latitude(); }

            //! \copydoc ICoordinateGeodetic::longitude()
            virtual const BlackMisc::Geo::CLongitude &longitude() const override { return this->m_position.longitude(); }

            //! \copydoc CCoordinateGeodetic::height
            const BlackMisc::PhysicalQuantities::CLength &getHeight() const { return this->m_position.geodeticHeight(); }

            //! Set height
            void setHeight(const BlackMisc::PhysicalQuantities::CLength &height) { this->m_position.setGeodeticHeight(height); }

            //! Get heading
            const BlackMisc::Aviation::CHeading &getHeading() const { return this->m_heading; }

            //! Set heading
            void setHeading(const BlackMisc::Aviation::CHeading &heading) { this->m_heading = heading; }

            //! Get altitude (true)
            const BlackMisc::Aviation::CAltitude &getAltitude() const { return this->m_altitude; }

            //! Set altitude
            void setAltitude(const BlackMisc::Aviation::CAltitude &altitude) { this->m_altitude = altitude; }

            //! Get pitch
            const BlackMisc::PhysicalQuantities::CAngle &getPitch() const { return this->m_pitch; }

            //! Set pitch
            void setPitch(const BlackMisc::PhysicalQuantities::CAngle &pitch) { this->m_pitch = pitch; }

            //! Get bank (angle)
            const BlackMisc::PhysicalQuantities::CAngle &getBank() const { return this->m_bank; }

            //! Set bank (angle)
            void setBank(const BlackMisc::PhysicalQuantities::CAngle &bank) { this->m_bank = bank; }

            //! Get groundspeed
            const BlackMisc::PhysicalQuantities::CSpeed &getGroundSpeed() const { return this->m_groundspeed; }

            //! Set groundspeed
            void setGroundspeed(const BlackMisc::PhysicalQuantities::CSpeed &groundspeed) { this->m_groundspeed = groundspeed; }

            //! Timestamp
            const QDateTime &getTimestamp() const { return this->m_timestamp;}

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftSituation)
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

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAircraftSituation, (o.m_position, o.m_altitude, o.m_heading, o.m_pitch, o.m_bank, o.m_groundspeed))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation)

#endif // guard
