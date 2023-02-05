/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTSITUATION_H
#define BLACKMISC_AVIATION_AIRCRAFTSITUATION_H

#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/aviation/aircraftvelocity.h"
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
#include "blackmisc/propertyindexref.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <array>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CAircraftSituation)

namespace BlackMisc
{
    namespace Geo { class CElevationPlane; }
    namespace Aviation
    {
        class CAircraftParts;
        class CAircraftPartsList;
        class CAircraftLights;

        //! Value object encapsulating information of an aircraft's situation
        class BLACKMISC_EXPORT CAircraftSituation :
            public CValueObject<CAircraftSituation>,
            public Geo::ICoordinateGeodetic,
            public ITimestampWithOffsetBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexPosition = CPropertyIndexRef::GlobalIndexCAircraftSituation,
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
                IndexPBHInfo,
                IndexVelocity,
                IndexGroundSpeed,
                IndexGroundElevationPlane,
                IndexGroundElevationInfo,
                IndexGroundElevationInfoTransferred,
                IndexGroundElevationInfoString,
                IndexGroundElevationPlusInfo,
                IndexCallsign,
                IndexCG,
                IndexSceneryOffset,
                IndexCanLikelySkipNearGroundInterpolation
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
                InNoGroundInfo,           //!< not known
                // send information
                OutOnGroundOwnAircraft    //!< sending on ground
            };

            //! How was altitude corrected?
            enum AltitudeCorrection
            {
                NoCorrection,
                Underflow,       //!< aircraft too low
                DraggedToGround, //!< other scenery too high, but on ground
                AGL,
                NoElevation,     //!< no correction as there is no elevation
                UnknownCorrection
            };

            //! Where did we get elevation from?
            enum GndElevationInfo
            {
                // best info (most accurate) last
                NoElevationInfo,
                Test,                //!< unit test
                SituationChange,     //!< from BlackMisc::Aviation::CAircraftSituationChange
                Extrapolated,        //!< extrapolated ("guessing")
                Average,             //!< average value of "nearby" situation CAircraftSituationList::averageElevationOfNonMovingAircraft
                Interpolated,        //!< interpolated between 2 elevations
                FromCache,           //!< from cache
                FromProvider         //!< from BlackMisc::Simulation::ISimulationEnvironmentProvider
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

            //! \copydoc Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc Mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftSituation &compareValue) const;

            //! Get position
            const Geo::CCoordinateGeodetic &getPosition() const { return m_position; }

            //! Position null?
            bool isPositionNull() const { return m_position.isNull(); }

            //! Position or altitude null?
            bool isPositionOrAltitudeNull() const { return this->isPositionNull() || this->getAltitude().isNull(); }

            //! Null situation
            virtual bool isNull() const override;

            //! Is given info better (more accurate)?
            bool isOtherElevationInfoBetter(GndElevationInfo otherInfo, bool transferred) const;

            //! Equal pitch, bank heading
            //! \sa Geo::ICoordinateGeodetic::equalNormalVectorDouble
            bool equalPbh(const CAircraftSituation &other) const;

            //! Equal PBH and vector
            //! \sa Geo::ICoordinateGeodetic::equalNormalVectorDouble
            bool equalPbhAndVector(const CAircraftSituation &other) const;

            //! Equal PBH and vecto, plus altitude
            //! \sa Geo::ICoordinateGeodetic::equalNormalVectorDouble
            bool equalPbhVectorAltitude(const CAircraftSituation &other) const;

            //! Equal PBH and vecto, plus altitude/elevation
            //! \sa Geo::ICoordinateGeodetic::equalNormalVectorDouble
            bool equalPbhVectorAltitudeElevation(const CAircraftSituation &other) const;

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

            //! On ground by parts?
            bool isOnGroundFromParts() const;

            //! On ground by network flag?
            bool isOnGroundFromNetwork() const;

            //! On ground?
            const QString &onGroundAsString() const;

            //! On ground info available?
            bool isOnGroundInfoAvailable() const;

            //! Set on ground
            bool setOnGround(bool onGround);

            //! Set on ground
            bool setOnGround(CAircraftSituation::IsOnGround onGround);

            //! Set on ground
            bool setOnGround(CAircraftSituation::IsOnGround onGround, CAircraftSituation::OnGroundDetails details);

            //! On ground factor 0..1 (on ground), -1 not set
            double getOnGroundFactor() const { return m_onGroundFactor; }

            //! Set on ground factor 0..1 (on ground), -1 not set
            void setOnGroundFactor(double groundFactor);

            //! Should we guess on ground?
            bool shouldGuessOnGround() const;

            //! Distance to ground, null if impossible to calculate
            PhysicalQuantities::CLength getGroundDistance(const PhysicalQuantities::CLength &centerOfGravity) const;

