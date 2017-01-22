/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTSITUATION_H
#define BLACKMISC_AVIATION_AIRCRAFTSITUATION_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <array>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information of an aircraft's situation
        class BLACKMISC_EXPORT CAircraftSituation :
            public CValueObject<CAircraftSituation>,
            public BlackMisc::Geo::ICoordinateGeodetic, public BlackMisc::ITimestampBased
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
                IndexGroundSpeed,
                IndexGroundElevation,
                IndexCallsign
            };

            //! Is on ground?
            enum IsOnGround
            {
                NotOnGround,
                OnGround,
                OnGroundSituationUnknown
            };

            //! Reliability of on ground information
            enum OnGroundReliability
            {
                OnGroundByGuessing,
                OnGroundByElevation,
                OnGroundByInterpolation,
                OnGroundReliabilityNoSet
            };

            //! Default constructor.
            CAircraftSituation();

            //! Comprehensive constructor
            CAircraftSituation(const BlackMisc::Geo::CCoordinateGeodetic &position,
                               const BlackMisc::Aviation::CHeading &heading = {},
                               const BlackMisc::PhysicalQuantities::CAngle &pitch = {},
                               const BlackMisc::PhysicalQuantities::CAngle &bank = {},
                               const BlackMisc::PhysicalQuantities::CSpeed &gs = {},
                               const BlackMisc::Aviation::CAltitude &groundElevation = { 0, nullptr });

            //! Comprehensive constructor
            CAircraftSituation(const BlackMisc::Aviation::CCallsign &correspondingCallsign,
                               const BlackMisc::Geo::CCoordinateGeodetic &position,
                               const BlackMisc::Aviation::CHeading &heading = {},
                               const BlackMisc::PhysicalQuantities::CAngle &pitch = {},
                               const BlackMisc::PhysicalQuantities::CAngle &bank = {},
                               const BlackMisc::PhysicalQuantities::CSpeed &gs = {},
                               const BlackMisc::Aviation::CAltitude &groundElevation = { 0, nullptr });

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! Compare by index
            int comparePropertyByIndex(const CPropertyIndex &index, const CAircraftSituation &compareValue) const;

            //! Get position
            const BlackMisc::Geo::CCoordinateGeodetic &getPosition() const { return this->m_position; }

            //! Set position
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_position = position; }

            //! \copydoc Geo::ICoordinateGeodetic::latitude()
            virtual BlackMisc::Geo::CLatitude latitude() const override { return this->m_position.latitude(); }

            //! \copydoc Geo::ICoordinateGeodetic::longitude()
            virtual BlackMisc::Geo::CLongitude longitude() const override { return this->m_position.longitude(); }

            //! On ground?
            IsOnGround isOnGround() const { return static_cast<CAircraftSituation::IsOnGround>(m_isOnGround); }

            //! On ground info available?
            bool isOnGroundInfoAvailable() const;

            //! Set on ground
            void setOnGround(CAircraftSituation::IsOnGround onGround) { m_isOnGround = static_cast<int>(onGround); }

            //! Set on ground
            void setOnGround(CAircraftSituation::IsOnGround onGround, CAircraftSituation::OnGroundReliability reliability);

            //! On ground reliability
            OnGroundReliability getOnGroundReliability() const { return static_cast<CAircraftSituation::OnGroundReliability>(m_onGroundReliability); }

            //! Reliability
            void setOnGroundReliabiliy(CAircraftSituation::OnGroundReliability onGroundReliability) { m_onGroundReliability = static_cast<int>(onGroundReliability); }

            //! \copydoc Geo::ICoordinateGeodetic::geodeticHeight
            const BlackMisc::Aviation::CAltitude &geodeticHeight() const override { return this->m_position.geodeticHeight(); }

            //! \copydoc Geo::ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override { return this->m_position.normalVector(); }

            //! \copydoc Geo::ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override { return this->m_position.normalVectorDouble(); }

            //! Elevation of the ground directly beneath
            const BlackMisc::Aviation::CAltitude &getGroundElevation() const { return this->m_groundElevation; }

            //! Is ground elevation value available
            bool hasGroundElevation() const;

            //! Elevation of the ground directly beneath
            void setGroundElevation(const BlackMisc::Aviation::CAltitude &elevation) { this->m_groundElevation = elevation; }

            //! Height above ground.
            BlackMisc::PhysicalQuantities::CLength getHeightAboveGround() const;

            //! Get heading
            const BlackMisc::Aviation::CHeading &getHeading() const { return this->m_heading; }

            //! Set heading
            void setHeading(const BlackMisc::Aviation::CHeading &heading) { this->m_heading = heading; }

            //! Get altitude
            const BlackMisc::Aviation::CAltitude &getAltitude() const { return this->m_position.geodeticHeight(); }

            //! Get altitude under consideration of ground elevation
            BlackMisc::Aviation::CAltitude getCorrectedAltitude() const;

            //! Set altitude
            void setAltitude(const BlackMisc::Aviation::CAltitude &altitude) { this->m_position.setGeodeticHeight(altitude); }

            //! Get pitch
            const BlackMisc::PhysicalQuantities::CAngle &getPitch() const { return this->m_pitch; }

            //! Set pitch
            void setPitch(const BlackMisc::PhysicalQuantities::CAngle &pitch) { this->m_pitch = pitch; }

            //! Get bank (angle)
            const BlackMisc::PhysicalQuantities::CAngle &getBank() const { return this->m_bank; }

            //! Set bank (angle)
            void setBank(const BlackMisc::PhysicalQuantities::CAngle &bank) { this->m_bank = bank; }

            //! Get ground speed
            const BlackMisc::PhysicalQuantities::CSpeed &getGroundSpeed() const { return this->m_groundSpeed; }

            //! Set ground speed
            void setGroundSpeed(const BlackMisc::PhysicalQuantities::CSpeed &groundspeed) { this->m_groundSpeed = groundspeed; }

            //! Corresponding callsign
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return this->m_correspondingCallsign; }

            //! Corresponding callsign
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);

            //! Milliseconds to add to timestamp for interpolation
            void setTimeOffsetMs(qint64 offset) { this->m_timeOffsetMs = offset; }

            //! Milliseconds to add to timestamp for interpolation
            qint64 getTimeOffsetMs() const { return this->m_timeOffsetMs; }

            //! Timestamp with offset added for interpolation
            qint64 getAdjustedMSecsSinceEpoch() const { return this->getMSecsSinceEpoch() + this->getTimeOffsetMs(); }

            //! Set flag indicating this is an interim position update
            void setInterimFlag(bool flag) { this->m_isInterim = flag; }

            //! Get flag indicating this is an interim position update
            bool isInterim() const { return this->m_isInterim; }

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            CCallsign m_correspondingCallsign;
            BlackMisc::Geo::CCoordinateGeodetic m_position;
            BlackMisc::Aviation::CHeading  m_heading;
            BlackMisc::PhysicalQuantities::CAngle m_pitch;
            BlackMisc::PhysicalQuantities::CAngle m_bank;
            BlackMisc::PhysicalQuantities::CSpeed m_groundSpeed;
            BlackMisc::Aviation::CAltitude m_groundElevation;
            int m_isOnGround = static_cast<int>(CAircraftSituation::OnGroundSituationUnknown);
            int m_onGroundReliability = static_cast<int>(CAircraftSituation::OnGroundReliabilityNoSet);
            qint64 m_timeOffsetMs = 0;
            bool m_isInterim = false;

            BLACK_METACLASS(
                CAircraftSituation,
                BLACK_METAMEMBER(correspondingCallsign),
                BLACK_METAMEMBER(position),
                BLACK_METAMEMBER(heading),
                BLACK_METAMEMBER(pitch),
                BLACK_METAMEMBER(bank),
                BLACK_METAMEMBER(groundSpeed),
                BLACK_METAMEMBER(groundElevation),
                BLACK_METAMEMBER(isOnGround),
                BLACK_METAMEMBER(onGroundReliability),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(timeOffsetMs),
                BLACK_METAMEMBER(isInterim)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation::IsOnGround)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation::OnGroundReliability)

#endif // guard
