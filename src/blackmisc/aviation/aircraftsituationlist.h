// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTSITUATIONLIST_H
#define BLACKMISC_AVIATION_AIRCRAFTSITUATIONLIST_H

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/timestampobjectlist.h"

#include <QMetaType>
#include <QList>
#include <QPair>
#include <QHash>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Aviation, CAircraftSituation, CAircraftSituationList)

namespace BlackMisc
{
    namespace Geo
    {
        class CElevationPlane;
    }
    namespace Aviation
    {
        class CAircraftParts;

        //! List of aircraft situations
        class BLACKMISC_EXPORT CAircraftSituationList :
            public CSequence<CAircraftSituation>,
            public Geo::IGeoObjectList<CAircraftSituation, CAircraftSituationList>,
            public ITimestampWithOffsetObjectList<CAircraftSituation, CAircraftSituationList>,
            public ICallsignObjectList<CAircraftSituation, CAircraftSituationList>,
            public Mixin::MetaType<CAircraftSituationList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftSituationList)
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
            int setGroundElevationChecked(const Geo::CElevationPlane &elevationPlane, CAircraftSituation::GndElevationInfo info, qint64 newerThanAdjustedMs = -1);

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
            CAircraftSituation findClosestElevationWithinRange(const Geo::ICoordinateGeodetic &coordinate, const PhysicalQuantities::CLength &range = Geo::CElevationPlane::singlePointRadius()) const;

            //! Set on ground
            void setOnGroundInfo(const COnGroundInfo &info);

            //! Set on ground details for all situations
            void setOnGroundDetails(COnGroundInfo::OnGroundDetails details);

            //! Add an offset to each altitude
            int addAltitudeOffset(const PhysicalQuantities::CLength &offset);

            //! Latest first and no null positions?
            bool isSortedAdjustedLatestFirstWithoutNullPositions() const;

            //! Remove the first situation
            //! \remark normally used when the first situation represents the latest situation
            CAircraftSituationList withoutFrontSituation() const;

            //! All pitch values
            QList<double> pitchValues(const PhysicalQuantities::CAngleUnit &unit) const;

            //! All ground speed values
            QList<double> groundSpeedValues(const PhysicalQuantities::CSpeedUnit &unit) const;

            //! All elevation values
            QList<double> elevationValues(const PhysicalQuantities::CLengthUnit &unit) const;

            //! All altitude values
            QList<double> altitudeValues(const PhysicalQuantities::CLengthUnit &unit) const;

            //! Pitch angles standard deviation and mean
            PhysicalQuantities::CAnglePair pitchStandardDeviationAndMean() const;

            //! Ground speed standard deviation and mean
            PhysicalQuantities::CSpeedPair groundSpeedStandardDeviationAndMean() const;

            //! Transfer elevations forward from older to newer
            //! \pre requires a list which is sorted "latest first"
            int transferElevationForward(const PhysicalQuantities::CLength &radius = Geo::CElevationPlane::singlePointRadius());

            //! Average elevation for "nearby" aircraft "not/slowly moving" and having an elevation
            Geo::CElevationPlane averageElevationOfTaxiingOnGroundAircraft(const CAircraftSituation &reference, const PhysicalQuantities::CLength &range, int minValues = 1, int sufficientValues = 2) const;
        };

        //! Situation per callsign
        using CAircraftSituationPerCallsign = QHash<CCallsign, CAircraftSituation>;

        //! Situations (list) per callsign
        using CAircraftSituationListPerCallsign = QHash<CCallsign, CAircraftSituationList>;

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraftSituation>)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationPerCallsign)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationListPerCallsign)

#endif // guard
