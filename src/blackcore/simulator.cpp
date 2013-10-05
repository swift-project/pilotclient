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

#include "blackcore/simulator.h"
#include "blackmisc/debug.h"
#include "blackmisc/context.h"
#include <QtGlobal>
#include <QLibrary>

namespace BlackCore {

	const quint32         ISimulator::InterfaceVersionMajor = 0;
	const quint32         ISimulator::InterfaceVersionMinor = 1;

	typedef ISimulator* (*createISimulatorInstance)();
	const char *IDRV_CREATE_SIM_INTERFACE = "BB_createISimulatorInstance";

	typedef quint32 (*getDriverVersionMajor)(void);
	const char *DRIVER_VERSION_MAJOR = "BB_InterfaceVersionMajor";

	typedef quint32 (*getDriverVersionMinor)(void);
	const char *DRIVER_VERSION_MINOR = "BB_InterfaceVersionMinor";

    void ISimulator::setLibraryContext(BlackMisc::IContext *context)
    {
        m_libraryContext = context;
    }

	ISimulator *ISimulator::createDriver(ESimulator sim)
	{
		QLibrary myLib;
		ISimulator *result = NULL;

		getDriverVersionMajor driverVersionMajor;
		//getDriverVersionMinor driverVersionMinor;
		createISimulatorInstance createDriver;

		switch (sim)
		{
	#ifdef Q_OS_WIN
		case FS9:
		{
			myLib.setFileName(SHARED_LIBRARY_NAME_FS9);
			if (! myLib.load())
            {
                //TODO throw
            }

			driverVersionMajor = (getDriverVersionMajor) myLib.resolve(DRIVER_VERSION_MAJOR);
			if (driverVersionMajor)
			{
				if (driverVersionMajor() != ISimulator::InterfaceVersionMajor)
                {
                    //TODO throw
                }
			}

			createDriver = (createISimulatorInstance) myLib.resolve(IDRV_CREATE_SIM_INTERFACE);
			if (createDriver)
			{
				bAppInfo << "Loaded successfully shared library 'bb_driver_fs9'";
				result = createDriver();
			}
		}
		break;

		case FSX:
			myLib.setFileName(SHARED_LIBRARY_NAME_FSX);
			if (! myLib.load())
            {
                //TODO throw
            }

			driverVersionMajor = (getDriverVersionMajor) myLib.resolve(DRIVER_VERSION_MAJOR);
			if (driverVersionMajor)
			{
				if (driverVersionMajor() != ISimulator::InterfaceVersionMajor)
                {
                    //TODO throw
                }
			}

			createDriver = (createISimulatorInstance) myLib.resolve(IDRV_CREATE_SIM_INTERFACE);
			if (createDriver)
			{
				bAppInfo << "Loaded successfully shared library 'bb_driver_fsx'";
				result = createDriver();
			}
		break;
	#endif
		case XPLANE:
			myLib.setFileName(SHARED_LIBRARY_NAME_XPLANE);
			if (! myLib.load())
            {
                //TODO throw
            }

			driverVersionMajor = (getDriverVersionMajor) myLib.resolve(DRIVER_VERSION_MAJOR);
			if (driverVersionMajor)
			{
				if (driverVersionMajor() != ISimulator::InterfaceVersionMajor)
                {
                    //TODO throw
                }
			}

			createDriver = (createISimulatorInstance) myLib.resolve(IDRV_CREATE_SIM_INTERFACE);
			if (createDriver)
			{
				bAppInfo << "Loaded successfully shared library 'bb_driver_xplane'";
				result = createDriver();
			}
			break;
		default:
			break;
		}

		return result;
	}

	void ISimulator::setcbSimStarted(const cbSimStarted &func)
	{
		m_cbSimStarted = func;
	}

    void ISimulator::setcbChangedAvionicsState(const cbChangedAvionicsState &func)
	{
		m_cbChangedAvionicsState = func;
	}

	void ISimulator::setcbChangedAnimationState(const cbChangedAnimationState &func)
	{
		m_cbChangedAnimationState = func;
	}

	void ISimulator::setcbChangedModel(const cbChangedModel &func)
	{
		m_cbChangedModel = func;
	}

} //! namespace BlackCore
