// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/aircraftmatchersetup.h"

#include <QStringBuilder>

#include "misc/stringutils.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation, CAircraftMatcherSetup)

namespace swift::misc::simulation
{
    CAircraftMatcherSetup::CAircraftMatcherSetup() { this->reset(); }

    CAircraftMatcherSetup::CAircraftMatcherSetup(CAircraftMatcherSetup::MatchingAlgorithm algorithm)
    {
        this->reset(algorithm);
    }

    CAircraftMatcherSetup::CAircraftMatcherSetup(CAircraftMatcherSetup::MatchingAlgorithm algorithm, MatchingMode mode,
                                                 PickSimilarStrategy pickStrategy)
    {
        this->setPickStrategy(pickStrategy);
        this->setMatchingAlgorithm(algorithm, false);
        this->setMatchingMode(mode);
    }

    bool CAircraftMatcherSetup::setMatchingAlgorithm(CAircraftMatcherSetup::MatchingAlgorithm algorithm, bool reset)
    {
        if (this->getMatchingAlgorithm() == algorithm) { return false; }
        if (reset) { this->reset(algorithm); }
        else { m_algorithm = static_cast<int>(algorithm); }
        return true;
    }

    bool CAircraftMatcherSetup::isReverseLookupModelString() const
    {
        return this->getMatchingMode().testFlag(ReverseLookupModelString);
    }

    bool CAircraftMatcherSetup::isReverseLookupSwiftLiveryIds() const
    {
        return this->getMatchingMode().testFlag(ReverseLookupSwiftLiveryIds);
    }

    void CAircraftMatcherSetup::resetReverseLookup()
    {
        MatchingMode m = this->getMatchingMode();
        m.setFlag(ReverseLookupDefault);
        this->setMatchingMode(m);
    }

    bool CAircraftMatcherSetup::doRunMsReverseLookupScript() const
    {
        return m_msReverseEnabled && !m_msReverseLookupFile.isEmpty();
    }

    bool CAircraftMatcherSetup::doRunMsMatchingStageScript() const
    {
        return m_msMatchingEnabled && !m_msMatchingStageFile.isEmpty();
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

    void CAircraftMatcherSetup::setAirlineGroupBehaviour(bool ifNoAirline, bool sameAsAirline)
    {
        MatchingMode m = this->getMatchingMode();
        const bool icao = m.testFlag(ByIcaoData);
        m.setFlag(ByAirlineGroupIfNoAirline, ifNoAirline);
        m.setFlag(ByAirlineGroupSameAsAirline, sameAsAirline);
        m.setFlag(ByIcaoData, icao);
        this->setMatchingMode(m);
    }

    QString CAircraftMatcherSetup::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        return u"algorithm: '" % this->getMatchingAlgorithmAsString() % u"' mode: '" % this->getMatchingModeAsString() %
               u"' force: '" % this->getForceModeAsString() % u"' strategy: '" % this->getPickStrategyAsString() %
               u"\' matching script: " % boolToOnOff(m_msReverseEnabled) % u'/' % boolToOnOff(m_msMatchingEnabled);
    }