            //! On ground reliability
            OnGroundDetails getOnGroundDetails() const { return static_cast<CAircraftSituation::OnGroundDetails>(m_onGroundDetails); }

            //! Do the ground details permit ground interpolation?
            bool hasGroundDetailsForGndInterpolation() const;

            //! On ground reliability as string
            const QString &getOnGroundDetailsAsString() const;

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

            //! Distance of coordinates of situation to coordinates of elevation plane
            PhysicalQuantities::CLength getGroundElevationDistance() const;

            //! How did we get gnd.elevation?
            GndElevationInfo getGroundElevationInfo() const;

            //! How did we get gnd.elevation?
            QString getGroundElevationInfoAsString() const;

            //! Ground elevation plus info
            QString getGroundElevationAndInfo() const;

            //! Is the elv.info transferred?
            bool isGroundElevationInfoTransferred() const { return m_isElvInfoTransferred; }

            //! How we did get gnd.elevation
            void setGroundElevationInfo(GndElevationInfo details) { m_elvInfo = static_cast<int>(details); }

            //! Can the elevation be transferred to another situation?
            bool canTransferGroundElevation(const CAircraftSituation &transferToSituation, const PhysicalQuantities::CLength &radius = Geo::CElevationPlane::singlePointRadius()) const;

            //! Transfer from "this" situation to \c otherSituation
            //! \remark "transfer" can be used, if the positions are known, "preset" if they are still unknown
            //! \sa CAircraftSituation::interpolateGroundElevation
            //! \sa CAircraftSituation::interpolateElevation
            bool transferGroundElevationFromMe(CAircraftSituation &transferToSituation, const PhysicalQuantities::CLength &radius = Geo::CElevationPlane::singlePointRadius()) const;

            //! Transfer ground elevation from given situation (to me)
            bool transferGroundElevationToMe(const CAircraftSituation &fromSituation, const PhysicalQuantities::CLength &radius, bool transferred);

            //! Transfer ground elevation from given situation (to me)
            bool transferGroundElevationToMe(const CAircraftSituation &fromSituation, bool transferred);

            //! Interpolate "this" elevation from the two adjacent positions
            //! \remark "transfer" can be used, if the positions are known, "preset" if they are still unknown
            //! \sa CAircraftSituation::transferGroundElevation
            //! \sa CAircraftSituation::presetGroundElevation
            bool interpolateElevation(const Aviation::CAircraftSituation &oldSituation, const Aviation::CAircraftSituation &newSituation);

            //! @{
            //! Is on ground by elevation data, requires elevation and CG
            IsOnGround isOnGroundByElevation() const;
            IsOnGround isOnGroundByElevation(const PhysicalQuantities::CLength &cg) const;
            //! @}

            //! Is ground elevation value available
            bool hasGroundElevation() const;

            //! Has inbound ground details
            bool hasInboundGroundDetails() const;

            //! Elevation of the ground directly beneath at the given situation
            bool setGroundElevation(const Aviation::CAltitude &altitude, GndElevationInfo info, bool transferred = false);

            //! Elevation of the ground directly beneath
            bool setGroundElevation(const Geo::CElevationPlane &elevationPlane, GndElevationInfo info, bool transferred = false);

            //! Set elevation of the ground directly beneath, but checked
            //! \remark override if better
            bool setGroundElevationChecked(const Geo::CElevationPlane &elevationPlane, GndElevationInfo info, bool transferred = false);

            //! Reset ground elevation
            void resetGroundElevation();

            //! Distance of ground elevation
            const PhysicalQuantities::CLength &getGroundElevationRadius() const;

            //! Height above ground.
            PhysicalQuantities::CLength getHeightAboveGround() const;

            //! Get heading
            const CHeading &getHeading() const { return m_heading; }

            //! Set heading
            void setHeading(const CHeading &heading);

            //! Get altitude
            const CAltitude &getAltitude() const { return m_position.geodeticHeight(); }

            //! Get altitude unit
            PhysicalQuantities::CLengthUnit getAltitudeUnit() const { return m_position.geodeticHeight().getUnit(); }

            //! Get altitude unit
            PhysicalQuantities::CLengthUnit getAltitudeOrDefaultUnit() const;

            //! @{
            //! Get altitude under consideration of ground elevation and ground flag
            //! \remark with dragToGround it will also compensate overflows, otherwise only underflow
            CAltitude getCorrectedAltitude(bool enableDragToGround = true, AltitudeCorrection *correction = nullptr) const;
            CAltitude getCorrectedAltitude(const PhysicalQuantities::CLength &centerOfGravity, bool enableDragToGround = true, AltitudeCorrection *correction = nullptr) const;
            //! @}

