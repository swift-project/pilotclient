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

#include "simconnectdatadefinition.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/interpolatormulti.h"
#include <QSharedPointer>
#include <QStringList>
#include <QByteArray>

namespace BlackSimPlugin
{
    namespace FsxCommon
    {
        //! Class representing a SimConnect object
        class CSimConnectObject
        {
        public:
            //! Type
            enum SimObjectType
            {
                Aircraft,
                TerrainProbe
            };

            //! Constructor
            CSimConnectObject();

            //! Constructor
            CSimConnectObject(SimObjectType type);

            //! Constructor providing initial situation/parts
            CSimConnectObject(const BlackMisc::Simulation::CSimulatedAircraft &aircraft,
                              DWORD requestId,
                              BlackMisc::Simulation::ISimulationEnvironmentProvider *simEnvProvider, BlackMisc::Simulation::IInterpolationSetupProvider *setupProvider,
                              BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                              BlackMisc::Simulation::CInterpolationLogger *logger);

            //! Destructor
            ~CSimConnectObject() {}

            //! Get callsign
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return m_aircraft.getCallsign(); }

            //! Callsign?
            bool hasCallsign() const { return !this->getCallsign().isEmpty(); }

            //! Simulated aircraft (as added)
            const BlackMisc::Simulation::CSimulatedAircraft &getAircraft() const { return m_aircraft; }

            //! Simulated aircraft model string
            const QString &getAircraftModelString() const { return m_aircraft.getModelString(); }

            //! Object type
            SimObjectType getType() const { return m_type; }

            //! Aircraft?
            bool isAircraft() const { return this->getType() == Aircraft; }

            //! Probe?
            bool isTerrainProbe() const { return this->getType() == TerrainProbe; }

            //! Set the type
            void setType(SimObjectType type) { m_type = type; }

            //! Set the aircraft
            void setAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Set model string
            void setAircraftModelString(const QString &modelString);

            //! Set CG
            void setAircraftCG(const BlackMisc::PhysicalQuantities::CLength &cg);

            //! Get current lights (requested from simulator)
            const BlackMisc::Aviation::CAircraftLights &getCurrentLightsInSimulator() const { return m_currentLightsInSim; }

            //! Received lights in simulator
            bool hasCurrentLightsInSimulator() const { return !m_currentLightsInSim.isNull(); }

            //! Set current lights when received from simulator
            void setCurrentLightsInSimulator(const BlackMisc::Aviation::CAircraftLights &lights) { m_currentLightsInSim = lights; }

            //! Pretend to have received lights from simulator
            void fakeCurrentLightsInSimulator() { m_currentLightsInSim.setNull(false); }

            //! Lights as sent to simulator
            const BlackMisc::Aviation::CAircraftLights &getLightsAsSent() const { return m_lightsAsSent; }

            //! Lights as sent to simulator
            void setLightsAsSent(const BlackMisc::Aviation::CAircraftLights &lights) { m_lightsAsSent = lights; }

            //! How often do we request data from simulator for this remote aircraft
            SIMCONNECT_PERIOD getSimDataPeriod() const { return m_requestSimDataPeriod; }

            //! How often do we request data from simulator for this remote aircraft
            void setSimDataPeriod(SIMCONNECT_PERIOD period) { m_requestSimDataPeriod = period; }

            //! Set Simconnect request id
            void setRequestId(DWORD id);

            //! Get SimConnect request id
            DWORD getRequestId() const { return m_requestId; }

            //! Get SimConnect with offset
            DWORD getRequestId(CSimConnectDefinitions::SimObjectRequest offset) const;

            //! Set Simconnect object id
            void setObjectId(DWORD id);

            //! Get SimConnect object id
            DWORD getObjectId() const { return m_objectId; }

            //! Get SimConnect object id
            QString getObjectIdAsString() const { return QString::number(this->getObjectId()); }

            //! Valid request id?
            bool hasValidRequestId() const { return m_validRequestId; }

            //! Valid object id?
            bool hasValidObjectId() const { return m_validObjectId; }

            //! Object is requested in simulator, not yet confirmed added
            bool isPendingAdded() const;

            //! Still pending
            bool isOutdatedPendingAdded(qint64 thresholdMs = 2000, qint64 currentMsSinceEpoch = -1) const;

            //! Adding is confirmed
            bool isConfirmedAdded() const;

            //! Marked as confirmed, means the simulator has "confirmed" the objectId as added and not instantly removed the object
            void setConfirmedAdded(bool confirm);

            //! Special states
            //! @{
            bool isAddedWhileRemoving() { return m_addedWhileRemoving; }
            void setAddedWhileRemoving(bool addedWileRemoved);
            bool isRemovedWhileAdding() const { return m_removedWhileAdding; }
            void setRemovedWhileAdding(bool removedWhileAdding);
            //! @}

            //! Removing is pending
            bool isPendingRemoved() const { return m_pendingRemoved; }

            //! Object which can be used for sending, not pending and valid ids
            bool isReadyToSend() const;

            //! Marked as pending for removal
            void setPendingRemoved(bool pending);

            //! Pending added or removed?
            bool isPending() const { return this->isPendingAdded() || this->isPendingRemoved(); }

            //! Has camera?
            bool hasCamera() const { return m_camera; }

            //! Reset camera positions
            void resetCameraPositions();

            //! Camera position
            const SIMCONNECT_DATA_XYZ &cameraPosition() const { return m_cameraPosition; }

            //! Camera rotation;
            const SIMCONNECT_DATA_PBH &cameraRotation() const { return m_cameraRotation; }

