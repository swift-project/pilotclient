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

    CAircraftPartsList IInterpolator::getPartsBeforeTime(const CCallsign &callsign, qint64 cutoffTime, BlackMisc::IInterpolator::PartsStatus &partsStatus)
    {
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "empty callsign");
        partsStatus.reset();

        partsStatus.supportsParts = this->isRemoteAircraftSupportingParts(callsign);
        if (!partsStatus.supportsParts) { return {}; }
        return this->remoteAircraftParts(callsign, cutoffTime);
    }

    void IInterpolator::enableDebugMessages(bool enabled)
    {
        this->m_withDebugMsg = enabled;
    }

    bool IInterpolator::InterpolationStatus::allTrue() const
    {
        return interpolationSucceeded && changedPosition;
    }

    void IInterpolator::InterpolationStatus::reset()
    {
        changedPosition = false;
        interpolationSucceeded = false;
    }

    bool IInterpolator::PartsStatus::allTrue() const
    {
        return supportsParts;
    }

    void IInterpolator::PartsStatus::reset()
    {
        supportsParts = false;
    }

} // namespace