    QVariant CAircraftMatcherSetup::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMatchingAlgorithm: return QVariant::fromValue(m_algorithm);
        case IndexMatchingMode: return QVariant::fromValue(m_mode);
        case IndexPickStrategy: return QVariant::fromValue(m_strategy);
        case IndexForceMode: return QVariant::fromValue(m_force);
        case IndexMsNetworkEntryFile: return QVariant::fromValue(m_msReverseLookupFile);
        case IndexMsMatchingStageFile: return QVariant::fromValue(m_msMatchingStageFile);
        case IndexMsNetworkEnabled: return QVariant::fromValue(m_msReverseEnabled);
        case IndexMsMatchingStageEnabled: return QVariant::fromValue(m_msMatchingEnabled);
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CAircraftMatcherSetup::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAircraftMatcherSetup>();
            return;
        }
        const auto i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMatchingAlgorithm: m_algorithm = variant.toInt(); break;
        case IndexMatchingMode: m_mode = variant.toInt(); break;
        case IndexForceMode: m_force = variant.toInt(); break;
        case IndexPickStrategy: m_strategy = variant.toInt(); break;
        case IndexMsNetworkEntryFile: m_msReverseLookupFile = variant.toString(); break;
        case IndexMsMatchingStageFile: m_msMatchingStageFile = variant.toString(); break;
        case IndexMsNetworkEnabled: m_msReverseEnabled = variant.toBool(); break;
        case IndexMsMatchingStageEnabled: m_msMatchingEnabled = variant.toBool(); break;
        default: break;
        }
        CValueObject::setPropertyByIndex(index, variant);
    }

    void CAircraftMatcherSetup::reset()
    {
        this->reset(MatchingStepwiseReducePlusScoreBased);
        m_msReverseLookupFile.clear();
        m_msMatchingStageFile.clear();
        m_msReverseEnabled = false;
        m_msMatchingEnabled = false;
    }

    void CAircraftMatcherSetup::reset(CAircraftMatcherSetup::MatchingAlgorithm algorithm)
    {
        m_algorithm = static_cast<int>(algorithm);
        MatchingMode mode = ModeNone;
        switch (algorithm)
        {
        case MatchingStepwiseReduce: mode = ModeDefaultReduce; break;
        case MatchingScoreBased: mode = ModeDefaultScore; break;
        case MatchingStepwiseReducePlusScoreBased:
        default: mode = ModeDefaultReducePlusScore; break;
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
        static const QString military("by military");
        static const QString civilian("by civilian");
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
        static const QString categorySmallAircraft("small aircraft categories");
        static const QString revModelString("reverse model lookup");
        static const QString revLiveryIds("reverse livery ids");
        static const QString agSameAsAirline("group as airline");
        static const QString agIfNoAirline("group if no airline");

        switch (modeFlag)
        {
        case ReverseLookupModelString: return revModelString;
        case ReverseLookupSwiftLiveryIds: return revLiveryIds;
        case ByModelString: return ms;
        case ByIcaoData: return icao;
        case ByFamily: return family;
        case ByLivery: return livery;
        case ByCombinedType: return combined;
        case ByIcaoOrderAircraftFirst: return icaoAircraft;
        case ByIcaoOrderAirlineFirst: return icaoAirline;
        case ByCivilian: return civilian;
        case ByMilitary: return military;
        case ByVtol: return vtol;
        case ByCategoryGlider: return categoryGlider;
        case ByCategoryMilitary: return categoryMilitary;
        case ByCategorySmallAircraft: return categorySmallAircraft;
        case ScoreIgnoreZeros: return noZeros;
        case ScorePreferColorLiveries: return preferColorLiveries;
        case ExcludeNoDbData: return exNoDb;
        case ExcludeNoExcluded: return exExcl;
        case ModelSetRemoveFailedModel: return removeFromModelSet;
        case ModelVerificationAtStartup: return verification;
        case ByAirlineGroupIfNoAirline: return agIfNoAirline;
        case ByAirlineGroupSameAsAirline: return agSameAsAirline;
        case ModelVerificationOnlyWarnError: return verificationWarn;
        case ModelFailoverIfNoModelCanBeAdded: return modelFailedAdded;
        default: break;
        }

        static const QString unknown("unknown");
        return unknown;
    }

    QString CAircraftMatcherSetup::modeToString(MatchingMode mode)
    {
        QStringList modes;
        if (mode.testFlag(ReverseLookupModelString)) { modes << modeFlagToString(ReverseLookupModelString); }
        if (mode.testFlag(ReverseLookupSwiftLiveryIds)) { modes << modeFlagToString(ReverseLookupSwiftLiveryIds); }
        if (mode.testFlag(ByModelString)) { modes << modeFlagToString(ByModelString); }
        if (mode.testFlag(ByIcaoData)) { modes << modeFlagToString(ByIcaoData); }
        if (mode.testFlag(ByIcaoOrderAircraftFirst)) { modes << modeFlagToString(ByIcaoOrderAircraftFirst); }
        if (mode.testFlag(ByIcaoOrderAirlineFirst)) { modes << modeFlagToString(ByIcaoOrderAirlineFirst); }
        if (mode.testFlag(ByFamily)) { modes << modeFlagToString(ByFamily); }
        if (mode.testFlag(ByLivery)) { modes << modeFlagToString(ByLivery); }
        if (mode.testFlag(ByCombinedType)) { modes << modeFlagToString(ByCombinedType); }
        if (mode.testFlag(ByCivilian)) { modes << modeFlagToString(ByCivilian); }
        if (mode.testFlag(ByMilitary)) { modes << modeFlagToString(ByMilitary); }
        if (mode.testFlag(ByCategoryGlider)) { modes << modeFlagToString(ByCategoryGlider); }
        if (mode.testFlag(ByCategoryMilitary)) { modes << modeFlagToString(ByCategoryMilitary); }
        if (mode.testFlag(ByCategorySmallAircraft)) { modes << modeFlagToString(ByCategorySmallAircraft); }
        if (mode.testFlag(ByVtol)) { modes << modeFlagToString(ByVtol); }
        if (mode.testFlag(ScoreIgnoreZeros)) { modes << modeFlagToString(ScoreIgnoreZeros); }
        if (mode.testFlag(ScorePreferColorLiveries)) { modes << modeFlagToString(ScorePreferColorLiveries); }
        if (mode.testFlag(ModelSetRemoveFailedModel)) { modes << modeFlagToString(ModelSetRemoveFailedModel); }
        if (mode.testFlag(ModelVerificationAtStartup)) { modes << modeFlagToString(ModelVerificationAtStartup); }
        if (mode.testFlag(ByAirlineGroupIfNoAirline)) { modes << modeFlagToString(ByAirlineGroupIfNoAirline); }
        if (mode.testFlag(ByAirlineGroupSameAsAirline)) { modes << modeFlagToString(ByAirlineGroupSameAsAirline); }
        if (mode.testFlag(ModelVerificationOnlyWarnError))
        {
            modes << modeFlagToString(ModelVerificationOnlyWarnError);
        }
        if (mode.testFlag(ModelFailoverIfNoModelCanBeAdded))
        {
            modes << modeFlagToString(ModelFailoverIfNoModelCanBeAdded);
        }
        return modes.join(", ");
    }

    const QString &CAircraftMatcherSetup::forceFlagToString(CAircraftMatcherSetup::ForceModeFlag forceFlag)
    {
        static const QString t("type");
        static const QString ec("engine count");
        static const QString e("engine");
        static const QString n("nothing");

        switch (forceFlag)
        {
        case ForceType: return t;
        case ForceEnginecount: return ec;
        case ForceEngine: return e;
        case ForceNothing:
        default: break;
        }
        return n;
    }

    QString CAircraftMatcherSetup::forceToString(ForceMode force)
    {
        if (force.testFlag(ForceNothing)) { return QStringLiteral("nothing"); }

        QStringList forces;
        if (force.testFlag(ForceType)) { forces << forceFlagToString(ForceType); }
        if (force.testFlag(ForceEnginecount)) { forces << forceFlagToString(ForceEnginecount); }
        if (force.testFlag(ForceEngine)) { forces << forceFlagToString(ForceEngine); }
        return forces.join(", ");
    }

    const QString &CAircraftMatcherSetup::strategyToString(CAircraftMatcherSetup::PickSimilarStrategy strategy)
    {
        static const QString f("first");
        static const QString o("order");
        static const QString r("random");

        switch (strategy)
        {
        case PickFirst: return f;
        case PickByOrder: return o;
        case PickRandom: return r;
        default: break;
        }

        static const QString unknown("unknown");
        return unknown;
    }

    CAircraftMatcherSetup::MatchingMode CAircraftMatcherSetup::matchingMode(
        bool revModelString, bool revLiveryIds, bool byModelString, bool byIcaoDataAircraft1st,
        bool byIcaoDataAirline1st, bool byFamily, bool byLivery, bool byCombinedType, bool byMilitary, bool byCivilian,
        bool byVtol, bool byGliderCategory, bool byMilitaryCategory, bool bySmallAircraftCategory,
        bool scoreIgnoreZeros, bool scorePreferColorLiveries, bool excludeNoDbData, bool excludeNoExcluded,
        bool modelVerification, bool modelVerificationWarnError, bool modelSetRemoveFailedModel, bool modelFailover)
    {
        if (modelFailover) { modelSetRemoveFailedModel = true; } // otherwise this does not make sense

        MatchingMode mode = byModelString ? ByModelString : ModeNone;
        if (byIcaoDataAircraft1st) { mode |= ByIcaoOrderAircraftFirst; }
        if (byIcaoDataAirline1st) { mode |= ByIcaoOrderAirlineFirst; }
        if (byFamily) { mode |= ByFamily; }
        if (byLivery) { mode |= ByLivery; }
        if (byCombinedType) { mode |= ByCombinedType; }
        if (byMilitary) { mode |= ByMilitary; }
        if (byCivilian) { mode |= ByCivilian; }
        if (byVtol) { mode |= ByVtol; }
        if (byGliderCategory) { mode |= ByCategoryGlider; }
        if (byMilitaryCategory) { mode |= ByCategoryMilitary; }
        if (bySmallAircraftCategory) { mode |= ByCategorySmallAircraft; }
        if (scoreIgnoreZeros) { mode |= ScoreIgnoreZeros; }
        if (scorePreferColorLiveries) { mode |= ScorePreferColorLiveries; }
        if (excludeNoDbData) { mode |= ExcludeNoDbData; }
        if (excludeNoExcluded) { mode |= ExcludeNoExcluded; }
        if (modelSetRemoveFailedModel) { mode |= ModelSetRemoveFailedModel; }
        if (modelVerification) { mode |= ModelVerificationAtStartup; }
        if (modelVerificationWarnError) { mode |= ModelVerificationOnlyWarnError; }
        if (modelFailover) { mode |= ModelFailoverIfNoModelCanBeAdded; }
        if (revModelString) { mode |= ReverseLookupModelString; }
        if (revLiveryIds) { mode |= ReverseLookupSwiftLiveryIds; }
        return mode;
    }
} // namespace swift::misc::simulation
