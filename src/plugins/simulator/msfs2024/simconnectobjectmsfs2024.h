// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_MSFS2024COMMON_SIMCONNECTOBJECT_H
#define SWIFT_SIMPLUGIN_MSFS2024COMMON_SIMCONNECTOBJECT_H

#include <QByteArray>
#include <QSharedPointer>
#include <QStringList>

#include "misc/simulation/interpolation/interpolatormulti.h"
#include "misc/simulation/simulatedaircraft.h"
#include "plugins/simulator/msfs2024/msfs2024export.h"
#include "plugins/simulator/msfs2024/simconnectdatadefinitionmsfs2024.h"
#include "plugins/simulator/msfs2024/simconnectsymbolsmsfs2024.h"

namespace swift::simplugin::msfs2024common
{
    //! Class representing a SimConnect object
    class MSFS2024_EXPORT CSimConnectObject
    {
    public:
        //! Type
        enum SimObjectType
        {
            AircraftNonAtc,
            AircraftSimulatedObject,
            AllTypes
        };

        //! Constructor
        CSimConnectObject();

        //! Constructor
        CSimConnectObject(SimObjectType type);

        //! Constructor providing initial situation/parts
        CSimConnectObject(const swift::misc::simulation::CSimulatedAircraft &aircraft, DWORD requestId,
                          swift::misc::simulation::ISimulationEnvironmentProvider *simEnvProvider,
                          swift::misc::simulation::IInterpolationSetupProvider *setupProvider,
                          swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                          swift::misc::simulation::CInterpolationLogger *logger);

        //! Get callsign
        const swift::misc::aviation::CCallsign &getCallsign() const { return m_aircraft.getCallsign(); }

        //! Get callsign
        const QString &getCallsignAsString() const { return m_aircraft.getCallsign().asString(); }

        //! Callsign?
        bool hasCallsign() const { return !this->getCallsign().isEmpty(); }

        //! Simulated aircraft (as added)
        const swift::misc::simulation::CSimulatedAircraft &getAircraft() const { return m_aircraft; }

        //! Simulated aircraft model
        const swift::misc::simulation::CAircraftModel &getAircraftModel() const { return m_aircraft.getModel(); }

        //! Simulated aircraft model string
        const QString &getAircraftModelString() const { return m_aircraft.getModelString(); }

        //! Object type
        SimObjectType getType() const { return m_type; }

        //! Type as string
        const QString &getTypeAsString() const { return typeToString(m_type); }

        //! Aircraft?
        bool isAircraft() const
        {
            return this->getType() == AircraftNonAtc || this->getType() == AircraftSimulatedObject;
        }

        //! Aircraft simulated object?
        bool isAircraftSimulatedObject() const { return this->getType() == AircraftSimulatedObject; }

        //! Aircraft NON ATC?
        bool isAircraftNonAtc() const { return this->getType() == AircraftNonAtc; }

        //! Set the type
        void setType(SimObjectType type) { m_type = type; }

        //! Set the aircraft
        void setAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Set model string
        void setAircraftModelString(const QString &modelString);

        //! Set CG
        void setAircraftCG(const swift::misc::physical_quantities::CLength &cg);

        //! Get current lights (requested from simulator)
        const swift::misc::aviation::CAircraftLights &getCurrentLightsInSimulator() const
        {
            return m_currentLightsInSim;
        }

        //! Received lights in simulator
        bool hasCurrentLightsInSimulator() const { return !m_currentLightsInSim.isNull(); }

        //! Set current lights when received from simulator
        void setCurrentLightsInSimulator(const swift::misc::aviation::CAircraftLights &lights)
        {
            m_currentLightsInSim = lights;
        }

        //! Pretend to have received lights from simulator
        void fakeCurrentLightsInSimulator() { m_currentLightsInSim.setNull(false); }

        //! Lights as sent to simulator
        const swift::misc::aviation::CAircraftLights &getLightsAsSent() const { return m_lightsAsSent; }

        //! Lights as sent to simulator
        void setLightsAsSent(const swift::misc::aviation::CAircraftLights &lights) { m_lightsAsSent = lights; }

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
        bool isOutdatedPendingAdded(qint64 thresholdMs = 5000, qint64 currentMsSinceEpoch = -1) const;

        //! Adding is confirmed
        bool isConfirmedAdded() const;

        //! Marked as confirmed, means the simulator has "confirmed" the objectId as added and not instantly removed the
        //! object
        void setConfirmedAdded(bool confirm);

