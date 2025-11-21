// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTSITUATION_H
#define SWIFT_MISC_AVIATION_AIRCRAFTSITUATION_H

#include <array>

#include <QMetaType>
#include <QString>
#include <QVector3D>

#include "misc/aviation/aircraftvelocity.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/heading.h"
#include "misc/aviation/ongroundinfo.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/elevationplane.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/math/mathutils.h"
#include "misc/metaclass.h"
#include "misc/pq/angle.h"
#include "misc/pq/length.h"
#include "misc/pq/speed.h"
#include "misc/pq/time.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftSituation)

namespace swift::misc
{
    namespace geo
    {
        class CElevationPlane;
    }
    namespace aviation
    {
        class CAircraftParts;
        class CAircraftPartsList;
        class CAircraftLights;

        //! Value object encapsulating information of an aircraft's situation
        class SWIFT_MISC_EXPORT CAircraftSituation :
            public CValueObject<CAircraftSituation>,
            public geo::ICoordinateGeodetic,
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
                IndexIsOnGroundInfo,
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
                IndexCanLikelySkipNearGroundInterpolation
            };

            //! How was altitude corrected?
            enum AltitudeCorrection
            {
                NoCorrection,
                Underflow, //!< aircraft too low
                DraggedToGround, //!< other scenery too high, but on ground
                AGL,
                NoElevation, //!< no correction as there is no elevation
                UnknownCorrection
            };

            //! Where did we get elevation from?
            enum GndElevationInfo
            {
                // best info (most accurate) last
                NoElevationInfo,
                Test, //!< unit test
                SituationChange, //!< from swift::misc::aviation::CAircraftSituationChange
                Extrapolated, //!< extrapolated ("guessing")
                Average, //!< average value of "nearby" situation
                         //!< CAircraftSituationList::averageElevationOfNonMovingAircraft
                Interpolated, //!< interpolated between 2 elevations
                FromCache, //!< from cache
                FromProvider //!< from swift::misc::simulation::ISimulationEnvironmentProvider
            };

            //! Default constructor.
            CAircraftSituation() = default;

            //! Constructor with callsign
            CAircraftSituation(const CCallsign &correspondingCallsign);

            //! Comprehensive constructor
            CAircraftSituation(const geo::CCoordinateGeodetic &position, const CHeading &heading = {},
                               const physical_quantities::CAngle &pitch = {},
                               const physical_quantities::CAngle &bank = {}, const physical_quantities::CSpeed &gs = {},
                               const geo::CElevationPlane &groundElevation = {});

            //! Comprehensive constructor
            CAircraftSituation(const CCallsign &correspondingCallsign, const geo::CCoordinateGeodetic &position,
                               const CHeading &heading = {}, const physical_quantities::CAngle &pitch = {},
                               const physical_quantities::CAngle &bank = {}, const physical_quantities::CSpeed &gs = {},
                               const geo::CElevationPlane &groundElevation = {});

            //! \copydoc mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftSituation &compareValue) const;

            //! Get position
            const geo::CCoordinateGeodetic &getPosition() const { return m_position; }

            //! Position null?
            bool isPositionNull() const { return m_position.isNull(); }

            //! Position or altitude null?
            bool isPositionOrAltitudeNull() const { return this->isPositionNull() || this->getAltitude().isNull(); }

            //! Null situation
            bool isNull() const override;

            //! Is given info better (more accurate)?
            bool isOtherElevationInfoBetter(GndElevationInfo otherInfo, bool transferred) const;

            //! Equal pitch, bank heading
            //! \sa geo::ICoordinateGeodetic::equalNormalVectorDouble
            bool equalPbh(const CAircraftSituation &other) const;

            //! Equal PBH and vector
            //! \sa geo::ICoordinateGeodetic::equalNormalVectorDouble
            bool equalPbhAndVector(const CAircraftSituation &other) const;

            //! Equal PBH and vecto, plus altitude
            //! \sa geo::ICoordinateGeodetic::equalNormalVectorDouble
            bool equalPbhVectorAltitude(const CAircraftSituation &other) const;

            //! Equal PBH and vecto, plus altitude/elevation
            //! \sa geo::ICoordinateGeodetic::equalNormalVectorDouble
            bool equalPbhVectorAltitudeElevation(const CAircraftSituation &other) const;

            //! Set to null
            void setNull();

            //! Set position
            void setPosition(const geo::CCoordinateGeodetic &position) { m_position = position; }

            //! \copydoc geo::ICoordinateGeodetic::latitude()
            geo::CLatitude latitude() const override { return m_position.latitude(); }

            //! \copydoc geo::ICoordinateGeodetic::longitude()
            geo::CLongitude longitude() const override { return m_position.longitude(); }

            //! Is on ground?
            bool isOnGround() const { return m_onGroundInfo.getOnGround() == COnGroundInfo::OnGround; }