            //! @{
            //! Set the corrected altitude from CAircraftSituation::getCorrectedAltitude
            AltitudeCorrection correctAltitude(bool enableDragToGround = true);
            AltitudeCorrection correctAltitude(const PhysicalQuantities::CLength &centerOfGravity = PhysicalQuantities::CLength::null(), bool enableDragToGround = true);
            //! @}

            //! Set altitude
            void setAltitude(const CAltitude &altitude);

            //! Add offset to altitude
            CAltitude addAltitudeOffset(const PhysicalQuantities::CLength &offset);

            //! Situation with altitude offset
            CAircraftSituation withAltitudeOffset(const PhysicalQuantities::CLength &offset) const;

            //! Get pressure altitude
            const CAltitude &getPressureAltitude() const { return m_pressureAltitude; }

            //! Set pressure altitude
            void setPressureAltitude(const CAltitude &altitude);

            //! Get pitch
            const PhysicalQuantities::CAngle &getPitch() const { return m_pitch; }

            //! Set pitch
            void setPitch(const PhysicalQuantities::CAngle &pitch);

            //! Get bank (angle)
            const PhysicalQuantities::CAngle &getBank() const { return m_bank; }

            //! Set bank (angle)
            void setBank(const PhysicalQuantities::CAngle &bank);

            //! Set PBH values to 0 (zero)
            void setZeroPBH();

            //! Set PBH and GS values to 0 (zero)
            void setZeroPBHandGs();

            //! Get PBH info (all together)
            QString getPBHInfo() const;

            //! Set 6DOF velocity
            void setVelocity(const CAircraftVelocity &velocity) { m_velocity = velocity; m_hasVelocity = true; }

            //! Get 6DOF velocity
            const CAircraftVelocity &getVelocity() const { return m_velocity; }

            //! Is velocity non-zero?
            bool hasVelocity() const { return m_hasVelocity; }

            //! Get ground speed
            const PhysicalQuantities::CSpeed &getGroundSpeed() const { return m_groundSpeed; }

            //! Set ground speed
            void setGroundSpeed(const PhysicalQuantities::CSpeed &groundspeed) { m_groundSpeed = groundspeed; }

            //! Is moving? Means ground speed > epsilon
            bool isMoving() const;

            //! Situation looks like an aircraft not near ground
            bool canLikelySkipNearGroundInterpolation() const;

            //! Distance per time
            PhysicalQuantities::CLength getDistancePerTime(const PhysicalQuantities::CTime &time, const PhysicalQuantities::CLength &min = PhysicalQuantities::CLength::null()) const;

            //! Distance per milliseconds
            PhysicalQuantities::CLength getDistancePerTime(int milliseconds, const PhysicalQuantities::CLength &min = PhysicalQuantities::CLength::null()) const;

            //! Distance per milliseconds (250ms)
            PhysicalQuantities::CLength getDistancePerTime250ms(const PhysicalQuantities::CLength &min = PhysicalQuantities::CLength::null()) const;

            //! Corresponding callsign
            const CCallsign &getCallsign() const { return m_correspondingCallsign; }

            //! Has corresponding callsign
            bool hasCallsign() const { return !this->getCallsign().isEmpty(); }

            //! Corresponding callsign
            void setCallsign(const CCallsign &callsign);

            //! Get CG if any
            const PhysicalQuantities::CLength &getCG() const { return m_cg; }

            //! Set CG
            void setCG(const PhysicalQuantities::CLength &cg);

            //! Has CG set?
            bool hasCG() const { return !m_cg.isNull(); }

            //! Get scenery offset if any
            const PhysicalQuantities::CLength &getSceneryOffset() const { return m_sceneryOffset; }

            //! Get scenery offset if any or zero ("0")
            const PhysicalQuantities::CLength &getSceneryOffsetOrZero() const;

            //! Set scenery offset
            void setSceneryOffset(const PhysicalQuantities::CLength &sceneryOffset);

            //! Has scenery offset?
            bool hasSceneryOffset() const { return !m_sceneryOffset.isNull(); }

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

            //! Enum to string
            static const QString &isOnGroundToString(IsOnGround onGround);

            //! Enum to string
            static const QString &onGroundDetailsToString(OnGroundDetails reliability);

            //! Enum to string
            static const QString &altitudeCorrectionToString(AltitudeCorrection correction);

            //! Means corrected altitude?
            static bool isCorrectedAltitude(AltitudeCorrection correction);

            //! Enum to string
            static const QString &gndElevationInfoToString(GndElevationInfo details);

            //! Delta distance, near to ground
            static const PhysicalQuantities::CLength &deltaNearGround();

            //! Null situation
            static const CAircraftSituation &null();

