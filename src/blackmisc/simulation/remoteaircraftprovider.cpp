// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/matchingutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/json.h"
#include "blackmisc/verify.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Json;
using namespace BlackConfig;

namespace BlackMisc::Simulation
{
    IRemoteAircraftProvider::IRemoteAircraftProvider()
    {}

    IRemoteAircraftProvider::~IRemoteAircraftProvider()
    {}

    const QStringList &CRemoteAircraftProvider::getLogCategories()
    {
        static const QStringList cats { CLogCategories::matching(), CLogCategories::network() };
        return cats;
    }

    CRemoteAircraftProvider::CRemoteAircraftProvider(QObject *parent) : QObject(parent),
                                                                        IRemoteAircraftProvider(),
                                                                        CIdentifiable(this)
    {}

    CSimulatedAircraftList CRemoteAircraftProvider::getAircraftInRange() const
    {
        QReadLocker l(&m_lockAircraft);
        const QList<CSimulatedAircraft> aircraftInRange = m_aircraftInRange.values();
        l.unlock();
        return CSimulatedAircraftList(aircraftInRange);
    }

    CCallsignSet CRemoteAircraftProvider::getAircraftInRangeCallsigns() const
    {
        QReadLocker l(&m_lockAircraft);
        const QList<CCallsign> callsigns = m_aircraftInRange.keys();
        l.unlock();
        return CCallsignSet(callsigns);
    }

    CSimulatedAircraft CRemoteAircraftProvider::getAircraftInRangeForCallsign(const CCallsign &callsign) const
    {
        const CSimulatedAircraft aircraft = this->getAircraftInRange().findFirstByCallsign(callsign);
        return aircraft;
    }

