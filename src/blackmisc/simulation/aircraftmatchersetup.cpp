/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "aircraftmatchersetup.h"
#include <QStringBuilder>

namespace BlackMisc
{
    namespace Simulation
    {
        CAircraftMatcherSetup::CAircraftMatcherSetup()
        {
            this->reset();
        }

        CAircraftMatcherSetup::CAircraftMatcherSetup(CAircraftMatcherSetup::MatchingAlgorithm algorithm)
        {
            this->reset(algorithm);
        }

        CAircraftMatcherSetup::CAircraftMatcherSetup(CAircraftMatcherSetup::MatchingAlgorithm algorithm, MatchingMode mode, PickSimilarStrategy pickStrategy)
        {
            this->setPickStrategy(pickStrategy);
            this->setMatchingAlgorithm(algorithm, false);
            this->setMatchingMode(mode);
        }

        bool CAircraftMatcherSetup::setMatchingAlgorithm(CAircraftMatcherSetup::MatchingAlgorithm algorithm, bool reset)
        {
            if (this->getMatchingAlgorithm() == algorithm) { return false; }
            if (reset)
            {
                this->reset(algorithm);
            }
            else
            {
                m_algorithm = static_cast<int>(algorithm);
            }
            return true;
        }

        void CAircraftMatcherSetup::setVerificationAtStartup(bool verify)
        {
            MatchingMode m = this->getMatchingMode();
            m.setFlag(ModelVerificationAtStartup, verify);
            this->setMatchingMode(m);
        }

        void CAircraftMatcherSetup::setOnlyShowVerificationWarningsAndErrors(bool only)
        {
            MatchingMode m = this->getMatchingMode();
            m.setFlag(ModelVerificationOnlyWarnError, only);
            this->setMatchingMode(m);
        }

        void CAircraftMatcherSetup::setReverseLookup(bool useModelLookup)
        {
            MatchingMode m = this->getMatchingMode();
            m.setFlag(ReverseLookupModelString, useModelLookup);
            this->setMatchingMode(m);
        }

        QString CAircraftMatcherSetup::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return u"algorithm: '" % this->getMatchingAlgorithmAsString() %
                   u"' mode: '" % this->getMatchingModeAsString() %
                   u"' strategy: '" % this->getPickStrategyAsString() %
                   u"\' matching script: " % boolToOnOff(m_msNetworkEnabled) % u'/' % boolToOnOff(m_msMatchingEnabled);
        }

