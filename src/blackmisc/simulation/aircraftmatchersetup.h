/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMATCHERSETUP_H
#define BLACKMISC_SIMULATION_AIRCRAFTMATCHERSETUP_H

#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Matcher settings
        class BLACKMISC_EXPORT CAircraftMatcherSetup : public CValueObject<CAircraftMatcherSetup>
        {
        public:
            //! Matching algorithm
            enum MatchingAlgorithm
            {
                MatchingScoreBased,
                MatchingStepwiseReducePlusScoreBased,
                MatchingStepwiseReduce,
            };

            //! Enabled matching mode flags
            enum MatchingModeFlag
            {
                ByModelString    = 1 << 0, //!< allow exact model string match
                ByIcaoData       = 1 << 1, //!< ICAO airline and aircraft codes
                ByFamily         = 1 << 2,
                ByLivery         = 1 << 3,
                ByCombinedType   = 1 << 4,
                ByManufacturer   = 1 << 5,
                ByMilitary       = 1 << 6, //!< military (in) will only search in military
                ByCivilian       = 1 << 7, //!< civilian (in) will only search in civilian
                ByMilOrCivilian  = ByCivilian | ByMilitary,
                ByVtol           = 1 << 8,
                ByIcaoOrderAircraftFirst    = (1 <<  9) | ByIcaoData,
                ByIcaoOrderAirlineFirst     = (1 << 10) | ByIcaoData,
                ByAirlineGroupSameAsAirline = (1 << 11) | ByIcaoData,
                ByAirlineGroupIfNoAirline   = (1 << 12) | ByIcaoData,

                // --- exclusion ---
                ExcludeNoDbData   = 1 << 13,
                ExcludeNoExcluded = 1 << 14,
                ExcludeDefault = ExcludeNoExcluded | ExcludeNoDbData,

                // --- model set ---
                ModelSetRemoveFailedModel        = 1 << 15,
                ModelVerificationAtStartup       = 1 << 16,
                ModelFailoverIfNoModelCanBeAdded = 1 << 17,
                ModelVerificationOnlyWarnError   = 1 << 18, // later added, hence 18

                // --- categories ---
                ByCategoryGlider            =  1 << 20,
                ByCategoryMilitary          =  1 << 21,
                ByCategorySmallAircraft     =  1 << 22,

                // --- reverse lookup ---
                ReverseLookupModelString    = 1 << 25,
                ReverseLookupSwiftLiveryIds = 1 << 26,
                ReverseLookupDefault = ReverseLookupModelString | ReverseLookupSwiftLiveryIds,

                // --- score based matching ---
                ScoreIgnoreZeros         = 1 << 28, //!< zero scores are ignored
                ScorePreferColorLiveries = 1 << 29, //!< prefer color liveries

                // --- others ---
                ModeNone          = 0,
                ModeByFLags       = ByMilOrCivilian  | ByVtol,

                // default mode for set handling
                ModeDefaultSet    = ModelSetRemoveFailedModel | ModelVerificationAtStartup | ModelFailoverIfNoModelCanBeAdded,

                // default depending on algorithm
                ModeDefaultScore  =     ScoreIgnoreZeros | ScorePreferColorLiveries | ExcludeDefault | ReverseLookupDefault,
                ModeDefaultReduce =          ModeByFLags | ByModelString | ByFamily | ByManufacturer | ByCombinedType | ByIcaoOrderAircraftFirst | ByAirlineGroupIfNoAirline | ReverseLookupDefault | ExcludeDefault | ByLivery,
                ModeDefaultReducePlusScore = ModeByFLags | ByModelString | ByFamily | ByManufacturer | ByCombinedType | ByIcaoOrderAircraftFirst | ByAirlineGroupIfNoAirline | ReverseLookupDefault | ExcludeDefault | ModeDefaultScore,
            };
            Q_DECLARE_FLAGS(MatchingMode, MatchingModeFlag)

            //! Force values
            enum ForceModeFlag
            {
                ForceNothing     = 0,
                ForceType        = 1 << 0,
                ForceEnginecount = 1 << 1,
                ForceEngine      = 1 << 2
            };
            Q_DECLARE_FLAGS(ForceMode, ForceModeFlag)

            //! How to pick among similar candiates
            enum PickSimilarStrategy
            {
                PickFirst,
                PickRandom,
                PickByOrder
            };

            //! Properties by index
            enum ColumnIndex
            {
                IndexMatchingAlgorithm = CPropertyIndex::GlobalIndexCAircraftMatcherSetup,
                IndexMatchingMode,
                IndexForceMode,
                IndexPickStrategy,
                IndexMsNetworkEntryFile,
                IndexMsMatchingStageFile,
                IndexMsNetworkEnabled,
                IndexMsMatchingStageEnabled
            };

            //! Constructor
            CAircraftMatcherSetup();

            //! Constructor
            CAircraftMatcherSetup(MatchingAlgorithm algorithm);

            //! Constructor
            CAircraftMatcherSetup(MatchingAlgorithm algorithm, MatchingMode mode, PickSimilarStrategy pickStrategy);

            //! Algorithm
            MatchingAlgorithm getMatchingAlgorithm() const { return static_cast<MatchingAlgorithm>(m_algorithm); }

            //! Algorithm as string
            const QString &getMatchingAlgorithmAsString() const { return algorithmToString(this->getMatchingAlgorithm()); }

            //! Algorithm
            bool setMatchingAlgorithm(MatchingAlgorithm algorithm, bool reset = true);

            //! Matching mode
            MatchingMode getMatchingMode() const { return static_cast<MatchingMode>(m_mode); }

            //! Force mode
            ForceMode getForceMode() const { return static_cast<ForceMode>(m_force); }

            //! Force mode
            void setForceMode(ForceMode fm) { m_force = static_cast<int>(fm); }

            //! Force mode as string
            QString getForceModeAsString() const { return forceToString(this->getForceMode()); }

            //! Reverse lookup
            //! @{
            bool isReverseLookupModelString() const;
            bool isReverseLookupSwiftLiveryIds() const;
            void resetReverseLookup();
            //! @}

            //! DB data?
            bool isDbDataOnly() const { return this->getMatchingMode().testFlag(ExcludeNoDbData); }

            //! Get matching files
            //! @{
            const QString &getMsReverseLookupFile() const { return m_msReverseLookupFile; }
            const QString &getMsMatchingStageFile() const { return m_msMatchingStageFile; }
            //! @}

            //! Set matching files
            //! @{
            void setMsReverseLookupFile(const QString &file) { m_msReverseLookupFile = file; }
            void setMsMatchingStageFile(const QString &file) { m_msMatchingStageFile = file; }
            //! @}

            //! Is matching script enabled
            //! @{
            bool isMsReverseLookupEnabled() const { return m_msReverseEnabled; }
            bool isMsMatchingStageEnabled() const { return m_msMatchingEnabled; }
            //! @}

            //! Run the scripts
            //! @{
            bool doRunMsReverseLookupScript() const;
            bool doRunMsMatchingStageScript() const;
            //! @}

            //! Is matching script enabled
            //! @{
            void setMsReverseLookupEnabled(bool enabled) { m_msReverseEnabled  = enabled; }
            void setMsMatchingStageEnabled(bool enabled) { m_msMatchingEnabled = enabled; }
            //! @}

            //! Verification at startup?
            //! \sa ModelVerificationOnStartup
            bool doVerificationAtStartup() const { return this->getMatchingMode().testFlag(ModelVerificationAtStartup); }

            //! Verification only shown for warning/errors?
            //! \sa ModelVerificationOnlyWarnError
            bool onlyShowVerificationWarningsAndErrors() const { return this->getMatchingMode().testFlag(ModelVerificationOnlyWarnError); }

            //! Failover if model cannot be loaded
            //! \sa ModelFailoverIfNoModelCanBeAdded
            bool doModelAddFailover() const { return this->getMatchingMode().testFlag(ModelFailoverIfNoModelCanBeAdded); }

            //! Set startup verification
            void setVerificationAtStartup(bool verify);

            //! Set the "show only errors" mode
            void setOnlyShowVerificationWarningsAndErrors(bool only);

            //! Matching mode as string
            QString getMatchingModeAsString() const { return modeToString(this->getMatchingMode()); }

            //! Dynamic offset values?
            void setMatchingMode(MatchingMode mode) { m_mode = static_cast<int>(mode); }

            //! Remove if failed?
            bool removeFromSetIfFailed() const { return this->getMatchingMode().testFlag(ModelSetRemoveFailedModel); }

            //! Strategy among equally suitable models
            PickSimilarStrategy getPickStrategy() const { return static_cast<PickSimilarStrategy>(m_strategy); }

            //! Strategy as string
            const QString &getPickStrategyAsString() const { return strategyToString(this->getPickStrategy()); }

            //! Set the strategy
            void setPickStrategy(PickSimilarStrategy strategy) { m_strategy = static_cast<int>(strategy); }

            //! Airline group behaviour
            void setAirlineGroupBehaviour(bool ifNoAirline, bool sameAsAirline);

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

            //! Reset
            void reset();

            //! Reset
            void reset(MatchingAlgorithm algorithm);

            //! Use category matching
            bool useCategoryMatching() const;

            //! Algorithm to string
            static const QString &algorithmToString(MatchingAlgorithm algorithm);

            //! Enumeration as string
            static const QString &modeFlagToString(MatchingModeFlag modeFlag);

            //! Enumeration as string
            static QString modeToString(MatchingMode mode);

            //! Force flag to string
            static const QString &forceFlagToString(ForceModeFlag forceFlag);

            //! Force to string
            static const QString forceToString(ForceMode force);

            //! Strategy to string
            static const QString &strategyToString(PickSimilarStrategy strategy);

            //! Mode by flags
            static MatchingMode matchingMode(bool revModelString,  bool revLiveryIds,
                                             bool byModelString,   bool byIcaoDataAircraft1st, bool byIcaoDataAirline1st,
                                             bool byFamily,        bool byLivery,              bool byCombinedType,
                                             bool byForceMilitary, bool byForceCivilian,
                                             bool byVtol,            bool byGliderCategory,           bool byMilitaryCategory,        bool bySmallAircraftCategory,
                                             bool scoreIgnoreZeros,  bool scorePreferColorLiveries,   bool excludeNoDbData,           bool excludeNoExcluded,
                                             bool modelVerification, bool modelVerificationWarnError, bool modelSetRemoveFailedModel, bool modelFailover);

        private:
            int m_algorithm = static_cast<int>(MatchingStepwiseReducePlusScoreBased);
            int m_mode      = static_cast<int>(ModeDefaultReducePlusScore);
            int m_strategy  = static_cast<int>(PickByOrder);
            int m_force     = static_cast<int>(ForceNothing);
            QString m_msReverseLookupFile;    //!< network entry matching script file
            QString m_msMatchingStageFile;    //!< matching stage matching script file
            bool m_msReverseEnabled  = false; //!< enable network matching script
            bool m_msMatchingEnabled = false; //!< enable matching stage matching script

            BLACK_METACLASS(
                CAircraftMatcherSetup,
                BLACK_METAMEMBER(algorithm),
                BLACK_METAMEMBER(mode),
                BLACK_METAMEMBER(strategy),
                BLACK_METAMEMBER(force),
                BLACK_METAMEMBER(msReverseLookupFile),
                BLACK_METAMEMBER(msMatchingStageFile),
                BLACK_METAMEMBER(msReverseEnabled),
                BLACK_METAMEMBER(msMatchingEnabled)
            );
        };
    } // ns
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftMatcherSetup)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftMatcherSetup::MatchingAlgorithm)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftMatcherSetup::MatchingMode)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftMatcherSetup::MatchingModeFlag)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftMatcherSetup::ForceMode)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftMatcherSetup::ForceModeFlag)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftMatcherSetup::PickSimilarStrategy)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::CAircraftMatcherSetup::MatchingMode)

#endif // guard
