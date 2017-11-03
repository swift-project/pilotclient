/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_SIMCONNECTOBJECT_H
#define BLACKSIMPLUGIN_FSXCOMMON_SIMCONNECTOBJECT_H

#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/interpolatormulti.h"
#include "simconnectdatadefinition.h"
#include <QSharedPointer>

namespace BlackSimPlugin
{
    namespace FsxCommon
    {
        //! Class representing a SimConnect object
        class CSimConnectObject
        {
        public:
            //! Constructor
            CSimConnectObject();

            //! Constructor, providing initial situation/parts
            CSimConnectObject(const BlackMisc::Simulation::CSimulatedAircraft &aircraft,
                              DWORD requestId,
                              BlackMisc::Simulation::CInterpolationLogger *logger);

            //! Destructor
            ~CSimConnectObject() {}

            //! Get callsign
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return m_aircraft.getCallsign(); }

            //! Simulated aircraft (as added)
            const BlackMisc::Simulation::CSimulatedAircraft &getAircraft() const { return m_aircraft; }

            //! Simulated aircraft model string
            const QString &getAircraftModelString() const { return m_aircraft.getModelString(); }

            //! Interpolator
            BlackMisc::Simulation::CInterpolatorMulti *getInterpolator() const { return m_interpolator.data(); }

            //! Add parts for interpolator
            void addAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

            //! Add situation for interpolator
            void addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Get current lights (requested from simulator)
            const BlackMisc::Aviation::CAircraftLights &getCurrentLightsInSimulator() const { return m_currentLightsInSim; }

            //! Received lights in simulator
            bool hasCurrentLightsInSimulator() const { return !m_currentLightsInSim.isNull(); }

            //! Set current lights when received from simulator
            void setCurrentLightsInSimulator(const BlackMisc::Aviation::CAircraftLights &lights) { m_currentLightsInSim = lights; }

            //! Parts as sent to simulator
            const DataDefinitionRemoteAircraftPartsWithoutLights &getPartsAsSent() const { return m_partsAsSent; }

            //! Parts as sent to simulator
            void setPartsAsSent(const DataDefinitionRemoteAircraftPartsWithoutLights &parts) { m_partsAsSent = parts; }

            //! Invalidate parts as sent
            void invalidatePartsAsSent();

            //! Lights as sent to simulator
            const BlackMisc::Aviation::CAircraftLights &getLightsAsSent() const { return m_lightsAsSent; }

            //! Lights as sent to simulator
            void setLightsAsSent(const BlackMisc::Aviation::CAircraftLights &lights) { m_lightsAsSent = lights; }

            //! How often do we request data from simulator for this remote aircraft
            SIMCONNECT_PERIOD getSimDataPeriod() const { return m_requestSimDataPeriod; }

            //! How often do we request data from simulator for this remote aircraft
            void setSimDataPeriod(SIMCONNECT_PERIOD period) { m_requestSimDataPeriod = period; }

            //! Set Simconnect request id
            void setRequestId(DWORD id) { m_requestId = id; m_validRequestId = true; }

            //! Get Simconnect request id
            DWORD getRequestId() const { return m_requestId; }

            //! Set Simconnect object id
            void setObjectId(DWORD id) { m_objectId = id; m_validObjectId = true; }

            //! Set Simconnect object id
            DWORD getObjectId() const { return m_objectId; }

            //! Valid request id?
            bool hasValidRequestId() const { return m_validRequestId; }

            //! Valid object id?
            bool hasValidObjectId() const { return m_validObjectId; }

            //! Object is requested, not yet added
            bool isPendingAdded() const;

            //! Adding is confirmed
            bool isConfirmedAdded() const;

            //! Marked as confirmed
            void setConfirmedAdded(bool confirm);

            //! Removing is pending
            bool isPendingRemoved() const { return m_pendingRemoved; }

            //! Marked as confirmed
            void setPendingRemoved(bool pending);

            //! VTOL?
            bool isVtol() const { return m_aircraft.isVtol(); }

            //! Was the object really added to SIM
            bool hasValidRequestAndObjectId() const;

            //! Toggle interpolator mode
            void toggleInterpolatorMode();

            //! Set interpolator mode
            bool setInterpolatorMode(BlackMisc::Simulation::CInterpolatorMulti::Mode mode);

            //! Interpolator info
            QString getInterpolatorInfo() const;

        private:
            BlackMisc::Simulation::CSimulatedAircraft m_aircraft; //!< corresponding aircraft
            DWORD m_requestId = 0;
            DWORD m_objectId  = 0;
            bool m_validRequestId = false;
            bool m_validObjectId  = false;
            bool m_confirmedAdded = false;
            bool m_pendingRemoved = false;
            int m_lightsRequestedAt = -1;
            DataDefinitionRemoteAircraftPartsWithoutLights m_partsAsSent {};          //!< parts as sent
            BlackMisc::Aviation::CAircraftLights m_currentLightsInSim { nullptr };    //!< current lights to know state for toggling
            BlackMisc::Aviation::CAircraftLights m_lightsAsSent { nullptr };          //!< lights as sent to simulator
            SIMCONNECT_PERIOD m_requestSimDataPeriod = SIMCONNECT_PERIOD_NEVER;       //!< how often do we query ground elevation
            QSharedPointer<BlackMisc::Simulation::CInterpolatorMulti> m_interpolator; //!< shared pointer because CSimConnectObject can be copied
        };

        //! Simulator objects (aka AI aircraft)
        class CSimConnectObjects : public QHash<BlackMisc::Aviation::CCallsign, CSimConnectObject>
        {
        public:
            //! Set ID of a SimConnect object, so far we only have an request id in the object
            bool setSimConnectObjectIdForRequestId(DWORD requestId, DWORD objectId, bool resetSentParts = false);

            //! Find which callsign belongs to the object id
            BlackMisc::Aviation::CCallsign getCallsignForObjectId(DWORD objectId) const;

            //! Get object per object id
            CSimConnectObject getSimObjectForObjectId(DWORD objectId) const;

            //! Get object per request id
            CSimConnectObject getSimObjectForRequestId(DWORD requestId) const;

            //! Is the object id one of our AI objects?
            bool isKnownSimObjectId(DWORD objectId) const;

            //! Pending add condition
            bool containsPendingAdded() const;

            //! Pending removed condition
            bool containsPendingRemoved() const;

            //! Number of pending added
            int countPendingAdded() const;

            //! Number of pending removed
            int countPendingRemoved() const;

            //! Callsigns of pending added callsigns
            BlackMisc::Aviation::CCallsignSet getPendingAddedCallsigns() const;

            //! Callsigns of pending removed callsigns
            BlackMisc::Aviation::CCallsignSet getPendingRemovedCallsigns() const;

            //! Toggle interpolator modes
            void toggleInterpolatorModes();

            //! Toggle interpolator modes
            void toggleInterpolatorMode(const BlackMisc::Aviation::CCallsign &callsign);

            //! Set interpolator modes
            int setInterpolatorModes(BlackMisc::Simulation::CInterpolatorMulti::Mode mode);
        };
    } // namespace
} // namespace

#endif // guard