        CVariant CAircraftMatcherSetup::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMatchingAlgorithm:   return CVariant::fromValue(m_algorithm);
            case IndexMatchingMode:        return CVariant::fromValue(m_mode);
            case IndexPickStrategy:        return CVariant::fromValue(m_strategy);
            case IndexMsNetworkEntryFile:  return CVariant::fromValue(m_msNetworkEntryFile);
            case IndexMsMatchingStageFile: return CVariant::fromValue(m_msMatchingStageFile);
            case IndexMsNetworkEnabled:    return CVariant::fromValue(m_msNetworkEnabled);
            case IndexMsMatchingStageEnabled: return CVariant::fromValue(m_msMatchingEnabled);
            default: break;
            }
            return CValueObject::propertyByIndex(index);
        }

        void CAircraftMatcherSetup::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftMatcherSetup>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMatchingAlgorithm: m_algorithm = variant.toInt(); break;
            case IndexMatchingMode:      m_mode = variant.toInt(); break;
            case IndexPickStrategy:      m_strategy = variant.toInt(); break;
            case IndexMsNetworkEntryFile:     m_msNetworkEntryFile  = variant.toQString(); break;
            case IndexMsMatchingStageFile:    m_msMatchingStageFile = variant.toQString(); break;
            case IndexMsNetworkEnabled:       m_msNetworkEnabled = variant.toBool(); break;
            case IndexMsMatchingStageEnabled: m_msNetworkEnabled = variant.toBool(); break;
            default: break;
            }
            CValueObject::setPropertyByIndex(index, variant);
        }

        void CAircraftMatcherSetup::reset()
        {
            this->reset(MatchingStepwiseReducePlusScoreBased);
            m_msNetworkEntryFile.clear();
            m_msMatchingStageFile.clear();
            m_msNetworkEnabled = false;
            m_msMatchingEnabled = false;
        }

        void CAircraftMatcherSetup::reset(CAircraftMatcherSetup::MatchingAlgorithm algorithm)
        {
            m_algorithm = static_cast<int>(algorithm);
            MatchingMode mode = ModeNone;
            switch (algorithm)
            {
            case MatchingStepwiseReduce: mode = ModeDefaultReduce; break;
            case MatchingScoreBased:     mode = ModeDefaultScore; break;
            case MatchingStepwiseReducePlusScoreBased:
            default:
                mode = ModeDefaultReducePlusScore;
                break;
            }
            mode |= ModeDefaultSet; // add set default

            this->setMatchingMode(mode);
            this->setPickStrategy(PickByOrder);
        }

        bool CAircraftMatcherSetup::useCategoryMatching() const
        {
            const MatchingMode mm = this->getMatchingMode();
            return mm.testFlag(ByCategoryGlider) || mm.testFlag(ByCategoryMilitary);
        }

        const QString &CAircraftMatcherSetup::algorithmToString(CAircraftMatcherSetup::MatchingAlgorithm algorithm)
        {
            static const QString rs("reduce + score based");
            static const QString s("score based");
            static const QString r("stepwise reduce");
            switch (algorithm)
            {
            case MatchingStepwiseReduce: return r;
            case MatchingScoreBased: return s;
            case MatchingStepwiseReducePlusScoreBased:
            default: break;
            }
            return rs;
        }

        const QString &CAircraftMatcherSetup::modeFlagToString(MatchingModeFlag modeFlag)
        {
            static const QString ms("by model string");
            static const QString icao("by ICAO");
            static const QString icaoAircraft("by ICAO aircraft first");
            static const QString icaoAirline("by ICAO airline first");
            static const QString family("by family");
            static const QString forceMil("force military");
            static const QString forceCiv("force civilian");
            static const QString vtol("VTOL");
            static const QString livery("by livery");
            static const QString combined("by combined code");
            static const QString noZeros("scoring, ignore zero scores");
            static const QString preferColorLiveries("scoring, prefer color liveries");
            static const QString exNoDb("excl.without DB data");
            static const QString exExcl("excl.excluded");
            static const QString removeFromModelSet("rem.from model set");
            static const QString verification("Verify models at startup");
            static const QString verificationWarn("Show only validation warnings/errors");
            static const QString modelFailedAdded("Replace models failed to be added");
            static const QString categoryGlider("glider categories");
            static const QString categoryMilitary("military categories");
            static const QString revModelString("reverse model lookup");

            switch (modeFlag)
            {
            case ReverseLookupModelString:   return revModelString;
            case ByModelString:              return ms;
            case ByIcaoData:                 return icao;
            case ByFamily:                   return family;
            case ByLivery:                   return livery;
            case ByCombinedType:             return combined;
            case ByIcaoOrderAircraftFirst:   return icaoAircraft;
            case ByIcaoOrderAirlineFirst:    return icaoAirline;
            case ByForceCivilian:            return forceCiv;
            case ByForceMilitary:            return forceMil;
            case ByVtol:                     return vtol;
            case ByCategoryGlider:           return categoryGlider;
            case ByCategoryMilitary:         return categoryMilitary;
            case ScoreIgnoreZeros:           return noZeros;
            case ScorePreferColorLiveries:   return preferColorLiveries;
            case ExcludeNoDbData:            return exNoDb;
            case ExcludeNoExcluded:          return exExcl;
            case ModelSetRemoveFailedModel:  return removeFromModelSet;
            case ModelVerificationAtStartup: return verification;
            case ModelVerificationOnlyWarnError:   return verificationWarn;
            case ModelFailoverIfNoModelCanBeAdded: return modelFailedAdded;
            default: break;
            }

            static const QString unknown("unknown");
            return unknown;
        }

        QString CAircraftMatcherSetup::modeToString(MatchingMode mode)
        {
            QStringList modes;
            if (mode.testFlag(ReverseLookupModelString))   { modes << modeFlagToString(ReverseLookupModelString); }
            if (mode.testFlag(ByModelString))              { modes << modeFlagToString(ByModelString); }
            if (mode.testFlag(ByIcaoData))                 { modes << modeFlagToString(ByIcaoData); }
            if (mode.testFlag(ByIcaoOrderAircraftFirst))   { modes << modeFlagToString(ByIcaoOrderAircraftFirst); }
            if (mode.testFlag(ByIcaoOrderAirlineFirst))    { modes << modeFlagToString(ByIcaoOrderAirlineFirst); }
            if (mode.testFlag(ByFamily))                   { modes << modeFlagToString(ByFamily); }
            if (mode.testFlag(ByLivery))                   { modes << modeFlagToString(ByLivery); }
            if (mode.testFlag(ByCombinedType))             { modes << modeFlagToString(ByCombinedType); }
            if (mode.testFlag(ByForceCivilian))            { modes << modeFlagToString(ByForceCivilian); }
            if (mode.testFlag(ByForceMilitary))            { modes << modeFlagToString(ByForceMilitary); }
            if (mode.testFlag(ByCategoryGlider))           { modes << modeFlagToString(ByCategoryGlider); }
            if (mode.testFlag(ByCategoryMilitary))         { modes << modeFlagToString(ByCategoryMilitary); }
            if (mode.testFlag(ByVtol))                     { modes << modeFlagToString(ByVtol); }
            if (mode.testFlag(ScoreIgnoreZeros))           { modes << modeFlagToString(ScoreIgnoreZeros); }
            if (mode.testFlag(ScorePreferColorLiveries))   { modes << modeFlagToString(ScorePreferColorLiveries); }
            if (mode.testFlag(ModelSetRemoveFailedModel))  { modes << modeFlagToString(ModelSetRemoveFailedModel); }
            if (mode.testFlag(ModelVerificationAtStartup)) { modes << modeFlagToString(ModelVerificationAtStartup); }
            if (mode.testFlag(ModelVerificationOnlyWarnError))   { modes << modeFlagToString(ModelVerificationOnlyWarnError); }
            if (mode.testFlag(ModelFailoverIfNoModelCanBeAdded)) { modes << modeFlagToString(ModelFailoverIfNoModelCanBeAdded); }

            return modes.join(", ");
        }

        const QString &CAircraftMatcherSetup::strategyToString(CAircraftMatcherSetup::PickSimilarStrategy strategy)
        {
            static const QString f("first");
            static const QString o("order");
            static const QString r("random");

            switch (strategy)
            {
            case PickFirst:   return f;
            case PickByOrder: return o;
            case PickRandom:  return r;
            default: break;
            }

            static const QString unknown("unknown");
            return unknown;
        }

        CAircraftMatcherSetup::MatchingMode CAircraftMatcherSetup::matchingMode(
            bool byModelString, bool byIcaoDataAircraft1st, bool byIcaoDataAirline1st, bool byFamily, bool byLivery, bool byCombinedType,
            bool byForceMilitary, bool byForceCivilian, bool byVtol,
            bool byGliderCategory, bool byMilitaryCategory,
            bool scoreIgnoreZeros, bool scorePreferColorLiveries,
            bool excludeNoDbData, bool excludeNoExcluded,
            bool modelVerification, bool modelVerificationWarnError,
            bool modelSetRemoveFailedModel, bool modelFailover)
        {
            if (modelFailover) { modelSetRemoveFailedModel = true; } // otherwise this does not make sense

            MatchingMode mode = byModelString ? ByModelString : ModeNone;
            if (byIcaoDataAircraft1st)     { mode |= ByIcaoOrderAircraftFirst; }
            if (byIcaoDataAirline1st)      { mode |= ByIcaoOrderAirlineFirst; }
            if (byFamily)                  { mode |= ByFamily; }
            if (byLivery)                  { mode |= ByLivery; }
            if (byCombinedType)            { mode |= ByCombinedType; }
            if (byForceMilitary)           { mode |= ByForceMilitary; }
            if (byForceCivilian)           { mode |= ByForceCivilian; }
            if (byVtol)                    { mode |= ByVtol; }
            if (byGliderCategory)          { mode |= ByCategoryGlider; }
            if (byMilitaryCategory)        { mode |= ByCategoryMilitary; }
            if (scoreIgnoreZeros)          { mode |= ScoreIgnoreZeros; }
            if (scorePreferColorLiveries)  { mode |= ScorePreferColorLiveries; }
            if (excludeNoDbData)           { mode |= ExcludeNoDbData; }
            if (excludeNoExcluded)         { mode |= ExcludeNoExcluded; }
            if (modelSetRemoveFailedModel) { mode |= ModelSetRemoveFailedModel; }
            if (modelVerification)         { mode |= ModelVerificationAtStartup; }
            if (modelVerificationWarnError) { mode |= ModelVerificationOnlyWarnError; }
            if (modelFailover)              { mode |= ModelFailoverIfNoModelCanBeAdded; }
            return mode;
        }
    } // namespace
} // namespace
