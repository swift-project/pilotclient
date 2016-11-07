/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/interpolator.h"
#include "blackmisc/aviation/callsign.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackMisc
{

    IInterpolator::IInterpolator(IRemoteAircraftProvider *provider, const QString &objectName, QObject *parent) :
        QObject(parent),
        CRemoteAircraftAware(provider)
    {
        Q_ASSERT_X(provider, Q_FUNC_INFO, "missing provider");
        this->setObjectName(objectName);
    }

    BlackMisc::Aviation::CAircraftSituation IInterpolator::getInterpolatedSituation(const BlackMisc::Aviation::CCallsign &callsign, qint64 currentTimeSinceEpoc,
            bool isVtolAircraft, InterpolationStatus &status) const
    {
        // has to be thread safe

        status.reset();
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");

        auto currentSituation = this->getInterpolatedSituation(this->remoteAircraftSituations(callsign), currentTimeSinceEpoc, isVtolAircraft, status);
        currentSituation.setCallsign(callsign); // make sure callsign is correct
        return currentSituation;
    }

    CAircraftPartsList IInterpolator::getPartsBeforeTime(const CAircraftPartsList &parts, qint64 cutoffTime, BlackMisc::IInterpolator::PartsStatus &partsStatus) const
    {
        partsStatus.reset();
        partsStatus.setSupportsParts(true);

        if (cutoffTime < 0) { return parts; }
        return parts.findBefore(cutoffTime);
    }

    CAircraftPartsList IInterpolator::getPartsBeforeTime(const CCallsign &callsign, qint64 cutoffTime, BlackMisc::IInterpolator::PartsStatus &partsStatus) const
    {
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
        partsStatus.reset();

        partsStatus.setSupportsParts(this->isRemoteAircraftSupportingParts(callsign));
        if (!partsStatus.isSupportingParts()) { return {}; }
        return this->remoteAircraftParts(callsign, cutoffTime);
    }

    void IInterpolator::setInterpolatorSetup(const CInterpolationAndRenderingSetup &setup)
    {
        QWriteLocker l(&m_lock);
        m_setup = setup;
    }

    CInterpolationAndRenderingSetup IInterpolator::getInterpolatorSetup() const
    {
        QReadLocker l(&m_lock);
        return m_setup;
    }

    bool IInterpolator::InterpolationStatus::allTrue() const
    {
        return m_interpolationSucceeded && m_changedPosition;
    }

    void IInterpolator::InterpolationStatus::reset()
    {
        m_changedPosition = false;
        m_interpolationSucceeded = false;
    }

    bool IInterpolator::PartsStatus::allTrue() const
    {
        return m_supportsParts;
    }

    void IInterpolator::PartsStatus::reset()
    {
        m_supportsParts = false;
    }
} // namespace
