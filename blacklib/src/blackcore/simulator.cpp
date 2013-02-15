/***************************************************************************
 *   Copyright (C) 2013 by Roland Winklmeier                               *
 *   roland.m.winklmeier@googlemail.com                                    *
 *                                                                         *
 *   For license information see LICENSE in the root folder of the         *
 *   source code.                                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGlobal>
#include <QLibrary>

#include <blackmisc/debug.h>
#include "blackcore/simulator.h"

namespace BlackCore {

	const quint32         ISimulator::InterfaceVersionMajor = 0;
	const quint32         ISimulator::InterfaceVersionMinor = 1;

	typedef ISimulator* (*createISimulatorInstance)();
	const char *IDRV_CREATE_SIM_INTERFACE = "BB_createISimulatorInstance";

	typedef quint32 (*getDriverVersionMajor)(void);
	const char *DRIVER_VERSION_MAJOR = "BB_InterfaceVersionMajor";

	typedef quint32 (*getDriverVersionMinor)(void);
	const char *DRIVER_VERSION_MINOR = "BB_InterfaceVersionMinor";

	ISimulator *ISimulator::createDriver(ESimulator sim)
	{
		QLibrary myLib;
		ISimulator *result = NULL;

		getDriverVersionMajor driverVersionMajor;
		getDriverVersionMinor driverVersionMinor;
		createISimulatorInstance createDriver;

		switch (sim)
		{
	#ifdef Q_OS_WIN
		case FS9:
		{
			myLib.setFileName(SHARED_LIBRARY_NAME_FS9);
			bAssertstr(myLib.load(), myLib.errorString());
			driverVersionMajor = (getDriverVersionMajor) myLib.resolve(DRIVER_VERSION_MAJOR);

			if (driverVersionMajor != NULL)
			{
				bAssertstr(driverVersionMajor() == ISimulator::InterfaceVersionMajor, QString("Wrong version of the driver. Try to reinstall!"));
			}

			createDriver = (createISimulatorInstance) myLib.resolve(IDRV_CREATE_SIM_INTERFACE);
			if (createDriver)
			{
				bInfo << "Loaded successfully shared library 'bb_driver_fs9'";
				result = createDriver();
			}
		}
		break;

		case FSX:
			myLib.setFileName(SHARED_LIBRARY_NAME_FSX);
			bAssertstr(myLib.load(), myLib.errorString());

			driverVersionMajor = (getDriverVersionMajor) myLib.resolve(DRIVER_VERSION_MAJOR);

			if (driverVersionMajor != NULL)
			{
				bAssertstr(driverVersionMajor() == ISimulator::InterfaceVersionMajor, QString("Wrong version of the driver. Try to reinstall!"));
			}

			createDriver = (createISimulatorInstance) myLib.resolve(IDRV_CREATE_SIM_INTERFACE);
			if (createDriver)
			{
				bInfo << "Loaded successfully shared library 'bb_driver_fsx'";
				result = createDriver();
			}
		break;
	#endif
		case XPLANE:
			myLib.setFileName(SHARED_LIBRARY_NAME_XPLANE);
			bAssertstr(myLib.load(), myLib.errorString());

			driverVersionMajor = (getDriverVersionMajor) myLib.resolve(DRIVER_VERSION_MAJOR);

			if (driverVersionMajor != NULL)
			{
				bAssertstr(driverVersionMajor() == ISimulator::InterfaceVersionMajor, QString("Wrong version of the driver. Try to reinstall!"));
			}

			createDriver = (createISimulatorInstance) myLib.resolve(IDRV_CREATE_SIM_INTERFACE);
			if (createDriver)
			{
				bInfo << "Loaded successfully shared library 'bb_driver_xplane'";
				result = createDriver();
			}
			break;
		default:
			break;
		}

		return result;
	}

	////////////////////////////////
	// Global section
	////////////////////////////////

	// Callback when the Simulation starts
	void ISimulator::setcbSimStarted(cbSimStarted func, void *context)
	{
		mSimStarted = func;
	}

	////////////////////////////////
	// User plane section
	////////////////////////////////
	// Callback frequency tuner
	void ISimulator::setChangedRadioFreq(cbChangedRadioFreq func, void * context)
	{
		mChangedRadioFreq = func;
		m_CallbackContext = context;
	}

	// Callback when the gear is moving
	void ISimulator::setcbChangedGearPosition(cbChangedGearPosition func, void *context)
	{
		mChangedGearPosition = func;
		m_CallbackContext = context;
	}

	// Callback if the user switched on/off a light
	void ISimulator::setcbChangedLights(cbChangedLights func, void *context)
	{
		mChangedLights = func;
		m_CallbackContext = context;
	}

	// Callback, when the Aircraft is set or gets changed
	void ISimulator::setcbChangedAircraftType(cbChangedAircraftType func, void *context)
	{
		mChangedAircraftType = func;
		m_CallbackContext = context;
	}

	// Callback, when the Flaps are moving
	void ISimulator::setcbChangedFlaps(cbChangedFlaps func, void *context)
	{
		mChangedFlaps = func;
		m_CallbackContext = context;
	}

} //! namespace BlackCore {



