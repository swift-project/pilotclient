/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTSITUATIONCHANGE_H
#define BLACKMISC_AVIATION_AIRCRAFTSITUATIONCHANGE_H

#include "callsign.h"
#include "altitude.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"
#include <QPair>

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftSituation;
        class CAircraftSituationList;

        //! Value object about changes in situations
        class BLACKMISC_EXPORT CAircraftSituationChange :
            public CValueObject<CAircraftSituationChange>,
            public ITimestampWithOffsetBased
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexCallsign = CPropertyIndex::GlobalIndexCAircraftSituationChange,
                IndexIsNull,
                IndexSituationsCount,
                IndexConstAscending,
                IndexConstDescending,
                IndexConstOnGround,
                IndexConstNotOnGround,
                IndexJustTakingOff,
                IndexJustTouchingDown,
                IndexRotatingUp,
                IndexContainsPushBack
            };

            //! Default constructor.
            CAircraftSituationChange();

            //! Ctor with 2 situations
            CAircraftSituationChange(const CAircraftSituation &s1, const CAircraftSituation &s2);

            //! Ctor with n situations
            CAircraftSituationChange(const CAircraftSituationList &situations, bool alreadySortedLatestFirst = false, bool calcStdDeviations = false);

            //! Get callsign
            const CCallsign &getCallsign() const { return m_correspondingCallsign; }

            //! Null?
            bool isNull() const { return m_situationsCount < 2; } // we need at least 2 situations

            //! Basend on n situations
            int getSituationsCount() const { return m_situationsCount; }

            //! \copydoc BlackMisc::Aviation::CAircraftSituationList::isConstAscending
            bool isConstAscending() const { return m_constAscending; }

            //! \copydoc BlackMisc::Aviation::CAircraftSituationList::isConstDescending
            bool isConstDescending() const { return m_constDescending; }

            //! \copydoc BlackMisc::Aviation::CAircraftSituationList::isConstOnGround
            bool isConstOnGround() const { return m_constOnGround; }

            //! Was on ground (without latest situation)?
            bool wasConstOnGround() const { return m_wasOnGround; }

            //! \copydoc BlackMisc::Aviation::CAircraftSituationList::isConstNotOnGround
            bool isConstNotOnGround() const { return m_constNotOnGround; }

            //! Was not on ground (without latest situation)?
            bool wasConstNotOnGround() const { return m_wasNotOnGround; }

            //! \copydoc BlackMisc::Aviation::CAircraftSituationList::isConstAccelerating
            bool isConstAccelerating() const { return m_constAccelerating; }

            //! \copydoc BlackMisc::Aviation::CAircraftSituationList::isConstDecelarating
            bool isConstDecelarating() const { return m_constAccelerating; }

            //! \copydoc BlackMisc::Aviation::CAircraftSituationList::isJustTakingOff
            bool isJustTakingOff() const { return m_justTakeoff; }

            //! \copydoc BlackMisc::Aviation::CAircraftSituationList::isJustTouchingDown
            bool isJustTouchingDown() const { return m_justTouchdown; }

            //! \copydoc BlackMisc::Aviation::CAircraftSituationList::isRotatingUp
            bool isRotatingUp() const { return m_rotateUp; }

            //! \copydoc BlackMisc::Aviation::CAircraftSituationList::containsPushBack
            bool containsPushBack() const { return m_containsPushBack; }

            //! AGL if it can be calculated, otherwise NULL
            //! \note distance is without CG, so on ground it can also be used to calculate
            QPair<PhysicalQuantities::CLength, PhysicalQuantities::CLength> getAltAglStdDevAndMean() const { return QPair<PhysicalQuantities::CLength, PhysicalQuantities::CLength>(m_altAglStdDev, m_altAglMean); }

            //! Altitude values
            QPair<CAltitude, CAltitude> getAltitudeStdDevAndMean() const { return QPair<CAltitude, CAltitude>(m_altStdDev, m_altMean); }

            //! Elevation values
            QPair<CAltitude, CAltitude> getElevationStdDevAndMean() const { return QPair<CAltitude, CAltitude>(m_elvStdDev, m_elvMean); }

            //! Ground speed values
            QPair<PhysicalQuantities::CSpeed, PhysicalQuantities::CSpeed> getGroundSpeedStdDevAndMean() const { return QPair<PhysicalQuantities::CSpeed, PhysicalQuantities::CSpeed>(m_gsStdDev, m_gsMean); }

            //! Pitch values
            QPair<PhysicalQuantities::CAngle, PhysicalQuantities::CAngle> getPitchStdDevAndMean() const { return QPair<PhysicalQuantities::CAngle, PhysicalQuantities::CAngle>(m_pitchStdDev, m_pitchMean); }

            //! Scnenery deviation (if it can be calculated, otherwise PhysicalQuantities::CLength::null)
            //! This is without CG, so substract CG to get deviation
            const PhysicalQuantities::CLength &guessedSceneryDeviation() const { return m_guessedSceneryDeviation; }

            //! Get scenery deviation under consideration of CG
            PhysicalQuantities::CLength getGuessedSceneryDeviation(const PhysicalQuantities::CLength &cg) const;

            //! \copydoc Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! Calculate the standard deviiations
            bool calculateStdDeviations(const CAircraftSituationList &situations);

            //! NULL object
            static const CAircraftSituationChange &null();

        private:
            //! Guess scenery deviation
            void guessSceneryDeviation();

            int m_situationsCount = -1;
            CCallsign m_correspondingCallsign;
            // latest -> m_timestampMSecsSinceEpoch
            qint64 m_oldestTimestampMSecsSinceEpoch = -1;
            qint64 m_oldestAdjustedTimestampMSecsSinceEpoch = -1;
            qint64 m_latestAdjustedTimestampMSecsSinceEpoch = -1;
            bool m_constAscending = false;
            bool m_constDescending = false;
            bool m_constOnGround = false;
            bool m_wasOnGround = false;
            bool m_constNotOnGround = false;
            bool m_wasNotOnGround = false;
            bool m_justTakeoff = false;
            bool m_justTouchdown = false;
            bool m_rotateUp = false;
            bool m_constAccelerating = false;
            bool m_constDecelerating = false;
            bool m_containsPushBack = false;
            CAltitude m_altStdDev = CAltitude::null();
            CAltitude m_altMean = CAltitude::null();
            CAltitude m_elvStdDev = CAltitude::null();
            CAltitude m_elvMean = CAltitude::null();
            PhysicalQuantities::CSpeed m_gsStdDev = PhysicalQuantities::CSpeed::null();
            PhysicalQuantities::CSpeed m_gsMean = PhysicalQuantities::CSpeed::null();
            PhysicalQuantities::CAngle m_pitchStdDev = PhysicalQuantities::CAngle::null();
            PhysicalQuantities::CAngle m_pitchMean = PhysicalQuantities::CAngle::null();
            PhysicalQuantities::CLength m_altAglStdDev = PhysicalQuantities::CLength::null();
            PhysicalQuantities::CLength m_altAglMean = PhysicalQuantities::CLength::null();
            PhysicalQuantities::CLength m_guessedSceneryDeviation = PhysicalQuantities::CLength::null();

            BLACK_METACLASS(
                CAircraftSituationChange,
                BLACK_METAMEMBER(situationsCount),
                BLACK_METAMEMBER(correspondingCallsign),
                BLACK_METAMEMBER(constAscending),
                BLACK_METAMEMBER(constDescending),
                BLACK_METAMEMBER(constOnGround),
                BLACK_METAMEMBER(constNotOnGround),
                BLACK_METAMEMBER(justTakeoff),
                BLACK_METAMEMBER(justTouchdown),
                BLACK_METAMEMBER(containsPushBack),
                BLACK_METAMEMBER(rotateUp),
                BLACK_METAMEMBER(altStdDev),
                BLACK_METAMEMBER(altMean),
                BLACK_METAMEMBER(elvStdDev),
                BLACK_METAMEMBER(elvMean),
                BLACK_METAMEMBER(gsStdDev),
                BLACK_METAMEMBER(gsMean),
                BLACK_METAMEMBER(altAglStdDev),
                BLACK_METAMEMBER(altAglMean),
                BLACK_METAMEMBER(pitchStdDev),
                BLACK_METAMEMBER(pitchMean),
                BLACK_METAMEMBER(guessedSceneryDeviation),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(oldestTimestampMSecsSinceEpoch),
                BLACK_METAMEMBER(oldestAdjustedTimestampMSecsSinceEpoch),
                BLACK_METAMEMBER(latestAdjustedTimestampMSecsSinceEpoch)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationChange)

#endif // guard