            //! Camera GUID
            GUID getCameraGUID() const { return m_cameraGuid; }

            //! Set camera GUID
            void setCameraGUID(GUID guid) { m_cameraGuid = guid; m_camera = true; }

            //! Reset the state (like it was a new onject) without affecting interpolator and aircraft
            void resetState();

            //! Reset the timestamp
            void resetTimestamp() { m_tsCreated = QDateTime::currentMSecsSinceEpoch(); }

            //! VTOL?
            bool isVtol() const { return m_aircraft.isVtol(); }

            //! Valid?
            bool isValid() const { return !this->isInvalid(); }

            //! Invalid?
            bool isInvalid() const { return !this->hasValidObjectId() && !this->hasValidRequestId(); }

            //! Engine count
            int getEngineCount() const { return m_aircraft.getEnginesCount(); }

            //! Was the object really added to simulator
            bool hasValidRequestAndObjectId() const;

            //! Callsign as LATIN1
            const QByteArray &getCallsignByteArray() const { return m_callsignByteArray; }

            //! \copydoc BlackMisc::Simulation::CInterpolator::getInterpolatorInfo
            QString getInterpolatorInfo(BlackMisc::Simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

            //! \copydoc BlackMisc::Simulation::CInterpolator::attachLogger
            void attachInterpolatorLogger(BlackMisc::Simulation::CInterpolationLogger *logger) const;

            //! \copydoc BlackMisc::Simulation::CInterpolator::getInterpolation
            BlackMisc::Simulation::CInterpolationResult getInterpolation(qint64 currentTimeSinceEpoc, const BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber) const;

            //! \copydoc BlackMisc::Simulation::CInterpolator::getLastInterpolatedSituation
            const BlackMisc::Aviation::CAircraftSituation &getLastInterpolatedSituation(BlackMisc::Simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

            //! Interpolator
            BlackMisc::Simulation::CInterpolatorMulti *getInterpolator() const { return m_interpolator.data(); }

            //! SimObject as string
            QString toQString() const;

            //! Type of id
            static SimObjectType requestIdToType(DWORD requestId);

            //! Type to string
            static const QString &typeToString(SimObjectType type);

        private:
            BlackMisc::Simulation::CSimulatedAircraft m_aircraft; //!< corresponding aircraft
            SimObjectType m_type = Aircraft;
            DWORD m_requestId = 0;
            DWORD m_objectId  = 0;
            bool m_validRequestId = false;
            bool m_validObjectId  = false;
            bool m_confirmedAdded = false;
            bool m_pendingRemoved = false;
            bool m_camera         = false;
            bool m_removedWhileAdding = false;
            bool m_addedWhileRemoving = false;
            int  m_lightsRequestedAt  = -1;
            qint64 m_tsCreated    = -1;
            GUID m_cameraGuid;
            SIMCONNECT_DATA_XYZ m_cameraPosition;
            SIMCONNECT_DATA_PBH m_cameraRotation;
            QByteArray m_callsignByteArray;
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
            bool setSimConnectObjectIdForRequestId(DWORD requestId, DWORD objectId);

            //! Find which callsign belongs to the object id
            BlackMisc::Aviation::CCallsign getCallsignForObjectId(DWORD objectId) const;

            //! Get object per object id
            CSimConnectObject getSimObjectForObjectId(DWORD objectId) const;

            //! Get object per request id
            CSimConnectObject getSimObjectForRequestId(DWORD requestId) const;

            //! Get by request or object id, just as possible
            CSimConnectObject getSimObjectForOtherSimObject(const CSimConnectObject &otherSimObj) const;

            //! Mark as added if existing
            CSimConnectObject markObjectAsAdded(DWORD objectId);

            //! Is the object id one of our AI objects?
            bool isKnownSimObjectId(DWORD objectId) const;

            //! Remove by id
            bool removeByObjectId(DWORD objectId);

            //! Remove by object id or request id
            bool removeByOtherSimObject(const CSimConnectObject &otherSimObj);

            //! Remove all the probes
            int removeAllProbes();

            //! Pending add condition
            bool containsPendingAdded() const;

            //! Pending removed condition
            bool containsPendingRemoved() const;

            //! Number of pending added
            int countPendingAdded() const;

            //! Number of pending removed
            int countPendingRemoved() const;

            //! Objects not pending
            int countConfirmedAdded();

            //! Get all callsigns
            BlackMisc::Aviation::CCallsignSet getAllCallsigns(bool withoutProbes = true) const;

            //! Get all callsign strings
            QStringList getAllCallsignStrings(bool sorted = false) const;

            //! Get all callsign strings as string
            QString getAllCallsignStringsAsString(bool sorted = false, const QString &separator = ", ") const;

            //! Callsigns of pending added callsigns
            BlackMisc::Aviation::CCallsignSet getPendingAddedCallsigns() const;

            //! Callsigns of pending removed callsigns
            BlackMisc::Aviation::CCallsignSet getPendingRemovedCallsigns() const;

            //! Get by type
            QList<CSimConnectObject> getByType(CSimConnectObject::SimObjectType type) const;

            //! All probes
            QList<CSimConnectObject> getProbes() const { return this->getByType(CSimConnectObject::TerrainProbe); }

            //! Get a non pending probe
            CSimConnectObject getNotPendingProbe() const;

            //! Contains object of type
            bool containsType(CSimConnectObject::SimObjectType type) const;
        };
    } // namespace
} // namespace

#endif // guard
