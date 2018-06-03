/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/matchingutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/json.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Json;
using namespace BlackConfig;

namespace BlackMisc
{
    namespace Simulation
    {
        CAircraftSituationChange IRemoteAircraftProvider::remoteAircraftSituationChange(const CCallsign &callsign) const
        {
            const CAircraftSituationList situations(this->remoteAircraftSituations(callsign));
            if (situations.isEmpty()) { return CAircraftSituationChange::null(); }
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                Q_ASSERT_X(situations.isSortedAdjustedLatestFirstWithoutNullPositions(), Q_FUNC_INFO, "Expect latest first");
            }
            const CAircraftModel model = this->getAircraftInRangeModelForCallsign(callsign);
            const CAircraftSituationChange change = CAircraftSituationChange(situations, model.getCG(), model.isVtol(), true, true);
            return change;
        }

        const CLogCategoryList &CRemoteAircraftProvider::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::matching(), CLogCategory::network() };
            return cats;
        }

        CRemoteAircraftProvider::CRemoteAircraftProvider(QObject *parent) : QObject(parent), CIdentifiable(this)
        { }

        CSimulatedAircraftList CRemoteAircraftProvider::getAircraftInRange() const
        {
            QReadLocker l(&m_lockAircraft);
            return m_aircraftInRange;
        }

        CCallsignSet CRemoteAircraftProvider::getAircraftInRangeCallsigns() const
        {
            return this->getAircraftInRange().getCallsigns();
        }

        CSimulatedAircraft CRemoteAircraftProvider::getAircraftInRangeForCallsign(const CCallsign &callsign) const
        {
            const CSimulatedAircraft aircraft = this->getAircraftInRange().findFirstByCallsign(callsign);
            return aircraft;
        }

        CAircraftModel CRemoteAircraftProvider::getAircraftInRangeModelForCallsign(const CCallsign &callsign) const
        {
            const CSimulatedAircraft aircraft(getAircraftInRangeForCallsign(callsign)); // threadsafe
            return aircraft.getModel();
        }

        CAircraftSituationList CRemoteAircraftProvider::remoteAircraftSituations(const CCallsign &callsign) const
        {
            QReadLocker l(&m_lockSituations);
            static const CAircraftSituationList empty;
            if (!m_situationsByCallsign.contains(callsign)) { return empty; }
            return m_situationsByCallsign[callsign];
        }

        int CRemoteAircraftProvider::remoteAircraftSituationsCount(const CCallsign &callsign) const
        {
            QReadLocker l(&m_lockSituations);
            if (!m_situationsByCallsign.contains(callsign)) { return -1; }
            return m_situationsByCallsign[callsign].size();
        }

        CAircraftPartsList CRemoteAircraftProvider::remoteAircraftParts(const CCallsign &callsign, qint64 cutoffTimeValuesBefore) const
        {
            static const CAircraftPartsList empty;
            QReadLocker l(&m_lockParts);
            if (!m_partsByCallsign.contains(callsign)) { return empty; }
            if (cutoffTimeValuesBefore < 0)
            {
                return m_partsByCallsign[callsign];
            }
            else
            {
                return m_partsByCallsign[callsign].findBefore(cutoffTimeValuesBefore);
            }
        }

        int CRemoteAircraftProvider::remoteAircraftPartsCount(const CCallsign &callsign, qint64 cutoffTimeValuesBefore) const
        {
            const int s = this->remoteAircraftParts(callsign, cutoffTimeValuesBefore).size();
            return s;
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

        int CRemoteAircraftProvider::getAircraftInRangeCount() const
        {
            return this->getAircraftInRange().size();
        }

        void CRemoteAircraftProvider::removeAllAircraft()
        {
            for (const CSimulatedAircraft &aircraft : getAircraftInRange())
            {
                const CCallsign cs(aircraft.getCallsign());
                emit this->removedAircraft(cs);
            }

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
                m_situationsAdded = 0;
                m_situationsLastModified.clear();
                m_testOffset.clear();
            }
            { QWriteLocker l(&m_lockPartsHistory); m_aircraftPartsHistory.clear(); }
            { QWriteLocker l(&m_lockMessages); m_reverseLookupMessages.clear(); }
            { QWriteLocker l(&m_lockAircraft); m_aircraftInRange.clear(); }
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
                m_aircraftInRange.push_back(aircraft);
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
                c = m_aircraftInRange.applyIfCallsign(callsign, vm, skipEqualValues);
            }
            if (c > 0)
            {
                emit this->changedAircraftInRange();
            }
            return c;
        }

        void CRemoteAircraftProvider::storeAircraftSituation(const CAircraftSituation &situation, bool allowTestOffset)
        {
            const CCallsign cs = situation.getCallsign();
            if (cs.isEmpty()) { return; }

            // verify
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                BLACK_VERIFY_X(situation.getTimeOffsetMs() > 0, Q_FUNC_INFO, "Missing offset");
            }

            // add offset (for testing only)
            CAircraftSituation situationCorrected(allowTestOffset ? this->testAddAltitudeOffsetToSituation(situation) : situation);

            // list from new to old
            const qint64 ts = QDateTime::currentMSecsSinceEpoch();
            {
                QWriteLocker lock(&m_lockSituations);
                m_situationsAdded++;
                m_situationsLastModified[cs] = ts;
                CAircraftSituationList &situationList = m_situationsByCallsign[cs];
                const int situations = situationList.size();
                if (situations < 1)
                {
                    situationList.prefillLatestAdjustedFirst(situationCorrected, IRemoteAircraftProvider::MaxSituationsPerCallsign);
                }
                else
                {
                    situationList.front().transferGroundElevation(situationCorrected); // transfer last situation if possible
                    situationList.push_frontKeepLatestFirstAdjustOffset(situationCorrected, IRemoteAircraftProvider::MaxSituationsPerCallsign);

                    // unify all inbound ground information
                    if (situation.hasInboundGroundDetails())
                    {
                        situationList.setOnGroundDetails(situation.getOnGroundDetails());
                    }
                }

                // check sort order
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    BLACK_VERIFY_X(situationList.isSortedAdjustedLatestFirstWithoutNullPositions(), Q_FUNC_INFO, "wrong sort order");
                    BLACK_VERIFY_X(situationList.size() <= IRemoteAircraftProvider::MaxSituationsPerCallsign, Q_FUNC_INFO, "Wrong size");
                }
            } // lock

            emit this->addedAircraftSituation(situationCorrected);
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
                partsList.push_frontKeepLatestFirstAdjustOffset(parts, IRemoteAircraftProvider::MaxPartsPerCallsign);

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
                const int c = m_aircraftInRange.setAircraftPartsSynchronized(callsign, parts);
                Q_UNUSED(c);
            }

            // update parts
            {
                // aircraft supporting parts
                QWriteLocker l(&m_lockParts);
                m_aircraftWithParts.insert(callsign); // mark as callsign which supports parts
            }

            emit this->addedAircraftParts(callsign, parts);
        }

        void CRemoteAircraftProvider::storeAircraftParts(const CCallsign &callsign, const QJsonObject &jsonObject, int currentOffset)
        {
            const CSimulatedAircraft remoteAircraft(this->getAircraftInRangeForCallsign(callsign));
            const bool isFull = jsonObject.value(CAircraftParts::attributeNameIsFullJson()).toBool();

            // If we are not yet synchronized, we throw away any incremental packet
            if (!remoteAircraft.hasValidCallsign()) { return; }
            if (!remoteAircraft.isPartsSynchronized() && !isFull) { return; }

            CAircraftParts parts;
            try
            {
                if (isFull)
                {
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
                CStatusMessage message = ex.toStatusMessage(this, "Invalid parts packet");
                message.setSeverity(CStatusMessage::SeverityDebug);
                CLogMessage::preformatted(message);
            }

            // make sure in any case right time and correct details
            parts.setCurrentUtcTime();
            parts.setTimeOffsetMs(currentOffset);
            parts.setPartsDetails(CAircraftParts::FSDAircraftParts);

            // store part history (parts always absolute)
            this->storeAircraftParts(callsign, parts, false);

            // history
            if (this->isAircraftPartsHistoryEnabled())
            {
                const QJsonDocument doc(jsonObject);
                const QString partsAsString = doc.toJson(QJsonDocument::Compact);
                const CStatusMessage message(getLogCategories(), CStatusMessage::SeverityInfo, callsign.isEmpty() ? callsign.toQString() + ": " + partsAsString.trimmed() : partsAsString.trimmed());

                QReadLocker l(&m_lockPartsHistory);
                if (m_aircraftPartsHistory.contains(callsign))
                {
                    CStatusMessageList &msgs = m_aircraftPartsHistory[callsign];
                    msgs.push_back(message);
                }
                else
                {
                    m_aircraftPartsHistory.insert(callsign, message);
                }
            }
        }

        bool CRemoteAircraftProvider::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRendering)
        {
            const CPropertyIndexVariantMap vm(CSimulatedAircraft::IndexEnabled, CVariant::fromValue(enabledForRendering));
            const int c = this->updateAircraftInRange(callsign, vm);
            return c > 0;
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
            const int c = m_aircraftInRange.setFastPositionUpdates(callsign, enableFastPositonUpdates);
            return c > 0;
        }

        bool CRemoteAircraftProvider::updateAircraftRendered(const CCallsign &callsign, bool rendered)
        {
            QWriteLocker l(&m_lockAircraft);
            const int c = m_aircraftInRange.setRendered(callsign, rendered);
            return c > 0;
        }

        int CRemoteAircraftProvider::updateAircraftGroundElevation(const CCallsign &callsign, const CElevationPlane &elevation, CAircraftSituation::GndElevationInfo info)
        {
            if (!this->isAircraftInRange(callsign)) { return 0; }

            // update aircraft situation
            const qint64 ts = QDateTime::currentMSecsSinceEpoch();
            const CAircraftModel model = this->getAircraftInRangeModelForCallsign(callsign);
            int updated = 0;
            {
                QWriteLocker l(&m_lockSituations);
                CAircraftSituationList &situations = m_situationsByCallsign[callsign];
                updated = situations.setGroundElevationCheckedAndGuessGround(elevation, info, model);
                if (updated < 1) { return 0; }
                m_situationsLastModified[callsign] = ts;
            }

            // aircraft updates
            QWriteLocker l(&m_lockAircraft);
            const int c = m_aircraftInRange.setGroundElevationChecked(callsign, elevation, info);
            Q_UNUSED(c); // just for info, expect 1

            return updated; // updated situations
        }

        bool CRemoteAircraftProvider::updateCG(const CCallsign &callsign, const CLength &cg)
        {
            QWriteLocker l(&m_lockAircraft);
            const int c = m_aircraftInRange.setCG(callsign, cg);
            return c > 0;
        }

        void CRemoteAircraftProvider::updateMarkAllAsNotRendered()
        {
            QWriteLocker l(&m_lockAircraft);
            m_aircraftInRange.markAllAsNotRendered();
        }

        void CRemoteAircraftProvider::enableReverseLookupMessages(bool enabled)
        {
            QWriteLocker l(&m_lockMessages);
            m_enableReverseLookupMsgs = enabled;
        }

        bool CRemoteAircraftProvider::isReverseLookupMessagesEnabled() const
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
            const CStatusMessage m = CMatchingUtils::logMessage(callsign, message, getLogCategories(), severity);
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

        CAircraftSituation CRemoteAircraftProvider::testAddAltitudeOffsetToSituation(const CAircraftSituation &situation) const
        {
            // for global offset testing set "true"
            constexpr bool globalOffsetTest = false;

            const CCallsign cs(situation.getCallsign());
            if (!globalOffsetTest && !this->hasTestAltitudeOffset(cs)) { return situation; }
            CLength os;
            if (globalOffsetTest)
            {
                os = CLength(100, CLengthUnit::ft());
            }
            else
            {
                QReadLocker l(&m_lockSituations);
                os = m_testOffset.value(cs);
            }
            if (os.isNull() || os.isZeroEpsilonConsidered()) { return situation; }
            const CAltitude newAlt = situation.getAltitude().withOffset(os);
            CAircraftSituation newSituation(situation);
            newSituation.setAltitude(newAlt);
            return newSituation;
        }

        CStatusMessageList CRemoteAircraftProvider::getAircraftPartsHistory(const CCallsign &callsign) const
        {
            QReadLocker l(&m_lockPartsHistory);
            return m_aircraftPartsHistory.value(callsign);
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
            return m_aircraftInRange.containsCallsign(callsign);
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
            std::function<void (const CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot)
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
            { QWriteLocker l1(&m_lockParts); m_partsByCallsign.remove(callsign); m_aircraftWithParts.remove(callsign); m_partsLastModified.remove(callsign); }
            { QWriteLocker l2(&m_lockSituations); m_situationsByCallsign.remove(callsign); m_situationsLastModified.remove(callsign); }
            { QWriteLocker l4(&m_lockPartsHistory); m_aircraftPartsHistory.remove(callsign); }
            bool removedCallsign = false;
            {
                QWriteLocker l(&m_lockAircraft);
                const int c = m_aircraftInRange.removeByCallsign(callsign);
                removedCallsign = c > 0;
            }
            return removedCallsign;
        }

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

        CAircraftSituationChange CRemoteAircraftAware::remoteAircraftSituationChange(const CCallsign &callsign) const
        {
            Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
            return this->provider()->remoteAircraftSituationChange(callsign);
        }

        CAircraftPartsList CRemoteAircraftAware::remoteAircraftParts(const CCallsign &callsign, qint64 cutoffTimeBefore) const
        {
            Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
            return this->provider()->remoteAircraftParts(callsign, cutoffTimeBefore);
        }

        int CRemoteAircraftAware::remoteAircraftPartsCount(const CCallsign &callsign, qint64 cutoffTimeBefore) const
        {
            Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
            return this->provider()->remoteAircraftPartsCount(callsign, cutoffTimeBefore);
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

        int CRemoteAircraftAware::updateAircraftGroundElevation(const CCallsign &callsign, const CElevationPlane &elevation, CAircraftSituation::GndElevationInfo info)
        {
            Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
            return this->provider()->updateAircraftGroundElevation(callsign, elevation, info);
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

        bool CRemoteAircraftAware::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRedering)
        {
            Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
            return this->provider()->updateAircraftEnabled(callsign, enabledForRedering);
        }

        CAircraftParts IRemoteAircraftProvider::getLatestAircraftParts(const CCallsign &callsign) const
        {
            static const CAircraftParts empty;
            const CAircraftPartsList parts = this->remoteAircraftParts(callsign, -1);
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
            Q_ASSERT_X(partsList.size() >= 1, Q_FUNC_INFO, "Need at least 1 value");
        }
    } // namespace
} // namespace
