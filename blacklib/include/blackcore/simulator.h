//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QString>

#include <blackcore/sim_callbacks.h>

#include "blackcore/vector_geo.h"
class CLibraryContext;

#define SHARED_LIBRARY_NAME_FS9 "bb_driver_fs9"
#define SHARED_LIBRARY_NAME_FSX "bb_driver_fsx"
#define SHARED_LIBRARY_NAME_XPLANE "bb_driver_xplane"

namespace BlackCore {

	class CVector3D;
	class CVectorGEO;
	class IContext;

	class ISimulator
	{
	public:

		/// Version of the driver interface. To increment when the interface change.
		static const quint32         InterfaceVersionMajor;
		static const quint32         InterfaceVersionMinor;


		enum ESimulator {   FS9 = 0,
							FSX,
							XPLANE};

		ISimulator(void) {}
		virtual ~ISimulator() {}

		virtual void setLibraryContext(IContext *context) = 0;

		////////////////////////////////
		// Global section
		////////////////////////////////

		virtual int init() = 0;

		virtual int connectTo() = 0;

		// Callback when the Simulation starts
		virtual void setcbSimStarted(cbSimStarted func, void *context);

		virtual bool isRunning() = 0;

		virtual QString getLastErrorMessage() = 0;

		virtual qint32 sendTextMessage(const QString &text) = 0;

		////////////////////////////////
		// Remote plane section
		////////////////////////////////

		virtual qint32 addPlane(const QString& callsign, const QString &type, const CVectorGEO * const pos, const double groundSpeed) = 0;

		virtual bool removePlane(const qint32 id) = 0;

		virtual bool updatePositionAndSpeed(const qint32 id, const CVectorGEO * const pos, const double groundSpeed) = 0;

		virtual bool setGear(const qint32 id, const qint32 percentage) = 0;

		virtual bool setFlaps(const qint32 id, const qint32 percentage) = 0;

		virtual bool setLights(const qint32 id, const qint32 map) = 0;

		////////////////////////////////
		// User plane section
		////////////////////////////////

		// Callback frequency tuner
		virtual void setChangedRadioFreq(cbChangedRadioFreq func, void * context);

		// Callback when the gear is moving
		virtual void setcbChangedGearPosition(cbChangedGearPosition func, void *context);

		// Callback if the user switched on/off a light
		virtual void setcbChangedLights(cbChangedLights func, void *context);

		// Callback, when the Aircraft is set or gets changed
		virtual void setcbChangedAircraftType(cbChangedAircraftType func, void *context);

		// Callback, when the Flaps are moving
		virtual void setcbChangedFlaps(cbChangedFlaps func, void *context);

		// This one is called regular and when we actually need it, therefor not a callback
		virtual double getUserSpeed() const = 0;

		// This one is called regular and when we actually need it, therefor not a callback
		virtual CVectorGEO getUserPosition() const = 0;
	
		// This one is called regular and when we actually need it, therefor not a callback
		virtual qint32 getUserPitch() const = 0;
	
		// This one is called regular and when we actually need it, therefor not a callback
		virtual qint32 getUserBank() const = 0;
	
		// This one is called regular and when we actually need it, therefor not a callback
		virtual qint32 getUserHeading() const = 0;
	
		// This one is called regular and when we actually need it, therefor not a callback
		virtual qint32 getUserPitchBankHeading() const = 0;

		virtual bool isOnGround() const = 0;

		static ISimulator *createDriver(ESimulator sim);
	
	protected:

		CLibraryContext * mLibraryContext;
	
		cbSimStarted mSimStarted;
		cbChangedRadioFreq mChangedRadioFreq;
		cbChangedGearPosition mChangedGearPosition;
		cbChangedLights mChangedLights;
		cbChangedAircraftType mChangedAircraftType;
		cbChangedFlaps mChangedFlaps;
	
		void *m_CallbackContext;
	
	};

} //! namespace BlackCore

#endif // SIMULATOR_H