    CAircraftModel CRemoteAircraftProvider::getAircraftInRangeModelForCallsign(const CCallsign &callsign) const
    {
        const CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign)); // threadsafe
        return aircraft.getModel();
    }

    CAircraftSituationList CRemoteAircraftProvider::remoteAircraftSituations(const CCallsign &callsign) const
    {
        static const CAircraftSituationList empty;
        QReadLocker l(&m_lockSituations);
        if (!m_situationsByCallsign.contains(callsign)) { return empty; }
        return m_situationsByCallsign[callsign];
    }

    CAircraftSituation CRemoteAircraftProvider::remoteAircraftSituation(const CCallsign &callsign, int index) const
    {
        const CAircraftSituationList situations = this->remoteAircraftSituations(callsign);
        if (index < 0 || index >= situations.size()) { return CAircraftSituation::null(); }
        return situations[index];
    }

    MillisecondsMinMaxMean CRemoteAircraftProvider::remoteAircraftSituationsTimestampDifferenceMinMaxMean(const CCallsign &callsign) const
    {
        const CAircraftSituationList situations = this->remoteAircraftSituations(callsign);
        return situations.getOffsetMinMaxMean();
    }

    CAircraftSituationList CRemoteAircraftProvider::latestRemoteAircraftSituations() const
    {
        QReadLocker l(&m_lockSituations);
        const QList<CAircraftSituation> situations(m_latestSituationByCallsign.values());
        l.unlock();
        return CAircraftSituationList(situations);
    }

    CAircraftSituationList CRemoteAircraftProvider::latestOnGroundProviderElevations() const
    {
        QReadLocker l(&m_lockSituations);
        const QList<CAircraftSituation> situations(m_latestOnGroundProviderElevation.values());
        l.unlock();
        return CAircraftSituationList(situations);
    }

    int CRemoteAircraftProvider::remoteAircraftSituationsCount(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockSituations);
        if (!m_situationsByCallsign.contains(callsign)) { return -1; }
        return m_situationsByCallsign[callsign].size();
    }

    CAircraftPartsList CRemoteAircraftProvider::remoteAircraftParts(const CCallsign &callsign) const
    {
        static const CAircraftPartsList empty;
        QReadLocker l(&m_lockParts);
        if (!m_partsByCallsign.contains(callsign)) { return empty; }
        return m_partsByCallsign[callsign];
    }

    int CRemoteAircraftProvider::remoteAircraftPartsCount(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockParts);
        if (!m_partsByCallsign.contains(callsign)) { return -1; }
        return m_partsByCallsign[callsign].size();
    }

    bool CRemoteAircraftProvider::isRemoteAircraftSupportingParts(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockParts);
        return m_aircraftWithParts.contains(callsign);
    }

    int CRemoteAircraftProvider::getRemoteAircraftSupportingPartsCount() const
    {
        QReadLocker l(&m_lockParts);
        return m_aircraftWithParts.size();
    }

    CCallsignSet CRemoteAircraftProvider::remoteAircraftSupportingParts() const
    {
        QReadLocker l(&m_lockParts);
        return m_aircraftWithParts;
    }

    CAircraftSituationChangeList CRemoteAircraftProvider::remoteAircraftSituationChanges(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockChanges);
        return m_changesByCallsign[callsign];
    }

    int CRemoteAircraftProvider::remoteAircraftSituationChangesCount(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockChanges);
        return m_changesByCallsign[callsign].size();
    }

    int CRemoteAircraftProvider::getAircraftInRangeCount() const
    {
        QReadLocker l(&m_lockAircraft);
        return m_aircraftInRange.size();
    }

    void CRemoteAircraftProvider::removeAllAircraft()
    {
        const CCallsignSet callsigns = this->getAircraftInRangeCallsigns();

        // locked members
        {
            QWriteLocker l(&m_lockParts);
            m_partsByCallsign.clear();
            m_aircraftWithParts.clear();
            m_partsAdded = 0;
            m_partsLastModified.clear();
        }
        {
            QWriteLocker l(&m_lockSituations);
            m_situationsByCallsign.clear();
            m_latestSituationByCallsign.clear();
            m_latestOnGroundProviderElevation.clear();
            m_situationsAdded = 0;
            m_situationsLastModified.clear();
            m_testOffset.clear();
        }
        {
            QWriteLocker l(&m_lockChanges);
            m_changesByCallsign.clear();
        }

        {
            QWriteLocker l(&m_lockPartsHistory);
            m_aircraftPartsMessages.clear();
        }
        {
            QWriteLocker l(&m_lockMessages);
            m_reverseLookupMessages.clear();
        }
        {
            QWriteLocker l(&m_lockAircraft);
            m_aircraftInRange.clear();
            m_dbCGPerCallsign.clear();
        }

        for (const CCallsign &cs : callsigns)
        {
            emit this->removedAircraft(cs);
        }
    }

    void CRemoteAircraftProvider::removeReverseLookupMessages(const CCallsign &callsign)
    {
        QWriteLocker l(&m_lockMessages);
        m_reverseLookupMessages.remove(callsign);
    }

    bool CRemoteAircraftProvider::addNewAircraftInRange(const CSimulatedAircraft &aircraft)
    {
        if (this->isAircraftInRange(aircraft.getCallsign())) { return false; }

        // store
        {
            QWriteLocker l(&m_lockAircraft);
            m_aircraftInRange.insert(aircraft.getCallsign(), aircraft);
        }
        emit this->addedAircraft(aircraft);
        emit this->changedAircraftInRange();
        return true;
    }

    int CRemoteAircraftProvider::updateAircraftInRange(const CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues)
    {
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");
        int c = 0;
        {
            QWriteLocker l(&m_lockAircraft);
            if (!m_aircraftInRange.contains(callsign)) { return 0; }
            c = m_aircraftInRange[callsign].apply(vm, skipEqualValues).size();
        }
        if (c > 0)
        {
            emit this->changedAircraftInRange();
        }
        return c;
    }

    bool CRemoteAircraftProvider::updateAircraftInRangeDistanceBearing(const CCallsign &callsign, const CAircraftSituation &situation, const CLength &distance, const CAngle &bearing)
    {
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");
        {
            QWriteLocker l(&m_lockAircraft);
            if (!m_aircraftInRange.contains(callsign)) { return false; }
            CSimulatedAircraft &aircraft = m_aircraftInRange[callsign];
            aircraft.setSituation(situation);
            if (!bearing.isNull()) { aircraft.setRelativeBearing(bearing); }
            if (!distance.isNull()) { aircraft.setRelativeDistance(distance); }
        }
        return true;
    }

    CAircraftSituation CRemoteAircraftProvider::storeAircraftSituation(const CAircraftSituation &situation, bool allowTestAltitudeOffset)
    {
        const CCallsign cs = situation.getCallsign();
        if (cs.isEmpty()) { return situation; }

        // testing
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            BLACK_VERIFY_X(situation.getTimeOffsetMs() > 0, Q_FUNC_INFO, "Missing offset");
            BLACK_VERIFY_X(situation.isValidVectorRange(), Q_FUNC_INFO, "Invalid vector");
        }

        // add altitude offset (for testing only)
        CAircraftSituation situationCorrected(allowTestAltitudeOffset ? this->addTestAltitudeOffsetToSituation(situation) : situation);

        // CG, model
        const CAircraftModel aircraftModel = this->getAircraftInRangeModelForCallsign(cs);
        if (situation.hasCG() && aircraftModel.getCG() != situation.getCG())
        {
            this->updateCG(cs, situation.getCG());
        }

        // list from new to old
        CAircraftSituationList updatedSituations; // copy of updated situations
        {
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            QWriteLocker lock(&m_lockSituations);
            m_situationsAdded++;
            m_situationsLastModified[cs] = now;
            CAircraftSituationList &newSituationsList = m_situationsByCallsign[cs];
            newSituationsList.setAdjustedSortHint(CAircraftSituationList::AdjustedTimestampLatestFirst);
            const int situations = newSituationsList.size();
            if (situations < 1)
            {
                newSituationsList.prefillLatestAdjustedFirst(situationCorrected, IRemoteAircraftProvider::MaxSituationsPerCallsign);
            }
            else if (!situationCorrected.hasVelocity() && newSituationsList.front().hasVelocity())
            {
                return situationCorrected;
            }
            else
            {
                // newSituationsList.push_frontKeepLatestFirstIgnoreOverlapping(situationCorrected, true, IRemoteAircraftProvider::MaxSituationsPerCallsign);
                newSituationsList.push_frontKeepLatestFirstAdjustOffset(situationCorrected, true, IRemoteAircraftProvider::MaxSituationsPerCallsign);
                newSituationsList.setAdjustedSortHint(CAircraftSituationList::AdjustedTimestampLatestFirst);
                newSituationsList.transferElevationForward(); // transfer elevations, will do nothing if elevations already exist

                // unify all inbound ground information
                if (situation.hasInboundGroundDetails())
                {
                    newSituationsList.setOnGroundDetails(situation.getOnGroundDetails());
                }
            }
            m_latestSituationByCallsign[cs] = situationCorrected;

            // check sort order
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(newSituationsList.isSortedAdjustedLatestFirstWithoutNullPositions(), Q_FUNC_INFO, "wrong adjusted sort order");
                BLACK_VERIFY_X(newSituationsList.isSortedLatestFirst(), Q_FUNC_INFO, "wrong sort order");
                BLACK_VERIFY_X(newSituationsList.size() <= IRemoteAircraftProvider::MaxSituationsPerCallsign, Q_FUNC_INFO, "Wrong size");
            }

            if (!situation.hasInboundGroundDetails())
            {
                // first use a version without standard deviations to guess "on ground
                const CAircraftSituationChange simpleChange(updatedSituations, situationCorrected.getCG(), aircraftModel.isVtol(), true, false);

                // guess GND
                simpleChange.guessOnGround(newSituationsList.front(), aircraftModel);
            }
            updatedSituations = m_situationsByCallsign[cs];

        } // lock

        // calculate change AFTER gnd. was guessed
        Q_ASSERT_X(!updatedSituations.isEmpty(), Q_FUNC_INFO, "Missing situations");
        const CAircraftSituationChange change(updatedSituations, situationCorrected.getCG(), aircraftModel.isVtol(), true, true);
        this->storeChange(change);

        if (change.hasSceneryDeviation())
        {
            const CLength offset = change.getGuessedSceneryDeviation();
            situationCorrected.setSceneryOffset(offset);

            QWriteLocker lock(&m_lockSituations);
            m_latestSituationByCallsign[cs].setSceneryOffset(offset);
            m_situationsByCallsign[cs].front().setSceneryOffset(offset);
        }

        // situation has been added
        emit this->addedAircraftSituation(situationCorrected);

        // bye
        return situationCorrected;
    }

    void CRemoteAircraftProvider::storeAircraftParts(const CCallsign &callsign, const CAircraftParts &parts, bool removeOutdated)
    {
        BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
        if (callsign.isEmpty()) { return; }

        // list sorted from new to old
        const qint64 ts = QDateTime::currentMSecsSinceEpoch();
        CAircraftPartsList correctiveParts;
        {
            QWriteLocker lock(&m_lockParts);
            m_partsAdded++;
            m_partsLastModified[callsign] = ts;
            CAircraftPartsList &partsList = m_partsByCallsign[callsign];
            partsList.push_frontKeepLatestFirstAdjustOffset(parts, true, IRemoteAircraftProvider::MaxPartsPerCallsign);
            partsList.setAdjustedSortHint(CAircraftPartsList::AdjustedTimestampLatestFirst);

            // remove outdated parts (but never remove the most recent one)
            if (removeOutdated) { IRemoteAircraftProvider::removeOutdatedParts(partsList); }
            correctiveParts = partsList;

            // check sort order
            Q_ASSERT_X(partsList.isSortedAdjustedLatestFirst(), Q_FUNC_INFO, "wrong sort order");
            Q_ASSERT_X(partsList.size() <= IRemoteAircraftProvider::MaxPartsPerCallsign, Q_FUNC_INFO, "Wrong size");
        } // lock

        // adjust gnd.flag from parts
        if (!correctiveParts.isEmpty())
        {
            QWriteLocker lock(&m_lockSituations);
            CAircraftSituationList &situationList = m_situationsByCallsign[callsign];
            const int c = situationList.adjustGroundFlag(parts);
            if (c > 0) { m_situationsLastModified[callsign] = ts; }
        }

        // update aircraft
        {
            QWriteLocker l(&m_lockAircraft);
            if (m_aircraftInRange.contains(callsign))
            {
                CSimulatedAircraft &aircraft = m_aircraftInRange[callsign];
                aircraft.setParts(parts);
                aircraft.setPartsSynchronized(true);
            }
        }

        // update parts
        {
            // aircraft supporting parts
            QWriteLocker l(&m_lockParts);
            m_aircraftWithParts.insert(callsign); // mark as callsign which supports parts
        }

        emit this->addedAircraftParts(callsign, parts);
    }

    void CRemoteAircraftProvider::storeAircraftParts(const CCallsign &callsign, const QJsonObject &jsonObject, qint64 currentOffsetMs)
    {
        const CSimulatedAircraft remoteAircraft(this->getAircraftInRangeForCallsign(callsign));
        const bool isFull = jsonObject.value(CAircraftParts::attributeNameIsFullJson()).toBool();
        const bool validCs = remoteAircraft.hasValidCallsign();
        if (!validCs)
        {
            if (!isFull) { return; } // incremental parts broadcasting
            return; // suspicious
        }

        // If we are not yet synchronized, we throw away any incremental packet
        if (!remoteAircraft.isPartsSynchronized() && !isFull) { return; }

        CAircraftParts parts;
        try
        {
            if (isFull)
            {
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    // validation in dev.env.
                    const int attributes = jsonObject.size();
                    const bool correctCount = (attributes == CAircraftParts::attributesCountFullJson);
                    BLACK_VERIFY_X(correctCount || !CBuildConfig::isLocalDeveloperDebugBuild(), Q_FUNC_INFO, "Wrong full aircraft parts");
                    if (!correctCount)
                    {
                        CLogMessage(this).warning(u"Wrong full parts attributes, %1 (expected %2)") << attributes << CAircraftParts::attributesCountFullJson;
                        //! \todo KB 2020-04 ignore? make incremental?
                        if (attributes < 3)
                        {
                            // EXPERIMENTAL
                            if (attributes < 1) { return; }

                            // treat as incremental
                            CLogMessage(this).warning(u"Treating %1 attributes as incremental") << attributes;
                            parts = this->remoteAircraftParts(callsign).frontOrDefault(); // latest
                            const QJsonObject config = applyIncrementalObject(parts.toJson(), jsonObject);
                            parts.convertFromJson(config);
                        }
                    }
                }
                parts.convertFromJson(jsonObject);
            }
            else
            {
                // incremental update
                parts = this->remoteAircraftParts(callsign).frontOrDefault(); // latest
                const QJsonObject config = applyIncrementalObject(parts.toJson(), jsonObject);
                parts.convertFromJson(config);
            }
        }
        catch (const CJsonException &ex)
        {
            CStatusMessage message = CStatusMessage::fromJsonException(ex, this, "Invalid parts packet");
            message.setSeverity(CStatusMessage::SeverityDebug);
            CLogMessage::preformatted(message);
        }

        // make sure in any case right time and correct details
        parts.setCurrentUtcTime();
        parts.setTimeOffsetMs(currentOffsetMs);
        parts.setPartsDetails(CAircraftParts::FSDAircraftParts);

        // store part history (parts always absolute)
        this->storeAircraftParts(callsign, parts, false);

        // history
        if (this->isAircraftPartsHistoryEnabled())
        {
            const QJsonDocument doc(jsonObject);
            const QString partsAsString = doc.toJson(QJsonDocument::Compact);
            const CStatusMessage message(this, CStatusMessage::SeverityInfo, callsign.isEmpty() ? callsign.toQString() + ": " + partsAsString.trimmed() : partsAsString.trimmed());

            QReadLocker l(&m_lockPartsHistory);
            if (m_aircraftPartsMessages.contains(callsign))
            {
                CStatusMessageList &msgs = m_aircraftPartsMessages[callsign];
                msgs.push_back(message);
            }
            else
            {
                m_aircraftPartsMessages.insert(callsign, message);
            }
        }
    }

    void CRemoteAircraftProvider::storeChange(const CAircraftSituationChange &change)
    {
        // a change with the same timestamp will be replaced
        const CCallsign cs(change.getCallsign());
        QWriteLocker lock(&m_lockChanges);
        CAircraftSituationChangeList &changeList = m_changesByCallsign[cs];
        changeList.push_frontKeepLatestAdjustedFirst(change, true, IRemoteAircraftProvider::MaxSituationsPerCallsign);
    }

    bool CRemoteAircraftProvider::guessOnGroundAndUpdateModelCG(CAircraftSituation &situation, const CAircraftSituationChange &change, const CAircraftModel &aircraftModel)
    {
        if (aircraftModel.hasCG() && !situation.hasCG()) { situation.setCG(aircraftModel.getCG()); }
        if (!situation.shouldGuessOnGround()) { return false; }
        return change.guessOnGround(situation, aircraftModel);
    }

    bool CRemoteAircraftProvider::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRendering)
    {
        // here just synonym
        return this->setAircraftEnabledFlag(callsign, enabledForRendering);
    }

    bool CRemoteAircraftProvider::setAircraftEnabledFlag(const CCallsign &callsign, bool enabledForRendering)
    {
        QWriteLocker l(&m_lockAircraft);
        if (!m_aircraftInRange.contains(callsign)) { return false; }
        return m_aircraftInRange[callsign].setEnabled(enabledForRendering);
    }

    int CRemoteAircraftProvider::updateMultipleAircraftEnabled(const CCallsignSet &callsigns, bool enabledForRendering)
    {
        if (callsigns.isEmpty()) { return 0; }
        QWriteLocker l(&m_lockAircraft);
        int c = 0;
        for (const CCallsign &cs : callsigns)
        {
            if (!m_aircraftInRange.contains(cs)) { continue; }
            if (m_aircraftInRange[cs].setEnabled(enabledForRendering)) { c++; }
        }
        return c;
    }

    bool CRemoteAircraftProvider::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
    {
        if (CIdentifiable::isMyIdentifier(originator)) { return false; }
        const CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexModel, CVariant::from(model));
        const int c = this->updateAircraftInRange(callsign, vm);
        return c > 0;
    }

    bool CRemoteAircraftProvider::updateAircraftNetworkModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
    {
        if (CIdentifiable::isMyIdentifier(originator)) { return false; }
        const CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexNetworkModel, CVariant::from(model));
        const int c = this->updateAircraftInRange(callsign, vm);
        return c > 0;
    }

    bool CRemoteAircraftProvider::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositonUpdates)
    {
        QWriteLocker l(&m_lockAircraft);
        if (!m_aircraftInRange.contains(callsign)) { return false; }
        return m_aircraftInRange[callsign].setFastPositionUpdates(enableFastPositonUpdates);
    }

    bool CRemoteAircraftProvider::updateAircraftRendered(const CCallsign &callsign, bool rendered)
    {
        QWriteLocker l(&m_lockAircraft);
        if (!m_aircraftInRange.contains(callsign)) { return false; }
        return m_aircraftInRange[callsign].setRendered(rendered);
    }

    int CRemoteAircraftProvider::updateMultipleAircraftRendered(const CCallsignSet &callsigns, bool rendered)
    {
        if (callsigns.isEmpty()) { return 0; }
        int c = 0;
        for (const CCallsign &cs : callsigns)
        {
            if (!m_aircraftInRange.contains(cs)) { continue; }
            if (m_aircraftInRange[cs].setRendered(rendered)) { c++; }
        }
        return c;
    }

    int CRemoteAircraftProvider::updateAircraftGroundElevation(const CCallsign &callsign, const CElevationPlane &elevation, CAircraftSituation::GndElevationInfo info, bool *setForOnGroundPosition)
    {
        if (!this->isAircraftInRange(callsign)) { return 0; }

        // update aircraft situation
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        const CAircraftModel model = this->getAircraftInRangeModelForCallsign(callsign);
        CAircraftSituationChange change;
        bool setForOnGndPosition = false;

        int updated = 0;
        {
            QWriteLocker l(&m_lockSituations);
            CAircraftSituationList &situations = m_situationsByCallsign[callsign];
            if (situations.isEmpty()) { return 0; }
            updated = setGroundElevationCheckedAndGuessGround(situations, elevation, info, model, &change, &setForOnGndPosition);
            if (updated < 1) { return 0; }
            m_situationsLastModified[callsign] = now;
            const CAircraftSituation latestSituation = situations.front();
            if (info == CAircraftSituation::FromProvider && latestSituation.isOnGround())
            {
                m_latestOnGroundProviderElevation[callsign] = latestSituation;
            }
        }

        // update change
        if (!change.isNull())
        {
            this->storeChange(change);
        }

        // aircraft updates
        QWriteLocker l(&m_lockAircraft);
        if (m_aircraftInRange.contains(callsign))
        {
            m_aircraftInRange[callsign].setGroundElevationChecked(elevation, info);
        }

        if (setForOnGroundPosition) { *setForOnGroundPosition = setForOnGndPosition; }
        return updated; // updated situations
    }

    bool CRemoteAircraftProvider::updateCG(const CCallsign &callsign, const CLength &cg)
    {
        QWriteLocker l(&m_lockAircraft);
        if (!m_aircraftInRange.contains(callsign)) { return false; }
        m_aircraftInRange[callsign].setCG(cg);
        return true;
    }

    bool CRemoteAircraftProvider::updateCGAndModelString(const CCallsign &callsign, const CLength &cg, const QString &modelString)
    {
        QWriteLocker l(&m_lockAircraft);
        if (!m_aircraftInRange.contains(callsign)) { return false; }
        CSimulatedAircraft &aircraft = m_aircraftInRange[callsign];
        if (!cg.isNull()) { aircraft.setCG(cg); }
        if (!modelString.isEmpty()) { aircraft.setModelString(modelString); }
        return true;
    }

    CCallsignSet CRemoteAircraftProvider::updateCGForModel(const QString &modelString, const CLength &cg)
    {
        CCallsignSet callsigns;
        if (modelString.isEmpty()) { return callsigns; }

        QWriteLocker l(&m_lockAircraft);
        for (CSimulatedAircraft &aircraft : m_aircraftInRange)
        {
            if (caseInsensitiveStringCompare(aircraft.getModelString(), modelString))
            {
                aircraft.setCG(cg);
                callsigns.push_back(aircraft.getCallsign());
            }
        }
        return callsigns;
    }

    CLength CRemoteAircraftProvider::getCGFromDB(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockAircraft);
        return m_dbCGPerCallsign.contains(callsign) ? m_dbCGPerCallsign[callsign] : CLength::null();
    }

    CLength CRemoteAircraftProvider::getCGFromDB(const QString &modelString) const
    {
        QReadLocker l(&m_lockAircraft);
        return m_dbCGPerCallsign.contains(modelString) ? m_dbCGPerCallsign[modelString] : CLength::null();
    }

    void CRemoteAircraftProvider::rememberCGFromDB(const CLength &cgFromDB, const CCallsign &callsign)
    {
        QWriteLocker l(&m_lockAircraft);
        m_dbCGPerCallsign[callsign] = cgFromDB;
    }

    void CRemoteAircraftProvider::rememberCGFromDB(const CLength &cgFromDB, const QString &modelString)
    {
        QWriteLocker l(&m_lockAircraft);
        m_dbCGPerModelString[modelString] = cgFromDB;
    }

    void CRemoteAircraftProvider::updateMarkAllAsNotRendered()
    {
        const CCallsignSet callsigns = this->getAircraftInRangeCallsigns();
        QWriteLocker l(&m_lockAircraft);
        for (const CCallsign &cs : callsigns)
        {
            m_aircraftInRange[cs].setRendered(false);
        }
    }

    void CRemoteAircraftProvider::enableReverseLookupMessages(ReverseLookupLogging enable)
    {
        QWriteLocker l(&m_lockMessages);
        m_enableReverseLookupMsgs = enable;
    }

    ReverseLookupLogging CRemoteAircraftProvider::isReverseLookupMessagesEnabled() const
    {
        QReadLocker l(&m_lockMessages);
        return m_enableReverseLookupMsgs;
    }

    CStatusMessageList CRemoteAircraftProvider::getReverseLookupMessages(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockMessages);
        return m_reverseLookupMessages.value(callsign);
    }

    void CRemoteAircraftProvider::addReverseLookupMessages(const CCallsign &callsign, const CStatusMessageList &messages)
    {
        if (callsign.isEmpty()) { return; }
        if (messages.isEmpty()) { return; }
        QWriteLocker l(&m_lockMessages);
        if (!m_enableReverseLookupMsgs) { return; }
        if (m_reverseLookupMessages.contains(callsign))
        {
            CStatusMessageList &msgs = m_reverseLookupMessages[callsign];
            msgs.push_back(messages);
        }
        else
        {
            m_reverseLookupMessages.insert(callsign, messages);
        }
    }

    void CRemoteAircraftProvider::addReverseLookupMessage(const CCallsign &callsign, const CStatusMessage &message)
    {
        if (callsign.isEmpty()) { return; }
        if (message.isEmpty()) { return; }
        this->addReverseLookupMessages(callsign, CStatusMessageList({ message }));
    }

    void CRemoteAircraftProvider::addReverseLookupMessage(const CCallsign &callsign, const QString &message, CStatusMessage::StatusSeverity severity)
    {
        if (callsign.isEmpty()) { return; }
        if (message.isEmpty()) { return; }
        const CStatusMessage m = CCallsign::logMessage(callsign, message, getLogCategories(), severity);
        this->addReverseLookupMessage(callsign, m);
    }

    void CRemoteAircraftProvider::clear()
    {
        this->removeAllAircraft();
    }

    bool CRemoteAircraftProvider::hasTestAltitudeOffset(const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return false; }
        QReadLocker l(&m_lockSituations);
        return m_testOffset.contains(callsign);
    }

    bool CRemoteAircraftProvider::hasTestAltitudeOffsetGlobalValue() const
    {
        QReadLocker l(&m_lockSituations);
        return m_testOffset.contains(testAltitudeOffsetCallsign());
    }

    CAircraftSituation CRemoteAircraftProvider::addTestAltitudeOffsetToSituation(const CAircraftSituation &situation) const
    {
        const CCallsign cs(situation.getCallsign());
        const bool globalOffset = this->hasTestAltitudeOffsetGlobalValue();
        if (!globalOffset && !this->hasTestAltitudeOffset(cs)) { return situation; }

        QReadLocker l(&m_lockSituations);
        const CLength os = m_testOffset.contains(cs) ? m_testOffset.value(cs) : m_testOffset.value(testAltitudeOffsetCallsign());
        if (os.isNull() || os.isZeroEpsilonConsidered()) { return situation; }
        return situation.withAltitudeOffset(os);
    }

    ReverseLookupLogging CRemoteAircraftProvider::whatToReverseLog() const
    {
        QReadLocker l(&m_lockMessages);
        return m_enableReverseLookupMsgs;
    }

    int CRemoteAircraftProvider::setGroundElevationCheckedAndGuessGround(
        CAircraftSituationList &situations, const CElevationPlane &elevationPlane, CAircraftSituation::GndElevationInfo info, const CAircraftModel &model,
        CAircraftSituationChange *changeOut, bool *setForOnGroundPosition)
    {
        if (setForOnGroundPosition) { *setForOnGroundPosition = false; } // set a default
        if (elevationPlane.isNull()) { return 0; }
        if (situations.isEmpty()) { return 0; }

        // the change has the timestamps of the latest situation
        // Q_ASSERT_X(situations.m_tsAdjustedSortHint == CAircraftSituationList::AdjustedTimestampLatestFirst || situations.isSortedAdjustedLatestFirstWithoutNullPositions(), Q_FUNC_INFO, "Need sorted situations without NULL positions");
        const CAircraftSituationChange simpleChange(situations, model.getCG(), model.isVtol(), true, false);
        int c = 0; // changed elevations
        bool latest = true;
        bool setForOnGndPosition = false;

        for (CAircraftSituation &s : situations)
        {
            const bool set = s.setGroundElevationChecked(elevationPlane, info);
            if (set)
            {
                // simpleChange is only valid for the latest situation
                // this will do nothing if not appropriate!
                const bool guessed = (latest ? simpleChange : CAircraftSituationChange::null()).guessOnGround(s, model);
                Q_UNUSED(guessed)
                c++;

                // if not guessed and "on ground" we mark the "elevation"
                // as an elevation for a ground position
                if (!setForOnGndPosition && s.hasInboundGroundDetails() && s.isOnGround())
                {
                    setForOnGndPosition = true;
                }
            }
            latest = false; // only first pos. is "the latest" one
        }

        if (setForOnGroundPosition) { *setForOnGroundPosition = setForOnGndPosition; }
        if (changeOut)
        {
            const CAircraftSituationChange change(situations, model.getCG(), model.isVtol(), true, true);
            *changeOut = change;
        }

        return c;
    }

    CStatusMessageList CRemoteAircraftProvider::getAircraftPartsHistory(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockPartsHistory);
        return m_aircraftPartsMessages.value(callsign);
    }

    bool CRemoteAircraftProvider::isAircraftPartsHistoryEnabled() const
    {
        QReadLocker l(&m_lockPartsHistory);
        return m_enableAircraftPartsHistory;
    }

    void CRemoteAircraftProvider::enableAircraftPartsHistory(bool enabled)
    {
        QWriteLocker l(&m_lockPartsHistory);
        m_enableAircraftPartsHistory = enabled;
    }

    int CRemoteAircraftProvider::aircraftSituationsAdded() const
    {
        QReadLocker l(&m_lockSituations);
        return m_situationsAdded;
    }

    qint64 CRemoteAircraftProvider::situationsLastModified(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockSituations);
        return m_situationsLastModified.value(callsign, -1);
    }

    qint64 CRemoteAircraftProvider::partsLastModified(const CCallsign &callsign) const
    {
        QReadLocker l(&m_lockParts);
        return m_partsLastModified.value(callsign, -1);
    }

    CElevationPlane CRemoteAircraftProvider::averageElevationOfNonMovingAircraft(const CAircraftSituation &reference, const CLength &range, int minValues, int sufficientValues) const
    {
        const CAircraftSituationList situations = this->latestOnGroundProviderElevations();
        return situations.averageElevationOfTaxiingOnGroundAircraft(reference, range, minValues, sufficientValues);
    }

    bool CRemoteAircraftProvider::testAddAltitudeOffset(const CCallsign &callsign, const CLength &offset)
    {
        const bool remove = offset.isNull() || offset.isZeroEpsilonConsidered();
        QWriteLocker l(&m_lockSituations);
        if (remove)
        {
            m_testOffset.remove(callsign);
            return false;
        }

        m_testOffset[callsign] = offset;
        return true;
    }

    int CRemoteAircraftProvider::aircraftPartsAdded() const
    {
        QReadLocker l(&m_lockParts);
        return m_partsAdded;
    }

    bool CRemoteAircraftProvider::isAircraftInRange(const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return false; }
        QReadLocker l(&m_lockAircraft);
        return m_aircraftInRange.contains(callsign);
    }

    bool CRemoteAircraftProvider::isVtolAircraft(const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return false; }
        const CSimulatedAircraft aircraft = this->getAircraftInRangeForCallsign(callsign);
        return aircraft.isVtol();
    }

    QList<QMetaObject::Connection> CRemoteAircraftProvider::connectRemoteAircraftProviderSignals(
        QObject *receiver,
        std::function<void(const CAircraftSituation &)> addedSituationFunction,
        std::function<void(const CCallsign &, const CAircraftParts &)> addedPartsFunction,
        std::function<void(const CCallsign &)> removedAircraftFunction,
        std::function<void(const CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot)
    {
        Q_ASSERT_X(receiver, Q_FUNC_INFO, "Missing receiver");

        // bind does not allow to define connection type, so we use receiver as workaround
        const QMetaObject::Connection uc; // unconnected
        const QMetaObject::Connection c1 = addedSituationFunction ? connect(this, &CRemoteAircraftProvider::addedAircraftSituation, receiver, addedSituationFunction, Qt::QueuedConnection) : uc;
        Q_ASSERT_X(c1 || !addedSituationFunction, Q_FUNC_INFO, "connect failed");
        const QMetaObject::Connection c2 = addedPartsFunction ? connect(this, &CRemoteAircraftProvider::addedAircraftParts, receiver, addedPartsFunction, Qt::QueuedConnection) : uc;
        Q_ASSERT_X(c2 || !addedPartsFunction, Q_FUNC_INFO, "connect failed");
        const QMetaObject::Connection c3 = removedAircraftFunction ? connect(this, &CRemoteAircraftProvider::removedAircraft, receiver, removedAircraftFunction, Qt::QueuedConnection) : uc;
        Q_ASSERT_X(c3 || !removedAircraftFunction, Q_FUNC_INFO, "connect failed");
        const QMetaObject::Connection c4 = aircraftSnapshotSlot ? connect(this, &CRemoteAircraftProvider::airspaceAircraftSnapshot, receiver, aircraftSnapshotSlot, Qt::QueuedConnection) : uc;
        Q_ASSERT_X(c4 || !aircraftSnapshotSlot, Q_FUNC_INFO, "connect failed");
        return QList<QMetaObject::Connection>({ c1, c2, c3, c4 });
    }

    bool CRemoteAircraftProvider::removeAircraft(const CCallsign &callsign)
    {
        {
            QWriteLocker l1(&m_lockParts);
            m_partsByCallsign.remove(callsign);
            m_aircraftWithParts.remove(callsign);
            m_partsLastModified.remove(callsign);
        }
        {
            QWriteLocker l2(&m_lockSituations);
            m_situationsByCallsign.remove(callsign);
            m_latestSituationByCallsign.remove(callsign);
            m_latestOnGroundProviderElevation.remove(callsign);
            m_situationsLastModified.remove(callsign);
        }
        {
            QWriteLocker l4(&m_lockPartsHistory);
            m_aircraftPartsMessages.remove(callsign);
        }
        bool removedCallsign = false;
        {
            QWriteLocker l(&m_lockAircraft);
            m_dbCGPerCallsign.remove(callsign);
            const int c = m_aircraftInRange.remove(callsign);
            removedCallsign = c > 0;
        }
        return removedCallsign;
    }

    CRemoteAircraftAware::~CRemoteAircraftAware()
    {}

    CSimulatedAircraftList CRemoteAircraftAware::getAircraftInRange() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getAircraftInRange();
    }

    bool CRemoteAircraftAware::isAircraftInRange(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->isAircraftInRange(callsign);
    }

    bool CRemoteAircraftAware::isVtolAircraft(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->isVtolAircraft(callsign);
    }

    int CRemoteAircraftAware::getAircraftInRangeCount() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getAircraftInRangeCount();
    }

    CCallsignSet CRemoteAircraftAware::getAircraftInRangeCallsigns() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getAircraftInRangeCallsigns();
    }

    CSimulatedAircraft CRemoteAircraftAware::getAircraftInRangeForCallsign(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getAircraftInRangeForCallsign(callsign);
    }

    CAircraftModel CRemoteAircraftAware::getAircraftInRangeModelForCallsign(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getAircraftInRangeModelForCallsign(callsign);
    }

    CAirspaceAircraftSnapshot CRemoteAircraftAware::getLatestAirspaceAircraftSnapshot() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getLatestAirspaceAircraftSnapshot();
    }

    CAircraftSituationList CRemoteAircraftAware::remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->remoteAircraftSituations(callsign);
    }

    CAircraftSituation CRemoteAircraftAware::remoteAircraftSituation(const CCallsign &callsign, int index) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->remoteAircraftSituation(callsign, index);
    }

    CAircraftSituationList CRemoteAircraftAware::latestRemoteAircraftSituations() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->latestRemoteAircraftSituations();
    }

    CAircraftSituationList CRemoteAircraftAware::latestOnGroundProviderElevations() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->latestOnGroundProviderElevations();
    }

    CAircraftSituationChangeList CRemoteAircraftAware::remoteAircraftSituationChanges(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->remoteAircraftSituationChanges(callsign);
    }

    CAircraftPartsList CRemoteAircraftAware::remoteAircraftParts(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->remoteAircraftParts(callsign);
    }

    int CRemoteAircraftAware::remoteAircraftPartsCount(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->remoteAircraftPartsCount(callsign);
    }

    CCallsignSet CRemoteAircraftAware::remoteAircraftSupportingParts() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->remoteAircraftSupportingParts();
    }

    int CRemoteAircraftAware::remoteAircraftSituationsCount(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->remoteAircraftSituationsCount(callsign);
    }

    bool CRemoteAircraftAware::updateAircraftModel(const Aviation::CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateAircraftModel(callsign, model, originator);
    }

    bool CRemoteAircraftAware::updateAircraftNetworkModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateAircraftNetworkModel(callsign, model, originator);
    }

    bool CRemoteAircraftAware::updateAircraftRendered(const CCallsign &callsign, bool rendered)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateAircraftRendered(callsign, rendered);
    }

    bool CRemoteAircraftAware::updateMultipleAircraftRendered(const CCallsignSet &callsigns, bool rendered)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateMultipleAircraftRendered(callsigns, rendered);
    }

    int CRemoteAircraftAware::updateAircraftGroundElevation(const CCallsign &callsign, const CElevationPlane &elevation, CAircraftSituation::GndElevationInfo info, bool *updatedAircraftGroundElevation)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateAircraftGroundElevation(callsign, elevation, info, updatedAircraftGroundElevation);
    }

    bool CRemoteAircraftAware::updateCG(const CCallsign &callsign, const CLength &cg)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateCG(callsign, cg);
    }

    CCallsignSet CRemoteAircraftAware::updateCGForModel(const QString &modelString, const CLength &cg)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateCGForModel(modelString, cg);
    }

    bool CRemoteAircraftAware::updateCGAndModelString(const CCallsign &callsign, const CLength &cg, const QString &modelString)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateCGAndModelString(callsign, cg, modelString);
    }

    void CRemoteAircraftAware::updateMarkAllAsNotRendered()
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        this->provider()->updateMarkAllAsNotRendered();
    }

    int CRemoteAircraftAware::aircraftSituationsAdded() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->aircraftSituationsAdded();
    }

    int CRemoteAircraftAware::aircraftPartsAdded() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->aircraftPartsAdded();
    }

    qint64 CRemoteAircraftAware::situationsLastModified(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->situationsLastModified(callsign);
    }

    qint64 CRemoteAircraftAware::partsLastModified(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->partsLastModified(callsign);
    }

    CElevationPlane CRemoteAircraftAware::averageElevationOfNonMovingAircraft(const CAircraftSituation &reference, const CLength &range, int minValues) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->averageElevationOfNonMovingAircraft(reference, range, minValues);
    }

    bool CRemoteAircraftAware::isRemoteAircraftSupportingParts(const CCallsign &callsign) const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->isRemoteAircraftSupportingParts(callsign);
    }

    int CRemoteAircraftAware::getRemoteAircraftSupportingPartsCount() const
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->getRemoteAircraftSupportingPartsCount();
    }

    bool CRemoteAircraftAware::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRendering)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateAircraftEnabled(callsign, enabledForRendering);
    }

    bool CRemoteAircraftAware::setAircraftEnabledFlag(const CCallsign &callsign, bool enabledForRendering)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->setAircraftEnabledFlag(callsign, enabledForRendering);
    }

    bool CRemoteAircraftAware::updateMultipleAircraftEnabled(const CCallsignSet &callsigns, bool enabledForRendering)
    {
        Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
        return this->provider()->updateMultipleAircraftEnabled(callsigns, enabledForRendering);
    }

    CAircraftParts IRemoteAircraftProvider::getLatestAircraftParts(const CCallsign &callsign) const
    {
        static const CAircraftParts empty;
        const CAircraftPartsList parts = this->remoteAircraftParts(callsign);
        return parts.isEmpty() ? empty : parts.latestObject();
    }

    void IRemoteAircraftProvider::removeOutdatedParts(CAircraftPartsList &partsList)
    {
        // remove all outdated parts, but keep at least one
        if (partsList.isEmpty()) { return; }

        // we expect the latest value at front
        // but to make sure we do the search
        const qint64 ts = partsList.latestTimestampMsecsSinceEpoch() - MaxPartsAgePerCallsignSecs * 1000;
        partsList.removeBefore(ts);
        Q_ASSERT_X(!partsList.isEmpty(), Q_FUNC_INFO, "Need at least 1 value");
    }

    const CCallsign &IRemoteAircraftProvider::testAltitudeOffsetCallsign()
    {
        static const CCallsign wildcard("ZZZZ");
        return wildcard;
    }
} // namespace