            //! On ground by parts?
            bool isOnGroundFromParts() const;

            //! On ground by network flag?
            bool isOnGroundFromNetwork() const;

            //! On ground info available?
            bool isOnGroundInfoAvailable() const;

            //! Should we guess on ground?
            bool shouldGuessOnGround() const;

            //! Distance to ground, null if impossible to calculate
            physical_quantities::CLength getGroundDistance(const physical_quantities::CLength &centerOfGravity) const;

            //! Do the ground details permit ground interpolation?
            bool hasGroundDetailsForGndInterpolation() const;

            //! On ground details
            void setOnGroundDetails(COnGroundInfo::OnGroundDetails details);

            //! On ground info
            aviation::COnGroundInfo getOnGroundInfo() const;

            //! Set the on ground info
            void setOnGroundInfo(const aviation::COnGroundInfo &info);

            //! \copydoc geo::ICoordinateGeodetic::geodeticHeight
            const CAltitude &geodeticHeight() const override { return m_position.geodeticHeight(); }

            //! \copydoc geo::ICoordinateGeodetic::normalVector
            QVector3D normalVector() const override { return m_position.normalVector(); }

            //! \copydoc geo::ICoordinateGeodetic::normalVectorDouble
            std::array<double, 3> normalVectorDouble() const override { return m_position.normalVectorDouble(); }

            //! Elevation of the ground directly beneath
            const CAltitude &getGroundElevation() const { return m_groundElevationPlane.getAltitude(); }

            //! Elevation of the ground directly beneath
            const geo::CElevationPlane &getGroundElevationPlane() const { return m_groundElevationPlane; }

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
            bool canTransferGroundElevation(
                const CAircraftSituation &transferToSituation,
                const physical_quantities::CLength &radius = geo::CElevationPlane::singlePointRadius()) const;

            //! Transfer from "this" situation to \c otherSituation
            //! \remark "transfer" can be used, if the positions are known, "preset" if they are still unknown
            //! \sa CAircraftSituation::interpolateGroundElevation
            //! \sa CAircraftSituation::interpolateElevation
            bool transferGroundElevationFromMe(
                CAircraftSituation &transferToSituation,
                const physical_quantities::CLength &radius = geo::CElevationPlane::singlePointRadius()) const;

            //! Transfer ground elevation from given situation (to me)
            bool transferGroundElevationToMe(const CAircraftSituation &fromSituation,
                                             const physical_quantities::CLength &radius, bool transferred);

            //! Transfer ground elevation from given situation (to me)
            bool transferGroundElevationToMe(const CAircraftSituation &fromSituation, bool transferred);

            //! Interpolate "this" elevation from the two adjacent positions
            //! \remark "transfer" can be used, if the positions are known, "preset" if they are still unknown
            //! \sa CAircraftSituation::transferGroundElevation
            //! \sa CAircraftSituation::presetGroundElevation
            bool interpolateElevation(const aviation::CAircraftSituation &oldSituation,
                                      const aviation::CAircraftSituation &newSituation);

            //! Is ground elevation value available
            bool hasGroundElevation() const;

            //! Has inbound ground details
            bool hasInboundGroundDetails() const;

            //! Elevation of the ground directly beneath at the given situation
            bool setGroundElevation(const aviation::CAltitude &altitude, GndElevationInfo info,
                                    bool transferred = false);

            //! Elevation of the ground directly beneath
            bool setGroundElevation(const geo::CElevationPlane &elevationPlane, GndElevationInfo info,
                                    bool transferred = false);

            //! Set elevation of the ground directly beneath, but checked
            //! \remark override if better
            bool setGroundElevationChecked(const geo::CElevationPlane &elevationPlane, GndElevationInfo info,
                                           bool transferred = false);

            //! Reset ground elevation
            void resetGroundElevation();

            //! Height above ground.
            physical_quantities::CLength getHeightAboveGround() const;

            //! Get heading
            const CHeading &getHeading() const { return m_heading; }

            //! Set heading
            void setHeading(const CHeading &heading);

            //! Get altitude
            const CAltitude &getAltitude() const { return m_position.geodeticHeight(); }

            //! Get altitude unit
            physical_quantities::CLengthUnit getAltitudeUnit() const { return m_position.geodeticHeight().getUnit(); }

            //! Get altitude unit
            physical_quantities::CLengthUnit getAltitudeOrDefaultUnit() const;

            //! @{
            //! Get altitude under consideration of ground elevation and ground flag
            //! \remark with dragToGround it will also compensate overflows, otherwise only underflow
            CAltitude getCorrectedAltitude(bool enableDragToGround = true,
                                           AltitudeCorrection *correction = nullptr) const;
            CAltitude getCorrectedAltitude(const physical_quantities::CLength &centerOfGravity,
                                           bool enableDragToGround = true,
                                           AltitudeCorrection *correction = nullptr) const;
            //! @}

