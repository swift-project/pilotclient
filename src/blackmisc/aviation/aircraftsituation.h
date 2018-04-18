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
#include "blackmisc/geo/elevationplane.h"
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
        class CAircraftParts;
        class CAircraftPartsList;

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
                IndexGroundElevationPlane,
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
                NotSetGroundDetails,
                // interpolated situation
                OnGroundByInterpolation,  //!< strongest for remote aircraft
                OnGroundByElevationAndCG,
                OnGroundByElevation,
                OnGroundByGuessing,       //!< weakest
                // received situation
                InFromNetwork,            //!< received from network
                InFromParts,              //!< set from aircraft parts
                InNoGroundInfo,           //!< not know
                // send information
                OutOnGroundOwnAircraft    //!< sending on ground
            };

            //! How was altitude corrected
            enum AltitudeCorrection
            {
                NoCorrection,
                Underflow,
                DraggedToGround,
                AGL,
                NoElevation,
                UnknownCorrection
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
                               const Geo::CElevationPlane &groundElevation = {});

            //! Comprehensive constructor
            CAircraftSituation(const CCallsign &correspondingCallsign,
                               const Geo::CCoordinateGeodetic &position,
                               const CHeading &heading = {},
                               const PhysicalQuantities::CAngle &pitch = {},
                               const PhysicalQuantities::CAngle &bank = {},
                               const PhysicalQuantities::CSpeed &gs = {},
                               const Geo::CElevationPlane &groundElevation = {});

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
            IsOnGround getOnGround() const { return static_cast<CAircraftSituation::IsOnGround>(m_onGround); }

            //! Is on ground?
            bool isOnGround() const { return this->getOnGround() == OnGround; }

            //! On ground?
            const QString &onGroundAsString() const;

            //! On ground info available?
            bool isOnGroundInfoAvailable() const;

            //! Set on ground
            void setOnGround(bool onGround);

            //! Set on ground
            void setOnGround(CAircraftSituation::IsOnGround onGround);

            //! Set on ground
            void setOnGround(CAircraftSituation::IsOnGround onGround, CAircraftSituation::OnGroundDetails details);

            //! On ground factor 0..1 (on ground), -1 not set
            double getOnGroundFactor() const { return m_onGroundFactor; }

            //! Set on ground factor 0..1 (on ground), -1 not set
            void setOnGroundFactor(double groundFactor);

            //! Should we guess on ground?
            bool shouldGuessOnGround() const;

            //! Guess on ground flag
            bool guessOnGround(bool vtol = false, const PhysicalQuantities::CLength &cg = PhysicalQuantities::CLength::null());

            //! Distance to ground, null if impossible to calculate
            PhysicalQuantities::CLength getGroundDistance(const PhysicalQuantities::CLength &centerOfGravity) const;

            //! On ground reliability
            OnGroundDetails getOnGroundDetails() const { return static_cast<CAircraftSituation::OnGroundDetails>(m_onGroundDetails); }

            //! Do the ground details permit ground interpolation?
            bool hasGroundDetailsForGndInterpolation() const;

            //! On ground reliability as string
            const QString &getOnDetailsAsString() const;

            //! On ground details
            bool setOnGroundDetails(CAircraftSituation::OnGroundDetails details);

            //! Set on ground as interpolated from ground fatcor
            bool setOnGroundFromGroundFactorFromInterpolation(double threshold = 0.5);

            //! Set on ground by underflow detection, detects below ground scenarios
            bool setOnGroundByUnderflowDetection(const PhysicalQuantities::CLength &cg);

            //! On ground info as string
            QString getOnGroundInfo() const;

            //! \copydoc Geo::ICoordinateGeodetic::geodeticHeight
            const CAltitude &geodeticHeight() const override { return m_position.geodeticHeight(); }

            //! \copydoc Geo::ICoordinateGeodetic::normalVector
            virtual QVector3D normalVector() const override { return m_position.normalVector(); }

            //! \copydoc Geo::ICoordinateGeodetic::normalVectorDouble
            virtual std::array<double, 3> normalVectorDouble() const override { return m_position.normalVectorDouble(); }

            //! Elevation of the ground directly beneath
            const CAltitude &getGroundElevation() const { return m_groundElevationPlane.getAltitude(); }

            //! Elevation of the ground directly beneath
            const Geo::CElevationPlane &getGroundElevationPlane() const { return m_groundElevationPlane; }

            //! Is on ground by elevation data, requires elevation and CG
            IsOnGround isOnGroundByElevation(const PhysicalQuantities::CLength &cg) const;

            //! Is ground elevation value available
            bool hasGroundElevation() const;

            //! Has inbound ground details
            bool hasInboundGroundDetails() const;

            //! Elevation of the ground directly beneath at the given situation
            void setGroundElevation(const Aviation::CAltitude &altitude);

            //! Elevation of the ground directly beneath
            void setGroundElevation(const Geo::CElevationPlane &elevationPlane) { m_groundElevationPlane = elevationPlane; }

            //! Set elevation of the ground directly beneath, but checked
            //! \remark override if better
            bool setGroundElevationChecked(const Geo::CElevationPlane &elevationPlane);

            //! Distance of ground elevation
            const PhysicalQuantities::CLength &getGroundElevationDistance() const;

            //! Height above ground.
            PhysicalQuantities::CLength getHeightAboveGround() const;

            //! Get heading
            const CHeading &getHeading() const { return m_heading; }

            //! Set heading
            void setHeading(const CHeading &heading) { m_heading = heading; }

            //! Get altitude
            const CAltitude &getAltitude() const { return m_position.geodeticHeight(); }

            //! Get altitude unit
            const PhysicalQuantities::CLengthUnit &getAltitudeUnit() const { return m_position.geodeticHeight().getUnit(); }

            //! Get altitude under consideration of ground elevation and ground flag
            //! \remark with dragToGround it will also compensate overflows, otherwise only underflow
            CAltitude getCorrectedAltitude(const PhysicalQuantities::CLength &centerOfGravity = PhysicalQuantities::CLength::null(), bool enableDragToGround = true, AltitudeCorrection *correctetion = nullptr) const;

            //! Set the corrected altitude from CAircraftSituation::getCorrectedAltitude
            AltitudeCorrection correctAltitude(const PhysicalQuantities::CLength &centerOfGravity = PhysicalQuantities::CLength::null(), bool enableDragToGround = true);

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

            //! Is moving? Means ground speed > epsilon
            bool isMoving() const;

            //! Situation looks like an aircraft not near ground
            bool canLikelySkipNearGroundInterpolation() const;

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

            //! Transfer ground flag from parts
            //! \param parts containing the gnd flag
            //! \param alwaysSetDetails mark as CAircraftSituation::InFromParts regardless of parts
            //! \param timeDeviationFactor 0..1 (of offset time) small deviations from time are accepted
            //! \param differenceMs returns time difference
            bool adjustGroundFlag(const CAircraftParts &parts, bool alwaysSetDetails, double timeDeviationFactor = 0.1, qint64 *differenceMs = nullptr);

            //! Transfer ground flag from parts list
            //! \param partsList containing the gnd flag
            //! \param alwaysSetDetails mark as CAircraftSituation::InFromParts regardless of parts
            //! \param timeDeviationFactor 0..1 (of offset time) small deviations from time are accepted
            //! \param differenceMs returns time difference
            bool adjustGroundFlag(const CAircraftPartsList &partsList, bool alwaysSetDetails, double timeDeviationFactor = 0.1, qint64 *differenceMs = nullptr);

            //! Get flag indicating this is an interim position update
            bool isInterim() const { return m_isInterim; }

            //! \copydoc Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Enum to string
            static const QString &isOnGroundToString(IsOnGround onGround);

            //! Enum to string
            static const QString &onGroundDetailsToString(OnGroundDetails reliability);

            //! Enum to string
            static const QString &altitudeCorrectionToString(AltitudeCorrection correction);

            //! Delta distance, near to ground
            static const PhysicalQuantities::CLength &deltaNearGround();

            //! Null situation
            static const CAircraftSituation &null();

            //! A default CG if not other value is available
            static const PhysicalQuantities::CLength &defaultCG();

        private:
            CCallsign m_correspondingCallsign;
            Geo::CCoordinateGeodetic m_position; //!< NULL position as default
            Aviation::CAltitude m_pressureAltitude { 0, nullptr };
            CHeading m_heading { 0, nullptr };
            PhysicalQuantities::CAngle m_pitch { 0, nullptr };
            PhysicalQuantities::CAngle m_bank  { 0, nullptr };
            PhysicalQuantities::CSpeed m_groundSpeed { 0, nullptr };
            bool m_isInterim = false;
            Geo::CElevationPlane m_groundElevationPlane; //!< NULL elevation as default
            int m_onGround = static_cast<int>(CAircraftSituation::OnGroundSituationUnknown);
            int m_onGroundDetails = static_cast<int>(CAircraftSituation::NotSetGroundDetails);
            double m_onGroundFactor = -1; //!< interpolated ground flag, 1..on ground, 0..not on ground, -1 no info

            BLACK_METACLASS(
                CAircraftSituation,
                BLACK_METAMEMBER(correspondingCallsign),
                BLACK_METAMEMBER(position),
                BLACK_METAMEMBER(pressureAltitude),
                BLACK_METAMEMBER(heading),
                BLACK_METAMEMBER(pitch),
                BLACK_METAMEMBER(bank),
                BLACK_METAMEMBER(groundSpeed),
                BLACK_METAMEMBER(groundElevationPlane),
                BLACK_METAMEMBER(onGround),
                BLACK_METAMEMBER(onGroundDetails),
                BLACK_METAMEMBER(onGroundFactor),
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
