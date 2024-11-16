// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTSITUATIONCHANGE_H
#define SWIFT_MISC_AVIATION_AIRCRAFTSITUATIONCHANGE_H

#include "misc/aviation/altitude.h"
#include "misc/aviation/callsign.h"
#include "misc/metaclass.h"
#include "misc/pq/angle.h"
#include "misc/pq/speed.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/timestampbased.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftSituationChange)

namespace swift::misc
{
    namespace simulation
    {
        class CAircraftModel;
    }
    namespace aviation
    {
        class CAircraftSituation;
        class CAircraftSituationList;

        //! Value object about changes in situations
        class SWIFT_MISC_EXPORT CAircraftSituationChange :
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
            CAircraftSituationChange(const CAircraftSituationList &situations, const physical_quantities::CLength &cg, bool isVtol, bool alreadySortedLatestFirst = false, bool calcStdDeviations = false);

            //! Get callsign
            const CCallsign &getCallsign() const { return m_correspondingCallsign; }

            //! Null?
            bool isNull() const { return m_situationsCount < 2; } // we need at least 2 situations

            //! Basend on n situations
            int getSituationsCount() const { return m_situationsCount; }

            //! \copydoc swift::misc::aviation::CAircraftSituationList::isConstAscending
            bool isConstAscending() const { return m_constAscending; }

            //! \copydoc swift::misc::aviation::CAircraftSituationList::isConstDescending
            bool isConstDescending() const { return m_constDescending; }

            //! \copydoc swift::misc::aviation::CAircraftSituationList::isConstOnGround
            bool isConstOnGround() const { return m_constOnGround; }

            //! Was on ground (without latest situation)?
            bool wasConstOnGround() const { return m_wasOnGround; }

            //! \copydoc swift::misc::aviation::CAircraftSituationList::isConstNotOnGround
            bool isConstNotOnGround() const { return m_constNotOnGround; }

            //! Was not on ground (without latest situation)?
            bool wasConstNotOnGround() const { return m_wasNotOnGround; }

            //! \copydoc swift::misc::aviation::CAircraftSituationList::isConstAccelerating
            bool isConstAccelerating() const { return m_constAccelerating; }

            //! \copydoc swift::misc::aviation::CAircraftSituationList::isConstDecelarating
            bool isConstDecelarating() const { return m_constAccelerating; }

            //! \copydoc swift::misc::aviation::CAircraftSituationList::isJustTakingOff
            bool isJustTakingOff() const { return m_justTakeoff; }

            //! \copydoc swift::misc::aviation::CAircraftSituationList::isJustTouchingDown
            bool isJustTouchingDown() const { return m_justTouchdown; }

            //! \copydoc swift::misc::aviation::CAircraftSituationList::isRotatingUp
            bool isRotatingUp() const { return m_rotateUp; }

            //! \copydoc swift::misc::aviation::CAircraftSituationList::containsPushBack
            bool containsPushBack() const { return m_containsPushBack; }

            //! Elevation standard deviation and mean
            CAltitudePair getElevationStdDevAndMean() const { return CAltitudePair(m_elvStdDev, m_elvMean); }

            //! Guess on ground flag
            bool guessOnGround(CAircraftSituation &situation, const simulation::CAircraftModel &model) const;

            //! Scnenery deviation (if it can be calculated, otherwise physical_quantities::CLength::null)
            //! \remark This is without CG, so substract CG to get deviation
            const physical_quantities::CLength &getGuessedSceneryDeviation() const { return m_guessedSceneryDeviation; }

            //! Get scenery deviation under consideration of CG
            physical_quantities::CLength getGuessedSceneryDeviationCG() const { return m_guessedSceneryDeviationCG; }

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

            //! \copydoc mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftSituationChange &compareValue) const;

            //! Calculate the standard deviiations
            bool calculateStdDeviations(const CAircraftSituationList &situations, const physical_quantities::CLength &cg);

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
            void setSceneryDeviation(const physical_quantities::CLength &deviation, const physical_quantities::CLength &cg, GuessedSceneryDeviation hint);

            //! Guess scenery deviation
            void guessSceneryDeviation(const physical_quantities::CLength &cg);

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
            physical_quantities::CSpeed m_gsStdDev = physical_quantities::CSpeed::null();
            physical_quantities::CSpeed m_gsMean = physical_quantities::CSpeed::null();
            physical_quantities::CAngle m_pitchStdDev = physical_quantities::CAngle::null();
            physical_quantities::CAngle m_pitchMean = physical_quantities::CAngle::null();
            physical_quantities::CLength m_gndDistStdDev = physical_quantities::CLength::null();
            physical_quantities::CLength m_gndDistMean = physical_quantities::CLength::null();
            physical_quantities::CLength m_guessedSceneryDeviation = physical_quantities::CLength::null();
            physical_quantities::CLength m_guessedSceneryDeviationCG = physical_quantities::CLength::null();
            physical_quantities::CLength m_maxGroundDistance = physical_quantities::CLength::null();
            physical_quantities::CLength m_minGroundDistance = physical_quantities::CLength::null();

            SWIFT_METACLASS(
                CAircraftSituationChange,
                SWIFT_METAMEMBER(situationsCount),
                SWIFT_METAMEMBER(correspondingCallsign),
                SWIFT_METAMEMBER(constAscending),
                SWIFT_METAMEMBER(constDescending),
                SWIFT_METAMEMBER(constOnGround),
                SWIFT_METAMEMBER(constNotOnGround),
                SWIFT_METAMEMBER(justTakeoff),
                SWIFT_METAMEMBER(justTouchdown),
                SWIFT_METAMEMBER(containsPushBack),
                SWIFT_METAMEMBER(rotateUp),
                SWIFT_METAMEMBER(altStdDev),
                SWIFT_METAMEMBER(altMean),
                SWIFT_METAMEMBER(elvStdDev),
                SWIFT_METAMEMBER(elvMean),
                SWIFT_METAMEMBER(gsStdDev),
                SWIFT_METAMEMBER(gsMean),
                SWIFT_METAMEMBER(gndDistStdDev),
                SWIFT_METAMEMBER(gndDistMean),
                SWIFT_METAMEMBER(pitchStdDev),
                SWIFT_METAMEMBER(pitchMean),
                SWIFT_METAMEMBER(guessedSceneryDeviation),
                SWIFT_METAMEMBER(guessedSceneryDeviationCG),
                SWIFT_METAMEMBER(guessedSceneryDeviationHint),
                SWIFT_METAMEMBER(timestampMSecsSinceEpoch),
                SWIFT_METAMEMBER(oldestTimestampMSecsSinceEpoch),
                SWIFT_METAMEMBER(oldestAdjustedTimestampMSecsSinceEpoch),
                SWIFT_METAMEMBER(latestAdjustedTimestampMSecsSinceEpoch));
        };
    } // namespace aviation
} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituationChange)
Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftSituationChange::GuessedSceneryDeviation)

#endif // guard