            //! @{
            //! Set the corrected altitude from CAircraftSituation::getCorrectedAltitude
            AltitudeCorrection correctAltitude(bool enableDragToGround = true);
            AltitudeCorrection
            correctAltitude(const physical_quantities::CLength &centerOfGravity = physical_quantities::CLength::null(),
                            bool enableDragToGround = true);
            //! @}

            //! Set altitude
            void setAltitude(const CAltitude &altitude);

            //! Add offset to altitude
            CAltitude addAltitudeOffset(const physical_quantities::CLength &offset);

            //! Situation with altitude offset
            CAircraftSituation withAltitudeOffset(const physical_quantities::CLength &offset) const;

            //! Get pressure altitude
            const CAltitude &getPressureAltitude() const { return m_pressureAltitude; }

            //! Set pressure altitude
            void setPressureAltitude(const CAltitude &altitude);

            //! Get pitch
            const physical_quantities::CAngle &getPitch() const { return m_pitch; }

            //! Set pitch
            void setPitch(const physical_quantities::CAngle &pitch);

            //! Get bank (angle)
            const physical_quantities::CAngle &getBank() const { return m_bank; }

            //! Set bank (angle)
            void setBank(const physical_quantities::CAngle &bank);

            //! Set PBH values to 0 (zero)
            void setZeroPBH();

            //! Set PBH and GS values to 0 (zero)
            void setZeroPBHandGs();

            //! Get PBH info (all together)
            QString getPBHInfo() const;

            //! Set 6DOF velocity
            void setVelocity(const CAircraftVelocity &velocity)
            {
                m_velocity = velocity;
                m_hasVelocity = true;
            }

            //! Get 6DOF velocity
            const CAircraftVelocity &getVelocity() const { return m_velocity; }

            //! Is velocity non-zero?
            bool hasVelocity() const { return m_hasVelocity; }

            //! Get ground speed
            const physical_quantities::CSpeed &getGroundSpeed() const { return m_groundSpeed; }

            //! Set ground speed
            void setGroundSpeed(const physical_quantities::CSpeed &groundspeed) { m_groundSpeed = groundspeed; }

            //! Is moving? Means ground speed > epsilon
            bool isMoving() const;

            //! Situation looks like an aircraft not near ground
            bool canLikelySkipNearGroundInterpolation() const;

            //! Distance per milliseconds
            physical_quantities::CLength
            getDistancePerTime(std::chrono::milliseconds,
                               const physical_quantities::CLength &min = physical_quantities::CLength::null()) const;

            //! Distance per milliseconds (250ms)
            physical_quantities::CLength getDistancePerTime250ms(
                const physical_quantities::CLength &min = physical_quantities::CLength::null()) const;

            //! Corresponding callsign
            const CCallsign &getCallsign() const { return m_correspondingCallsign; }

            //! Has corresponding callsign
            bool hasCallsign() const { return !this->getCallsign().isEmpty(); }

            //! Corresponding callsign
            void setCallsign(const CCallsign &callsign);

            //! Get CG if any
            const physical_quantities::CLength &getCG() const { return m_cg; }

            //! Set CG
            void setCG(const physical_quantities::CLength &cg);

            //! Has CG set?
            bool hasCG() const { return !m_cg.isNull(); }

            //! Set flag indicating this is an interim position update
            void setInterimFlag(bool flag) { m_isInterim = flag; }

            //! Transfer ground flag from parts
            //! \param parts containing the gnd flag
            //! \param alwaysSetDetails mark as CAircraftSituation::InFromParts regardless of parts
            //! \param timeDeviationFactor 0..1 (of offset time) small deviations from time are accepted
            //! \param differenceMs returns time difference
            bool adjustGroundFlag(const CAircraftParts &parts, bool alwaysSetDetails, double timeDeviationFactor = 0.1,
                                  qint64 *differenceMs = nullptr);

            //! Transfer ground flag from parts list
            //! \param partsList containing the gnd flag
            //! \param alwaysSetDetails mark as CAircraftSituation::InFromParts regardless of parts
            //! \param timeDeviationFactor 0..1 (of offset time) small deviations from time are accepted
            //! \param differenceMs returns time difference
            bool adjustGroundFlag(const CAircraftPartsList &partsList, bool alwaysSetDetails,
                                  double timeDeviationFactor = 0.1, qint64 *differenceMs = nullptr);

            //! Get flag indicating this is an interim position update
            bool isInterim() const { return m_isInterim; }

            //! Enum to string
            static const QString &altitudeCorrectionToString(AltitudeCorrection correction);

            //! Means corrected altitude?
            static bool isCorrectedAltitude(AltitudeCorrection correction);