            //! A default CG if not other value is available
            static const PhysicalQuantities::CLength &defaultCG();

            //! \name Ground flag comparisons
            //! @{

            //! Both on ground
            static bool isGfEqualOnGround(double oldGroundFactor, double newGroundFactor)
            {
                return isDoubleEpsilonEqual(1.0, oldGroundFactor) && isDoubleEpsilonEqual(1.0, newGroundFactor);
            }

            //! Both not on ground
            static bool isGfEqualAirborne(double oldGroundFactor, double newGroundFactor)
            {
                return isDoubleEpsilonEqual(0.0, oldGroundFactor) && isDoubleEpsilonEqual(0.0, newGroundFactor);
            }

            //! Aircraft is starting
            static bool isGfStarting(double oldGroundFactor, double newGroundFactor)
            {
                return isDoubleEpsilonEqual(0.0, oldGroundFactor) && isDoubleEpsilonEqual(1.0, newGroundFactor);
            }

            //! Aircraft is landing
            static bool isGfLanding(double oldGroundFactor, double newGroundFactor)
            {
                return isDoubleEpsilonEqual(1.0, oldGroundFactor) && isDoubleEpsilonEqual(0.0, newGroundFactor);
            }
            //! @}

            //! Interpolate between the 2 situations for situation
            //! \remark NULL if there are no two elevations or threshold MaxDeltaElevationFt is exceeded
            static Geo::CElevationPlane interpolatedElevation(const CAircraftSituation &situation, const CAircraftSituation &oldSituation, const CAircraftSituation &newSituation, const PhysicalQuantities::CLength &distance = PhysicalQuantities::CLength::null());

            //! Threshold until we interpolate elevations
            static constexpr double MaxDeltaElevationFt = 25.0;

            //! Within this range deviation is so small we consider values "almost constant"
            static const PhysicalQuantities::CLength &allowedAltitudeDeviation();

            //! Guessed lights
            CAircraftLights guessLights() const;

            //! Register metadata
            static void registerMetadata();

        private:
            CCallsign m_correspondingCallsign;
            Geo::CCoordinateGeodetic m_position; //!< NULL position as default
            Geo::CElevationPlane m_groundElevationPlane; //!< NULL elevation as default
            Aviation::CAltitude m_pressureAltitude      { 0, nullptr };
            CHeading m_heading                          { 0, nullptr };
            PhysicalQuantities::CAngle  m_pitch         { 0, nullptr };
            PhysicalQuantities::CAngle  m_bank          { 0, nullptr };
            PhysicalQuantities::CSpeed  m_groundSpeed   { 0, nullptr };
            PhysicalQuantities::CLength m_cg            { 0, nullptr };
            PhysicalQuantities::CLength m_sceneryOffset { 0, nullptr };
            bool m_hasVelocity = false;
            CAircraftVelocity m_velocity;
            bool m_isInterim = false; //!< interim situation?
            bool m_isElvInfoTransferred = false; //!< the gnd.elevation has been transferred
            int m_onGround = static_cast<int>(CAircraftSituation::OnGroundSituationUnknown);
            int m_onGroundDetails = static_cast<int>(CAircraftSituation::NotSetGroundDetails);
            int m_elvInfo = static_cast<int>(CAircraftSituation::NoElevationInfo); //!< where did we gnd.elevation from?
            double m_onGroundFactor = -1;      //!< interpolated ground flag, 1..on ground, 0..not on ground, -1 no info
            QString m_onGroundGuessingDetails; //!< only for debugging, not transferred via DBus etc.

            //! Equal double values?
            static bool isDoubleEpsilonEqual(double d1, double d2)
            {
                return qAbs(d1 - d2) <= std::numeric_limits<double>::epsilon();
            }

            BLACK_METACLASS(
                CAircraftSituation,
                BLACK_METAMEMBER(correspondingCallsign),
                BLACK_METAMEMBER(position),
                BLACK_METAMEMBER(pressureAltitude),
                BLACK_METAMEMBER(heading),
                BLACK_METAMEMBER(pitch),
                BLACK_METAMEMBER(bank),
                BLACK_METAMEMBER(groundSpeed),
                BLACK_METAMEMBER(cg),
                BLACK_METAMEMBER(sceneryOffset),
                BLACK_METAMEMBER(hasVelocity),
                BLACK_METAMEMBER(velocity),
                BLACK_METAMEMBER(groundElevationPlane),
                BLACK_METAMEMBER(onGround),
                BLACK_METAMEMBER(onGroundDetails),
                BLACK_METAMEMBER(elvInfo),
                BLACK_METAMEMBER(isElvInfoTransferred),
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
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation::AltitudeCorrection)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituation::GndElevationInfo)

#endif // guard
