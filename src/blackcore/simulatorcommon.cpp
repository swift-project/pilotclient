/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/simulatorcommon.h"
#include "blackcore/db/databaseutils.h"
#include "blackcore/db/databaseutils.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/interpolator.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/threadutils.h"

#include <QDateTime>
#include <QString>
#include <QThread>
#include <functional>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;
using namespace BlackCore;
using namespace BlackCore::Db;

namespace BlackCore
{
    CSimulatorCommon::CSimulatorCommon(const CSimulatorPluginInfo &info,
                                       BlackMisc::Simulation::IOwnAircraftProvider    *ownAircraftProvider,
                                       BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                       IWeatherGridProvider                           *weatherGridProvider,
                                       QObject *parent)
        : ISimulator(parent),
          COwnAircraftAware(ownAircraftProvider),
          CRemoteAircraftAware(remoteAircraftProvider),
          CWeatherGridAware(weatherGridProvider),
          m_simulatorPluginInfo(info)
    {
        this->setObjectName("Simulator: " + info.getIdentifier());

        // provider signals, hook up with remote aircraft provider
        m_remoteAircraftProviderConnections.append(
            this->m_remoteAircraftProvider->connectRemoteAircraftProviderSignals(
                this, // receiver must match object in bind
                std::bind(&CSimulatorCommon::ps_remoteProviderAddAircraftSituation, this, std::placeholders::_1),
                std::bind(&CSimulatorCommon::ps_remoteProviderAddAircraftParts, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&CSimulatorCommon::ps_remoteProviderRemovedAircraft, this, std::placeholders::_1),
                std::bind(&CSimulatorCommon::ps_recalculateRenderedAircraft, this, std::placeholders::_1))
        );

        // timer
        this->m_oneSecondTimer.setObjectName(this->objectName().append(":m_oneSecondTimer"));
        connect(&m_oneSecondTimer, &QTimer::timeout, this, &CSimulatorCommon::ps_oneSecondTimer);
        this->m_oneSecondTimer.start(1000);

        // swift data
        if (sApp && sApp->getWebDataServices())
        {
            connect(sApp->getWebDataServices(), &CWebDataServices::allSwiftDbDataRead, this, &CSimulatorCommon::ps_allSwiftDataRead);
        }

        // info
        CLogMessage(this).info("Initialized simulator driver %1") << m_simulatorPluginInfo.toQString();
    }

    CSimulatorCommon::~CSimulatorCommon() { }

    bool CSimulatorCommon::logicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        Q_ASSERT_X(remoteAircraft.hasModelString(), Q_FUNC_INFO, "Missing model string");
        Q_ASSERT_X(remoteAircraft.hasCallsign(), Q_FUNC_INFO, "Missing callsign");
        if (!remoteAircraft.isEnabled()) { return false; }

        // if not restriced, directly change
        if (!m_interpolationRenderingSetup.isRenderingRestricted()) { this->physicallyAddRemoteAircraft(remoteAircraft); return true; }

