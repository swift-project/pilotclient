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
#include "blackmisc/pq/time.h"
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
    namespace Geo { class CElevationPlane; }
    namespace Aviation
    {
        //! Value object encapsulating information of an aircraft's situation
        class BLACKMISC_EXPORT CAircraftSituation :
            public CValueObject<CAircraftSituation>,
            public Geo::ICoordinateGeodetic, public ITimestampWithOffsetBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexPosition = CPropertyIndex::GlobalIndexCAircraftSituation,
                IndexLatitude,
                IndexLongitude,
                IndexAltitude,
                IndexHeading,
                IndexBank,
                IndexIsOnGround,
                IndexIsOnGroundString,
                IndexOnGroundReliability,
                IndexOnGroundReliabilityString,
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
            enum OnGroundDetails
            {
                NotSet,
                // interpolated situation
                OnGroundByInterpolation,  //!< strongest for remote aircraft
                OnGroundByElevationAndCG,
                OnGroundByElevation,
                OnGroundByGuessing,       //!< weakest
                // received situation
                InFromNetworkSituation,   //!< received from network
                InFromParts,
                InNoGroundInfo,
                // send information
                OutOnGroundOwnAircraft    //!< sending on ground
            };

            //! Default constructor.
            CAircraftSituation();

            //! Constructor with callsign
            CAircraftSituation(const CCallsign &correspondingCallsign);

            //! Comprehensive constructor
            CAircraftSituation(const Geo::CCoordinateGeodetic &position,
                               const CHeading &heading = {},
                               const PhysicalQuantities::CAngle &pitch = {},
                               const PhysicalQuantities::CAngle &bank = {},
                               const PhysicalQuantities::CSpeed &gs = {},
                               const CAltitude &groundElevation = { 0, nullptr });

            //! Comprehensive constructor
            CAircraftSituation(const CCallsign &correspondingCallsign,
                               const Geo::CCoordinateGeodetic &position,
                               const CHeading &heading = {},
                               const PhysicalQuantities::CAngle &pitch = {},
                               const PhysicalQuantities::CAngle &bank = {},
                               const PhysicalQuantities::CSpeed &gs = {},
                               const CAltitude &groundElevation = { 0, nullptr });

            //! \copydoc Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! Compare by index
            int comparePropertyByIndex(const CPropertyIndex &index, const CAircraftSituation &compareValue) const;

            //! Get position
            const Geo::CCoordinateGeodetic &getPosition() const { return m_position; }

            //! Position null?
            bool isPositionNull() const { return m_position.isNull(); }

            //! Null situation
            virtual bool isNull() const override;

            //! Set to null
            void setNull();

            //! Set position
            void setPosition(const Geo::CCoordinateGeodetic &position) { m_position = position; }

            //! \copydoc Geo::ICoordinateGeodetic::latitude()
            virtual Geo::CLatitude latitude() const override { return m_position.latitude(); }

            //! \copydoc Geo::ICoordinateGeodetic::longitude()
            virtual Geo::CLongitude longitude() const override { return m_position.longitude(); }

            //! On ground?
            IsOnGround isOnGround() const { return static_cast<CAircraftSituation::IsOnGround>(m_isOnGround); }

            //! On ground?
            const QString &isOnGroundAsString() const;

            //! On ground info available?
            bool isOnGroundInfoAvailable() const;

            //! Set on ground
            void setOnGround(CAircraftSituation::IsOnGround onGround) { m_isOnGround = static_cast<int>(onGround); }

            //! Set on ground
            void setOnGround(CAircraftSituation::IsOnGround onGround, CAircraftSituation::OnGroundDetails reliability);

            //! On ground reliability
            OnGroundDetails getOnGroundDetails() const { return static_cast<CAircraftSituation::OnGroundDetails>(m_onGroundDetails); }

            //! On ground reliability as string
            const QString &getOnDetailsAsString() const;

            //! Reliability
            void setOnGroundReliabiliy(CAircraftSituation::OnGroundDetails onGroundReliability) { m_onGroundDetails = static_cast<int>(onGroundReliability); }

            //! On ground info as string
            QString getOnGroundInfo() const;

            //! \copydoc Geo::ICoordinateGeodetic::geodeticHeight
            const CAltitude &geodeticHeight() const override { return m_position.geodeticHeight(); }

            //! \copydoc Geo::ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override { return m_position.normalVector(); }

            //! \copydoc Geo::ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override { return m_position.normalVectorDouble(); }

            //! Elevation of the ground directly beneath
            const CAltitude &getGroundElevation() const { return m_groundElevation; }

            //! Is ground elevation value available
            bool hasGroundElevation() const;

            //! Elevation of the ground directly beneath
            void setGroundElevation(const CAltitude &elevation) { m_groundElevation = elevation; }

            //! Set elevation of the ground directly beneath, but checked
            bool setGroundElevationChecked(const CAltitude &elevation, bool ignoreNullValues = true, bool overrideExisting = true);

            //! Set elevation of the ground directly beneath, but checked
            bool setGroundElevationChecked(const Geo::CElevationPlane &elevationPlane, bool ignoreNullValues = true, bool overrideExisting = true);

            //! Height above ground.
            PhysicalQuantities::CLength getHeightAboveGround() const;

            //! Get heading
            const CHeading &getHeading() const { return m_heading; }

            //! Set heading
            void setHeading(const CHeading &heading) { m_heading = heading; }

            //! Get altitude
            const CAltitude &getAltitude() const { return m_position.geodeticHeight(); }

            //! Get altitude under consideration of ground elevation
            CAltitude getCorrectedAltitude(const PhysicalQuantities::CLength &centerOfGravity = {}, bool *corrected = nullptr) const;

            //! Set altitude
            void setAltitude(const CAltitude &altitude) { m_position.setGeodeticHeight(altitude); }

            //! Get pressure altitude
            const CAltitude &getPressureAltitude() const { return m_pressureAltitude; }

            //! Set pressure altitude
            void setPressureAltitude(const CAltitude &altitude);

            //! Get pitch
            const PhysicalQuantities::CAngle &getPitch() const { return m_pitch; }

            //! Set pitch
            void setPitch(const PhysicalQuantities::CAngle &pitch) { m_pitch = pitch; }

            //! Get bank (angle)
            const PhysicalQuantities::CAngle &getBank() const { return m_bank; }

            //! Set bank (angle)
            void setBank(const PhysicalQuantities::CAngle &bank) { m_bank = bank; }

            //! Get ground speed
            const PhysicalQuantities::CSpeed &getGroundSpeed() const { return m_groundSpeed; }

            //! Set ground speed
            void setGroundSpeed(const PhysicalQuantities::CSpeed &groundspeed) { m_groundSpeed = groundspeed; }

            //! Distance per time
            PhysicalQuantities::CLength getDistancePerTime(const PhysicalQuantities::CTime &time) const;

            //! Distance per milliseconds
            PhysicalQuantities::CLength getDistancePerTime(int milliseconds) const;

            //! Corresponding callsign
            const CCallsign &getCallsign() const { return m_correspondingCallsign; }

            //! Corresponding callsign
            void setCallsign(const CCallsign &callsign);

            //! Set flag indicating this is an interim position update
            void setInterimFlag(bool flag) { m_isInterim = flag; }

            //! Get flag indicating this is an interim position update
            bool isInterim() const { return m_isInterim; }

            //! \copydoc Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Enum to string
            static const QString &isOnGroundToString(IsOnGround onGround);

            //! Enum to string
            static const QString &onGroundDetailsToString(OnGroundDetails reliability);

        private:
            CCallsign m_correspondingCallsign;
            Geo::CCoordinateGeodetic m_position; // NULL position
            Aviation::CAltitude m_pressureAltitude { 0, nullptr };
            CHeading m_heading;
            PhysicalQuantities::CAngle m_pitch;
            PhysicalQuantities::CAngle m_bank;
            PhysicalQuantities::CSpeed m_groundSpeed;
            CAltitude m_groundElevation{ 0, CAltitude::MeanSeaLevel, PhysicalQuantities::CLengthUnit::nullUnit() };
            int m_isOnGround = static_cast<int>(CAircraftSituation::OnGroundSituationUnknown);
            int m_onGroundDetails = static_cast<int>(CAircraftSituation::NotSet);
            bool m_isInterim = false;

            BLACK_METACLASS(
                CAircraftSituation,
                BLACK_METAMEMBER(correspondingCallsign),
                BLACK_METAMEMBER(position),
                BLACK_METAMEMBER(pressureAltitude),
                BLACK_METAMEMBER(heading),
                BLACK_METAMEMBER(pitch),
                BLACK_METAMEMBER(bank),
                BLACK_METAMEMBER(groundSpeed),
                BLACK_METAMEMBER(groundElevation),
                BLACK_METAMEMBER(isOnGround),
                BLACK_METAMEMBER(onGroundDetails),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(timeOffsetMs),
                BLACK_METAMEMBER(isInterim)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation::IsOnGround)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation::OnGroundDetails)

#endif // guard
