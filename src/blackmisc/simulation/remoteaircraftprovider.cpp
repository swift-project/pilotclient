/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/remoteaircraftprovider.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Simulation
    {
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

        bool CRemoteAircraftAware::updateAircraftGroundElevation(const CCallsign &callsign, const CElevationPlane &elevation)
        {
            Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
            return this->provider()->updateAircraftGroundElevation(callsign, elevation);
        }

        void CRemoteAircraftAware::updateMarkAllAsNotRendered()
        {
            Q_ASSERT_X(this->provider(), Q_FUNC_INFO, "No object available");
            this->provider()->updateMarkAllAsNotRendered();
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