        // will be added with next snapshot ps_recalculateRenderedAircraft
        return false;
    }

    bool CSimulatorCommon::logicallyRemoveRemoteAircraft(const CCallsign &callsign)
    {
        // if not restriced, directly change
        if (!m_interpolationRenderingSetup.isRenderingRestricted()) { this->physicallyRemoveRemoteAircraft(callsign); return true; }

        // will be added with next snapshot ps_recalculateRenderedAircraft
        return false;
    }

    void CSimulatorCommon::blinkHighlightedAircraft()
    {
        if (m_highlightedAircraft.isEmpty() || m_highlightEndTimeMsEpoch < 1) { return; }
        m_blinkCycle = !m_blinkCycle;

        if (QDateTime::currentMSecsSinceEpoch() < m_highlightEndTimeMsEpoch)
        {
            // blink mode, toggle aircraft
            for (const CSimulatedAircraft &aircraft : m_highlightedAircraft)
            {
                if (m_blinkCycle)
                {
                    this->physicallyRemoveRemoteAircraft(aircraft.getCallsign());
                }
                else
                {
                    this->physicallyAddRemoteAircraft(aircraft);
                }
            }
        }
        else
        {
            // restore
            for (const CSimulatedAircraft &aircraft : m_highlightedAircraft)
            {
                // get the current state for this aircraft
                // it might has been removed in the meantime
                const CCallsign cs(aircraft.getCallsign());
                resetAircraftFromProvider(cs);
            }
            m_highlightedAircraft.clear();
            m_highlightEndTimeMsEpoch = 0;
        }
    }

    void CSimulatorCommon::resetAircraftFromProvider(const CCallsign &callsign)
    {
        CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
        bool enabled = aircraft.isEnabled();
        if (enabled)
        {
            // are we already visible?
            if (!isPhysicallyRenderedAircraft(callsign))
            {
                this->physicallyAddRemoteAircraft(aircraft);
            }
        }
        else
        {
            this->physicallyRemoveRemoteAircraft(callsign);
        }
    }

    bool CSimulatorCommon::setInitialAircraftSituation(CSimulatedAircraft &aircraft) const
    {
        if (!this->m_interpolator) { return false; }
        const CCallsign callsign(aircraft.getCallsign());
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Missing callsign");

        // with an interpolator the interpolated situation is used
        // to avoid position jittering when displayed
        const qint64 time = QDateTime::currentMSecsSinceEpoch();
        IInterpolator::InterpolationStatus interpolationStatus;
        const CAircraftSituation as(m_interpolator->getInterpolatedSituation(callsign, time, aircraft.isVtol(), interpolationStatus));
        if (interpolationStatus.didInterpolationSucceed())
        {
            aircraft.setSituation(as);
            return true;
        }
        else
        {
            return false;
        }
    }

    void CSimulatorCommon::reverseLookupAndUpdateOwnAircraftModel(const QString &modelString)
    {
        CAircraftModel model = getOwnAircraftModel();
        model.setModelString(modelString);
        this->reverseLookupAndUpdateOwnAircraftModel(model);
    }

    void CSimulatorCommon::reverseLookupAndUpdateOwnAircraftModel(const BlackMisc::Simulation::CAircraftModel &model)
    {
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing sApp");
        Q_ASSERT_X(sApp->hasWebDataServices(), Q_FUNC_INFO, "Missing web services");

        if (!model.hasModelString()) { return; }
        if (this->getOwnAircraftModel() != model)
        {
            if (CDatabaseUtils::hasDbAircraftData())
            {
                const CAircraftModel newModel = reverseLookupModel(model);
                const bool updated = this->updateOwnModel(newModel); // update in provider (normally the context)
                if (updated)
                {
                    emit this->ownAircraftModelChanged(this->getOwnAircraftModel());
                }
            }
            else
            {
                // we wait for the data
                connect(sApp->getWebDataServices(), &CWebDataServices::allSwiftDbDataRead, this, [ = ]
                {
                    this->reverseLookupAndUpdateOwnAircraftModel(model);
                });
            }
        }
    }

    CAircraftModel CSimulatorCommon::reverseLookupModel(const CAircraftModel &model)
    {
        bool modified = false;
        const CAircraftModel reverseModel = CDatabaseUtils::consolidateOwnAircraftModelWithDbData(model, false, &modified);
        return reverseModel;
    }

    void CSimulatorCommon::ps_allSwiftDataRead()
    {
        // void
    }

    CAircraftModel CSimulatorCommon::getDefaultModel() const
    {
        return m_defaultModel;
    }

    const CSimulatorPluginInfo &CSimulatorCommon::getSimulatorPluginInfo() const
    {
        return m_simulatorPluginInfo;
    }

    const CSimulatorSetup &CSimulatorCommon::getSimulatorSetup() const
    {
        return m_simulatorSetup;
    }

    void CSimulatorCommon::unload()
    {
        this->disconnectFrom(); // disconnect from simulator
        this->m_remoteAircraftProviderConnections.disconnectAll(); // disconnect signals from provider
    }

    void CSimulatorCommon::setInterpolationAndRenderingSetup(const CInterpolationAndRenderingSetup &setup)
    {
        this->m_interpolator->setInterpolatorSetup(setup);
        if (this->m_interpolationRenderingSetup == setup) { return; }
        this->m_interpolationRenderingSetup = setup;

        const bool r = setup.isRenderingRestricted();
        const bool e = setup.isRenderingEnabled();

        emit renderRestrictionsChanged(r, e, setup.getMaxRenderedAircraft(), setup.getMaxRenderedDistance());
    }

    CInterpolationAndRenderingSetup CSimulatorCommon::getInterpolationAndRenderingSetup() const
    {
        return m_interpolationRenderingSetup;
    }

    void CSimulatorCommon::highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime)
    {
        const CCallsign cs(aircraftToHighlight.getCallsign());
        this->m_highlightedAircraft.removeByCallsign(cs);
        if (enableHighlight)
        {
            const qint64 deltaT = displayTime.valueRounded(CTimeUnit::ms(), 0);
            this->m_highlightEndTimeMsEpoch = QDateTime::currentMSecsSinceEpoch() + deltaT;
            this->m_highlightedAircraft.push_back(aircraftToHighlight);
        }
    }

    int CSimulatorCommon::physicallyRemoveMultipleRemoteAircraft(const CCallsignSet &callsigns)
    {
        int removed = 0;
        for (const CCallsign &callsign : callsigns)
        {
            physicallyRemoveRemoteAircraft(callsign);
            removed++;
        }
        return removed;
    }

    void CSimulatorCommon::ps_oneSecondTimer()
    {
        blinkHighlightedAircraft();
    }

    void CSimulatorCommon::ps_recalculateRenderedAircraft(const CAirspaceAircraftSnapshot &snapshot)
    {
        if (!snapshot.isValidSnapshot()) { return;}

        // for unrestricted values all add/remove actions are directly linked
        // when changing back from restricted->unrestricted an one time update is required
        if (!snapshot.isRestricted() && !snapshot.isRestrictionChanged()) { return; }

        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "Needs to run in object thread");
        Q_ASSERT_X(snapshot.generatingThreadName() != QThread::currentThread()->objectName(), Q_FUNC_INFO, "Expect snapshot from background thread");

        // restricted snapshot values?
        bool changed = false;
        if (snapshot.isRenderingEnabled())
        {
            CCallsignSet callsignsInSimulator(physicallyRenderedAircraft()); // state in simulator
            CCallsignSet callsignsToBeRemoved(callsignsInSimulator.difference(snapshot.getEnabledAircraftCallsignsByDistance()));
            CCallsignSet callsignsToBeAdded(snapshot.getEnabledAircraftCallsignsByDistance().difference(callsignsInSimulator));
            if (!callsignsToBeRemoved.isEmpty())
            {
                const int r = this->physicallyRemoveMultipleRemoteAircraft(callsignsToBeRemoved);
                changed = r > 0;
            }

            if (!callsignsToBeAdded.isEmpty())
            {
                CSimulatedAircraftList aircraftToBeAdded(getAircraftInRange().findByCallsigns(callsignsToBeAdded)); // thread safe copy
                for (const CSimulatedAircraft &aircraft : aircraftToBeAdded)
                {
                    Q_ASSERT_X(aircraft.isEnabled(), Q_FUNC_INFO, "Disabled aircraft detected as to be added");
                    Q_ASSERT_X(aircraft.hasModelString(), Q_FUNC_INFO, "Missing model string");
                    this->physicallyAddRemoteAircraft(aircraft);
                    changed = true;
                }
            }
        }
        else
        {
            // no rendering at all, we remove everything
            const int r = this->physicallyRemoveAllRemoteAircraft();
            changed = r > 0;
        }

        // we have handled snapshot
        if (changed)
        {
            emit airspaceSnapshotHandled();
        }
    }

    void CSimulatorCommon::ps_remoteProviderAddAircraftSituation(const CAircraftSituation &situation)
    {
        Q_UNUSED(situation);
    }

    void CSimulatorCommon::ps_remoteProviderAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const CAircraftParts &parts)
    {
        Q_UNUSED(callsign);
        Q_UNUSED(parts);
    }

    void CSimulatorCommon::ps_remoteProviderRemovedAircraft(const CCallsign &callsign)
    {
        Q_UNUSED(callsign);
    }

    void CSimulatorCommon::reset()
    {
        m_statsUpdateAircraftCountMs = 0;
        m_statsUpdateAircraftTimeAvgMs = 0;
        m_statsUpdateAircraftTimeTotalMs = 0;
        this->clearAllAircraft();
    }

    void CSimulatorCommon::clearAllAircraft()
    {
        m_aircraftToAddAgainWhenRemoved.clear();
    }

} // namespace
