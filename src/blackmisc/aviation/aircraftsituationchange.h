// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTSITUATIONCHANGE_H
#define BLACKMISC_AVIATION_AIRCRAFTSITUATIONCHANGE_H

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/timestampbased.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/blackmiscexport.h"

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CAircraftSituationChange)

namespace BlackMisc
{
    namespace Simulation
    {
        class CAircraftModel;
    }
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
                IndexCallsign = CPropertyIndexRef::GlobalIndexCAircraftSituationChange,
                IndexIsNull,
                IndexSituationsCount,
                IndexConstAscending,
                IndexConstDescending,
                IndexConstOnGround,
                IndexConstNotOnGround,
                IndexJustTakingOff,
                IndexJustTouchingDown,
                IndexRotatingUp,
                IndexContainsPushBack,
                IndexAltitudeMean,
                IndexAltitudeStdDev,
                IndexElevationMean,
                IndexElevationStdDev,
            };

            //! Hint about the guessed scenery deviation
            enum GuessedSceneryDeviation
            {
                NoDeviationInfo,
                AllOnGround, //!< based on all situations on ground
                WasOnGround, //!< was on ground except last situation
                SmallAGLDeviationNearGround //!< "Almost const AGL" near Ground
            };

            //! Default constructor.
            CAircraftSituationChange() = default;

            //! Ctor with n situations
            //! \remark the timestamps of the latest situation will be used
            CAircraftSituationChange(const CAircraftSituationList &situations, const PhysicalQuantities::CLength &cg, bool isVtol, bool alreadySortedLatestFirst = false, bool calcStdDeviations = false);

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

            //! Elevation standard deviation and mean
            CAltitudePair getElevationStdDevAndMean() const { return CAltitudePair(m_elvStdDev, m_elvMean); }

            //! Guess on ground flag
            bool guessOnGround(CAircraftSituation &situation, const Simulation::CAircraftModel &model) const;

            //! Scnenery deviation (if it can be calculated, otherwise PhysicalQuantities::CLength::null)
            //! \remark This is without CG, so substract CG to get deviation
            const PhysicalQuantities::CLength &getGuessedSceneryDeviation() const { return m_guessedSceneryDeviation; }

            //! Get scenery deviation under consideration of CG
            PhysicalQuantities::CLength getGuessedSceneryDeviationCG() const { return m_guessedSceneryDeviationCG; }

            //! Scenery deviation hint
            GuessedSceneryDeviation getSceneryDeviationHint() const { return static_cast<GuessedSceneryDeviation>(m_guessedSceneryDeviationHint); }

            //! Scenery deviation hint hint as string
            const QString &getSceneryDeviationHintAsString() const { return guessedSceneryDeviationToString(this->getSceneryDeviationHint()); }

            //! Scenery deviation available?
            bool hasSceneryDeviation() const;

            //! Elevation within CAircraftSituation::allowedAltitudeDeviation range
            bool hasElevationDevWithinAllowedRange() const;

            //! Altitude within CAircraftSituation::allowedAltitudeDeviation range
            bool hasAltitudeDevWithinAllowedRange() const;

            //! \copydoc Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc Mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftSituationChange &compareValue) const;

            //! Calculate the standard deviiations
            bool calculateStdDeviations(const CAircraftSituationList &situations, const PhysicalQuantities::CLength &cg);

            //! NULL object
            static const CAircraftSituationChange &null();

            //! The enum as string
            static const QString &guessedSceneryDeviationToString(GuessedSceneryDeviation hint);

            //! Register metadata
            static void registerMetadata();

        private:
            //! Scenery deviation hint
            void setSceneryDeviationHint(GuessedSceneryDeviation hint) { m_guessedSceneryDeviationHint = static_cast<int>(hint); }

            //! Set scenery deviation
            void setSceneryDeviation(const PhysicalQuantities::CLength &deviation, const PhysicalQuantities::CLength &cg, GuessedSceneryDeviation hint);

            //! Guess scenery deviation
            void guessSceneryDeviation(const PhysicalQuantities::CLength &cg);

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
            int m_guessedSceneryDeviationHint = static_cast<int>(NoDeviationInfo);
            CAltitude m_altStdDev = CAltitude::null();
            CAltitude m_altMean = CAltitude::null();
            CAltitude m_elvStdDev = CAltitude::null();
            CAltitude m_elvMean = CAltitude::null();
            PhysicalQuantities::CSpeed m_gsStdDev = PhysicalQuantities::CSpeed::null();
            PhysicalQuantities::CSpeed m_gsMean = PhysicalQuantities::CSpeed::null();
            PhysicalQuantities::CAngle m_pitchStdDev = PhysicalQuantities::CAngle::null();
            PhysicalQuantities::CAngle m_pitchMean = PhysicalQuantities::CAngle::null();
            PhysicalQuantities::CLength m_gndDistStdDev = PhysicalQuantities::CLength::null();
            PhysicalQuantities::CLength m_gndDistMean = PhysicalQuantities::CLength::null();
            PhysicalQuantities::CLength m_guessedSceneryDeviation = PhysicalQuantities::CLength::null();
            PhysicalQuantities::CLength m_guessedSceneryDeviationCG = PhysicalQuantities::CLength::null();
            PhysicalQuantities::CLength m_maxGroundDistance = PhysicalQuantities::CLength::null();
            PhysicalQuantities::CLength m_minGroundDistance = PhysicalQuantities::CLength::null();

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
                BLACK_METAMEMBER(gndDistStdDev),
                BLACK_METAMEMBER(gndDistMean),
                BLACK_METAMEMBER(pitchStdDev),
                BLACK_METAMEMBER(pitchMean),
                BLACK_METAMEMBER(guessedSceneryDeviation),
                BLACK_METAMEMBER(guessedSceneryDeviationCG),
                BLACK_METAMEMBER(guessedSceneryDeviationHint),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(oldestTimestampMSecsSinceEpoch),
                BLACK_METAMEMBER(oldestAdjustedTimestampMSecsSinceEpoch),
                BLACK_METAMEMBER(latestAdjustedTimestampMSecsSinceEpoch)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationChange)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftSituationChange::GuessedSceneryDeviation)

#endif // guard
