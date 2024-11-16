// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTSITUATIONLIST_H
#define SWIFT_MISC_AVIATION_AIRCRAFTSITUATIONLIST_H

#include <QHash>
#include <QList>
#include <QMetaType>
#include <QPair>

#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/callsignobjectlist.h"
#include "misc/collection.h"
#include "misc/geo/elevationplane.h"
#include "misc/geo/geoobjectlist.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampobjectlist.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::aviation, CAircraftSituation, CAircraftSituationList)

namespace swift::misc
{
    namespace geo
    {
        class CElevationPlane;
    }
    namespace aviation
    {
        class CAircraftParts;

        //! List of aircraft situations
        class SWIFT_MISC_EXPORT CAircraftSituationList :
            public CSequence<CAircraftSituation>,
            public geo::IGeoObjectList<CAircraftSituation, CAircraftSituationList>,
            public ITimestampWithOffsetObjectList<CAircraftSituation, CAircraftSituationList>,
            public ICallsignObjectList<CAircraftSituation, CAircraftSituationList>,
            public mixin::MetaType<CAircraftSituationList>
        {
        public:
            SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAircraftSituationList)
            using CSequence::CSequence;

            //! Default constructor.
            CAircraftSituationList() = default;

            //! Construct from a base class object.
            CAircraftSituationList(const CSequence<CAircraftSituation> &other);

            //! Front or NULL
            CAircraftSituation frontOrNull() const;

            //! Back or NULL
            CAircraftSituation backOrNull() const;

            //! Index or NULL
            CAircraftSituation indexOrNull(int index) const;

            //! Set ground elevation from elevation plane
            int setGroundElevationChecked(const geo::CElevationPlane &elevationPlane,
                                          CAircraftSituation::GndElevationInfo info, qint64 newerThanAdjustedMs = -1);

            //! Adjust flag from parts by using CAircraftSituation::adjustGroundFlag
            int adjustGroundFlag(const CAircraftParts &parts, double timeDeviationFactor = 0.1);

            //! Contains on ground details?
            bool containsOnGroundDetails(COnGroundInfo::OnGroundDetails details) const;

            //! Contains any push back?
            //! \remark only valid for non VTOL aircraft
            bool containsPushBack() const;

            //! Are all on ground details the same?
            bool areAllOnGroundDetailsSame(COnGroundInfo::OnGroundDetails details) const;

            //! Are all situations on ground?
            bool isConstOnGround() const;

            //! Are all situations not on ground?
            bool isConstNotOnGround() const;

            //! Constantly descending?
            bool isConstDescending(bool alreadySortedLatestFirst = false) const;

            //! Constantly ascending?
            bool isConstAscending(bool alreadySortedLatestFirst = false) const;

            //! Constantly accelerating?
            bool isConstAccelerating(bool alreadySortedLatestFirst = false) const;

            //! Constantly decelarating?
            bool isConstDecelarating(bool alreadySortedLatestFirst = false) const;

            //! Is the ground flag changing for the situations
            QPair<bool, COnGroundInfo::IsOnGround> isGndFlagStableChanging(bool alreadySortedLatestFirst = false) const;

            //! Is just taking off?
            bool isJustTakingOff(bool alreadySortedLatestFirst = false) const;

            //! Is just touching down?
            bool isJustTouchingDown(bool alreadySortedLatestFirst = false) const;

            //! Is taking off?
            bool isTakingOff(bool alreadySortedLatestFirst = false) const;

            //! Is touching down?
            bool isTouchingDown(bool alreadySortedLatestFirst = false) const;

            //! Is rotating up?
            bool isRotatingUp(bool alreadySortedLatestFirst = false) const;

            //! Count the number of situations with COnGroundInfo::IsOnGround
            int countOnGround(COnGroundInfo::IsOnGround og) const;

            //! CLosest elevation within given range
            CAircraftSituation findClosestElevationWithinRange(
                const geo::ICoordinateGeodetic &coordinate,
                const physical_quantities::CLength &range = geo::CElevationPlane::singlePointRadius()) const;

            //! Set on ground
            void setOnGroundInfo(const COnGroundInfo &info);

            //! Set on ground details for all situations
            void setOnGroundDetails(COnGroundInfo::OnGroundDetails details);

            //! Add an offset to each altitude
            int addAltitudeOffset(const physical_quantities::CLength &offset);

            //! Latest first and no null positions?
            bool isSortedAdjustedLatestFirstWithoutNullPositions() const;

            //! Remove the first situation
            //! \remark normally used when the first situation represents the latest situation
            CAircraftSituationList withoutFrontSituation() const;

            //! All pitch values
            QList<double> pitchValues(const physical_quantities::CAngleUnit &unit) const;

            //! All ground speed values
            QList<double> groundSpeedValues(const physical_quantities::CSpeedUnit &unit) const;

            //! All elevation values
            QList<double> elevationValues(const physical_quantities::CLengthUnit &unit) const;

            //! All altitude values
            QList<double> altitudeValues(const physical_quantities::CLengthUnit &unit) const;

            //! Pitch angles standard deviation and mean
            physical_quantities::CAnglePair pitchStandardDeviationAndMean() const;

            //! Ground speed standard deviation and mean
            physical_quantities::CSpeedPair groundSpeedStandardDeviationAndMean() const;

            //! Transfer elevations forward from older to newer
            //! \pre requires a list which is sorted "latest first"
            int transferElevationForward(
                const physical_quantities::CLength &radius = geo::CElevationPlane::singlePointRadius());

            //! Average elevation for "nearby" aircraft "not/slowly moving" and having an elevation
            geo::CElevationPlane averageElevationOfTaxiingOnGroundAircraft(const CAircraftSituation &reference,
                                                                           const physical_quantities::CLength &range,
                                                                           int minValues = 1,
                                                                           int sufficientValues = 2) const;
        };

        //! Situation per callsign
        using CAircraftSituationPerCallsign = QHash<CCallsign, CAircraftSituation>;

        //! Situations (list) per callsign
        using CAircraftSituationListPerCallsign = QHash<CCallsign, CAircraftSituationList>;

    } // namespace aviation
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituationList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::aviation::CAircraftSituation>)
Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituationPerCallsign)
Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituationListPerCallsign)

#endif // guard