        //! @{
        //! Special states
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

        //! Camera position/rotation
        void setCameraPositionAndRotation(const SIMCONNECT_DATA_XYZ &position, const SIMCONNECT_DATA_PBH &rotation)
        {
            m_cameraPosition = position;
            m_cameraRotation = rotation;
        }

        //! Camera GUID
        GUID getCameraGUID() const { return m_cameraGuid; }

        //! Set camera GUID
        void setCameraGUID(GUID guid)
        {
            m_cameraGuid = guid;
            m_camera = true;
        }

        //! No camera anymore
        void removeCamera() { m_camera = false; }

        //! Set observer
        void setObserverName(const QString &observer) { m_observerName = observer; }

        //! Observer name
        const QString &getObserverName() const { return m_observerName; }

        //! Reset the state (like it was a new onject) without affecting interpolator and aircraft
        void resetState();

        //! Reset so it can be added again
        void resetToAddAgain();

        //! Reset the timestamp
        void resetTimestampToNow() { m_tsCreated = QDateTime::currentMSecsSinceEpoch(); }

        //! VTOL?
        bool isVtol() const { return m_aircraft.isVtol(); }

        //! Valid?
        bool isValid() const { return !this->isInvalid(); }

        //! Invalid?
        bool isInvalid() const { return !this->hasValidObjectId() && !this->hasValidRequestId(); }

        //! Created timestamp?
        bool hasCreatedTimestamp() const { return m_tsCreated >= 0; }

        //! Created timestamp
        qint64 getCreatedTimestamp() const { return m_tsCreated; }

        //! Engine count
        int getEngineCount() const { return m_aircraft.getEnginesCount(); }

        //! Was the object really added to simulator
        bool hasValidRequestAndObjectId() const;

        //! Adding has been failed before
        int getAddingExceptions() const { return m_addingExceptions; }

        //! Set adding failed before
        void setAddingExceptions(int number) { m_addingExceptions = number; }

        //! Increase adding exception
        void increaseAddingExceptions() { m_addingExceptions++; }

        //! Decrease adding exception
        void decreaseAddingExceptions()
        {
            if (m_addingExceptions > 0) { m_addingExceptions--; }
        }

        //! Adding and directly removed
        int getAddingDirectlyRemoved() const { return m_addingDirectlyRemoved; }

        //! Set adding and directly removed
        void setAddingDirectlyRemoved(int number) { m_addingDirectlyRemoved = number; }

        //! Increase adding and directly removed
        void increaseAddingDirectlyRemoved() { m_addingDirectlyRemoved++; }

        //! Copy the counters from another object
        void copyAddingFailureCounters(const CSimConnectObject &otherObject);

        //! Callsign as LATIN1
        const QByteArray &getCallsignByteArray() const { return m_callsignByteArray; }