            //! Enum to string
            static const QString &gndElevationInfoToString(GndElevationInfo details);

            //! Delta distance, near to ground
            static const physical_quantities::CLength &deltaNearGround();

            //! Null situation
            static const CAircraftSituation &null();

            //! A default CG if not other value is available
            static const physical_quantities::CLength &defaultCG();

            //! \name Ground flag comparisons
            //! @{

            //! Both on ground
            static bool isGfEqualOnGround(double oldGroundFactor, double newGroundFactor)
            {
                using namespace swift::misc::math;
                return CMathUtils::epsilonEqualLimits(1.0, oldGroundFactor) &&
                       CMathUtils::epsilonEqualLimits(1.0, newGroundFactor);
            }

            //! Both not on ground
            static bool isGfEqualAirborne(double oldGroundFactor, double newGroundFactor)
            {
                using namespace swift::misc::math;
                return CMathUtils::epsilonEqualLimits(0.0, oldGroundFactor) &&
                       CMathUtils::epsilonEqualLimits(0.0, newGroundFactor);
            }

            //! Aircraft is starting
            static bool isGfStarting(double oldGroundFactor, double newGroundFactor)
            {
                using namespace swift::misc::math;
                return CMathUtils::epsilonEqualLimits(0.0, oldGroundFactor) &&
                       CMathUtils::epsilonEqualLimits(1.0, newGroundFactor);
            }

            //! Aircraft is landing
            static bool isGfLanding(double oldGroundFactor, double newGroundFactor)
            {
                using namespace swift::misc::math;
                return CMathUtils::epsilonEqualLimits(1.0, oldGroundFactor) &&
                       CMathUtils::epsilonEqualLimits(0.0, newGroundFactor);
            }
            //! @}

            //! Interpolate between the 2 situations for situation
            //! \remark NULL if there are no two elevations or threshold MaxDeltaElevationFt is exceeded
            static geo::CElevationPlane
            interpolatedElevation(const CAircraftSituation &situation, const CAircraftSituation &oldSituation,
                                  const CAircraftSituation &newSituation,
                                  const physical_quantities::CLength &distance = physical_quantities::CLength::null());

            //! Threshold until we interpolate elevations
            static constexpr double MaxDeltaElevationFt = 25.0;

            //! Within this range deviation is so small we consider values "almost constant"
            static const physical_quantities::CLength &allowedAltitudeDeviation();

            //! Guessed lights
            CAircraftLights guessLights() const;

            //! Register metadata
            static void registerMetadata();

        private:
            CCallsign m_correspondingCallsign;
            geo::CCoordinateGeodetic m_position; //!< NULL position as default
            geo::CElevationPlane m_groundElevationPlane; //!< NULL elevation as default
            aviation::CAltitude m_pressureAltitude { 0, nullptr };
            CHeading m_heading { 0, nullptr };
            physical_quantities::CAngle m_pitch { 0, nullptr };
            physical_quantities::CAngle m_bank { 0, nullptr };
            physical_quantities::CSpeed m_groundSpeed { 0, nullptr };
            physical_quantities::CLength m_cg { 0, nullptr };
            bool m_hasVelocity = false;
            CAircraftVelocity m_velocity;
            bool m_isInterim = false; //!< interim situation?
            bool m_isElvInfoTransferred = false; //!< the gnd.elevation has been transferred
            int m_elvInfo = static_cast<int>(CAircraftSituation::NoElevationInfo); //!< where did we gnd.elevation from?
            aviation::COnGroundInfo m_onGroundInfo;

            SWIFT_METACLASS(
                CAircraftSituation,
                SWIFT_METAMEMBER(correspondingCallsign),
                SWIFT_METAMEMBER(position),
                SWIFT_METAMEMBER(pressureAltitude),
                SWIFT_METAMEMBER(heading),
                SWIFT_METAMEMBER(pitch),
                SWIFT_METAMEMBER(bank),
                SWIFT_METAMEMBER(groundSpeed),
                SWIFT_METAMEMBER(cg),
                SWIFT_METAMEMBER(hasVelocity),
                SWIFT_METAMEMBER(velocity),
                SWIFT_METAMEMBER(groundElevationPlane),
                SWIFT_METAMEMBER(onGroundInfo),
                SWIFT_METAMEMBER(elvInfo),
                SWIFT_METAMEMBER(isElvInfoTransferred),
                SWIFT_METAMEMBER(timestampMSecsSinceEpoch),
                SWIFT_METAMEMBER(timeOffsetMs),
                SWIFT_METAMEMBER(isInterim));
        };
    } // namespace aviation
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituation)
Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituation::AltitudeCorrection)
Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituation::GndElevationInfo)

#endif // SWIFT_MISC_AVIATION_AIRCRAFTSITUATION_H
