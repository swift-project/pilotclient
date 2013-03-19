//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QString>
#include <functional>

#include "blackcore/sim_callbacks.h"
#include "blackcore/vector_geo.h"
#include "blackcore/vector_3d.h"

#define SHARED_LIBRARY_NAME_FS9 "bb_driver_fs9"
#define SHARED_LIBRARY_NAME_FSX "bb_driver_fsx"
#define SHARED_LIBRARY_NAME_XPLANE "bb_driver_xplane"

namespace BlackMisc {

    class IContext;

}

namespace BlackCore {

    class CPlaneModel
    {
    public:
        QString name;
        QString typeCode;
        QString airlineCode;
    };

    class CPhysicalState
    {
    public:
        CPhysicalState() : headingDegrees(0), pitchDegrees(0), bankDegrees(0), groundSpeedKnots(0)
        {}
        CVectorGeo position;
        float headingDegrees;
        float pitchDegrees;
        float bankDegrees;
        float groundSpeedKnots;
        CVector3D trueSpeedMetersPerSec;
    };

    class CAvionicsState
    {
        CAvionicsState() : squawkCode(0), squawkModeC(false), squawkIdent(false),
            com1FreqHz(0), com2FreqHz(0)
        {}
        qint16 squawkCode;
        bool squawkModeC;
        bool squawkIdent;
        qint32 com1FreqHz;
        qint32 com2FreqHz;
    };

    class CAnimationState
    {
    public:
        CAnimationState() : gearPercent(0), flapsPercent(0), landingLights(false),
            taxiLights(false), navLights(false), strobeLights(false), beaconLights(false)
        {}
        qint8 gearPercent;
        qint8 flapsPercent;
        bool landingLights;
        bool taxiLights;
        bool navLights;
        bool strobeLights;
        bool beaconLights;
    };

    typedef std::tr1::function<void(const bool status)> cbSimStarted;
    typedef std::tr1::function<void(const CAvionicsState &state)> cbChangedAvionicsState;
    typedef std::tr1::function<void(const CAnimationState &state)> cbChangedAnimationState;
    typedef std::tr1::function<void(const CPlaneModel &model)> cbChangedModel;
    typedef std::tr1::function<void(const QString &message)> cbSendTextMessage;

    class ISimulator
	{
	public:

		/// Version of the driver interface. To increment when the interface change.
		static const quint32         InterfaceVersionMajor;
		static const quint32         InterfaceVersionMinor;

		enum ESimulator {
            FS9 = 0,
			FSX,
			XPLANE,
        };

		ISimulator() {}
		virtual ~ISimulator() {}

		virtual void setLibraryContext(BlackMisc::IContext *context);

		static ISimulator *createDriver(ESimulator sim);
	
		////////////////////////////////
		// Global section
		////////////////////////////////

		virtual int init() = 0;

		virtual int connect() = 0;

		// Callback when the Simulation starts
		virtual void setcbSimStarted(const cbSimStarted &func);

		virtual bool isConnected() = 0;

		virtual QString getLastErrorMessage() = 0;

		////////////////////////////////
		// User plane section
		////////////////////////////////

		// Callback avionics state
		virtual void setcbChangedAvionicsState(const cbChangedAvionicsState &func);

		// Callback animation state
		virtual void setcbChangedAnimationState(const cbChangedAnimationState &func);

		// Callback, when the Aircraft is set or gets changed
		virtual void setcbChangedModel(const cbChangedModel &func);

        // Not const because it may need to mutate state in order to communicate with the sim
		virtual bool isOnGround() = 0;

        // This might block - use QtConcurrent::run if that is a problem
        virtual CPhysicalState getPhysicalState() = 0;

		////////////////////////////////
		// Remote plane section
		////////////////////////////////

        // This might block - use QtConcurrent::run if that is a problem
		virtual qint32 addPlane(const QString &callsign) = 0;

		virtual bool removePlane(const qint32 planeID) = 0;

        virtual void setModel(const qint32 planeID, const CPlaneModel &model) = 0;

		virtual bool setPhysicalState(const qint32 planeID, const CPhysicalState &state) = 0;

		virtual bool setAnimationState(const qint32 planeID, const CAnimationState &state) = 0;

        // Calls the supplied visitor function once for every model available in the simulator.
        virtual void visitAllModels(const std::tr1::function<void(const CPlaneModel &)> &visitor) = 0;

        // Fills container with all models. Works for any container that supports push_back.
        template <class T>
        void getAllModels(T &container) { visitAllModels(std::tr1::bind(T::push_back, container)); }

	protected:
        BlackMisc::IContext *m_libraryContext;
	
		cbSimStarted m_cbSimStarted;
		cbChangedAvionicsState m_cbChangedAvionicsState;
		cbChangedAnimationState m_cbChangedAnimationState;
		cbChangedModel m_cbChangedModel;
	};

} //! namespace BlackCore

#endif // SIMULATOR_H