        //! \copydoc swift::misc::simulation::CInterpolator::getInterpolatorInfo
        QString
        getInterpolatorInfo(swift::misc::simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! \copydoc swift::misc::simulation::CInterpolator::attachLogger
        void attachInterpolatorLogger(swift::misc::simulation::CInterpolationLogger *logger) const;

        //! \copydoc swift::misc::simulation::CInterpolator::getInterpolation
        swift::misc::simulation::CInterpolationResult
        getInterpolation(qint64 currentTimeSinceEpoch,
                         const swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign &setup,
                         uint32_t aircraftNumber) const;

        //! \copydoc swift::misc::simulation::CInterpolator::getLastInterpolatedSituation
        const swift::misc::aviation::CAircraftSituation &getLastInterpolatedSituation(
            swift::misc::simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! \copydoc swift::misc::simulation::CInterpolator::getInterpolationMessages
        const swift::misc::CStatusMessageList &getInterpolationMessages(
            swift::misc::simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const;

        //! Interpolator
        swift::misc::simulation::CInterpolatorMulti *getInterpolator() const { return m_interpolator.data(); }

        //! SimObject as string
        QString toQString() const;

        //! Type of id
        static SimObjectType requestIdToType(DWORD requestId);

        //! Type to string
        static const QString &typeToString(SimObjectType type);

        //! Same type
        static bool isSameTypeGroup(SimObjectType t1, SimObjectType t2);

        //! Aircraft?
        static bool isAircraft(SimObjectType type);

    private:
        swift::misc::simulation::CSimulatedAircraft m_aircraft; //!< corresponding aircraft
        SimObjectType m_type = AircraftNonAtc;
        DWORD m_requestId = 0;
        DWORD m_objectId = 0;
        bool m_validRequestId = false;
        bool m_validObjectId = false;
        bool m_confirmedAdded = false;
        bool m_pendingRemoved = false;
        bool m_camera = false;
        bool m_removedWhileAdding = false;
        bool m_addedWhileRemoving = false;
        int m_addingExceptions = 0; //!< exception when added
        int m_addingDirectlyRemoved = 0; //!< added, but removed directly afterwards
        qint64 m_tsCreated = -1;
        GUID m_cameraGuid;
        SIMCONNECT_DATA_XYZ m_cameraPosition;
        SIMCONNECT_DATA_PBH m_cameraRotation;
        QByteArray m_callsignByteArray;
        QString m_observerName;
        swift::misc::aviation::CAircraftLights m_currentLightsInSim {
            nullptr
        }; //!< current lights to know state for toggling
        swift::misc::aviation::CAircraftLights m_lightsAsSent { nullptr }; //!< lights as sent to simulator
        SIMCONNECT_PERIOD m_requestSimDataPeriod = SIMCONNECT_PERIOD_NEVER; //!< how often do we query ground elevation
        QSharedPointer<swift::misc::simulation::CInterpolatorMulti>
            m_interpolator; //!< shared pointer because CSimConnectObject can be copied
    };

    //! Simulator objects (aka AI aircraft)
    class CSimConnectObjects : public QHash<swift::misc::aviation::CCallsign, CSimConnectObject>
    {
    public:
        //! Insert
        bool insert(const CSimConnectObject &simObject, bool updateTimestamp = false);

        //! Set ID of a SimConnect object, so far we only have an request id in the object
        bool setSimConnectObjectIdForRequestId(DWORD requestId, DWORD objectId);

        //! Find which callsign belongs to the object id
        swift::misc::aviation::CCallsign getCallsignForObjectId(DWORD objectId) const;

        //! Get object per object id
        CSimConnectObject getSimObjectForObjectId(DWORD objectId) const;

        //! Get object per request id
        CSimConnectObject getSimObjectForRequestId(DWORD requestId) const;

        //! Get by request or object id, just as possible
        CSimConnectObject getSimObjectForOtherSimObject(const CSimConnectObject &otherSimObj) const;

        //! Get the oldest object
        CSimConnectObject getOldestObject() const;

        //! Is the object id one of our AI objects?
        bool isKnownSimObjectId(DWORD objectId) const;

        //! Remove by id
        bool removeByObjectId(DWORD objectId);

        //! Remove by object id or request id
        bool removeByOtherSimObject(const CSimConnectObject &otherSimObj);

        //! Remove all the probes
        int removeAllProbes();

        //! Remove callsigns
        int removeCallsigns(const swift::misc::aviation::CCallsignSet &callsigns);

        //! Remove all pending added objects
        CSimConnectObjects removeOutdatedPendingAdded(CSimConnectObject::SimObjectType type);

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
        swift::misc::aviation::CCallsignSet getAllCallsigns(bool withoutProbes = true) const;

        //! Get all callsign strings
        QStringList getAllCallsignStrings(bool sorted = false, bool withoutProbes = true) const;

        //! Get all callsign strings as string
        QString getAllCallsignStringsAsString(bool sorted = false, const QString &separator = ", ") const;

        //! Callsigns of pending added callsigns
        swift::misc::aviation::CCallsignSet getPendingAddedCallsigns() const;

        //! Callsigns of pending removed callsigns
        swift::misc::aviation::CCallsignSet getPendingRemovedCallsigns() const;

        //! Get by type
        QList<CSimConnectObject> getByType(CSimConnectObject::SimObjectType type) const;

        //! All aircraft
        QList<CSimConnectObject> getAircraft() const;

        //! Get a non pending probe
        CSimConnectObject getNotPendingProbe() const;

        //! Get a non pending probe
        CSimConnectObject getOldestNotPendingProbe() const;

        //! Contains object of type
        bool containsType(CSimConnectObject::SimObjectType type) const;

        //! Aircraft?
        bool containsAircraft() const;
    };
} // namespace swift::simplugin::msfs2024common

#endif // SWIFT_SIMPLUGIN_MSFS2024COMMON_SIMCONNECTOBJECT_H
