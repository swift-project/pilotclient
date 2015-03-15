/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTSIMULATOR_H
#define BLACKCORE_CONTEXTSIMULATOR_H

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME "org.swift.pilotclient.BlackCore.ContextSimulator"

//! DBus object path for context
#define BLACKCORE_CONTEXTSIMULATOR_OBJECTPATH "/Simulator"

//! @}

#include "context.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_runtime.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blacksim/simulatorinfo.h"
#include "blacksim/simulatorinfolist.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/project.h"
#include "blackmisc/pixmap.h"
#include <QObject>

namespace BlackCore
{
    //! Network context
    class IContextSimulator : public CContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)

    public:
        //! Service name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME);
            return s;
        }

        //! Service path
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTSIMULATOR_OBJECTPATH);
            return s;
        }

        //! \copydoc CContext::getPathAndContextId()
        virtual QString getPathAndContextId() const { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextSimulator *create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn);

        //! Destructor
        virtual ~IContextSimulator() {}

    signals:
        //! Emitted when the simulator connection changes
        void connectionChanged(bool connected);

        //! Simulator started or stopped
        void startedChanged(bool startedChanged);

        //! Simulator combined status
        void simulatorStatusChanged(bool connected, bool running, bool paused);

        //! Installed aircraft models ready or changed
        void installedAircraftModelsChanged();

        //! A single model has been matched for given aircraft
        void modelMatchingCompleted(BlackMisc::Simulation::CSimulatedAircraft aircraft);

        //! Emitted when own aircraft model changes
        void ownAircraftModelChanged(BlackMisc::Simulation::CSimulatedAircraft aircraft);

    public slots:

        //! Return list of available simulator plugins
        virtual BlackSim::CSimulatorInfoList getAvailableSimulatorPlugins() const = 0;

        //! Returns true when simulator is connected
        //! \sa isSimulating
        virtual bool isConnected() const = 0;

        //! Can we connect?
        virtual bool canConnect() const = 0;

        //! Connect to simulator
        virtual bool connectToSimulator() = 0;

        //! Connect to simulator (asynchronous version)
        virtual void asyncConnectToSimulator() = 0;

        //! Disconnect from simulator
        virtual bool disconnectFromSimulator() = 0;

        //! Returns true when simulator is running / simulating
        virtual bool isSimulating() const = 0;

        //! Simulator info
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const = 0;

        //! Airports in range
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const = 0;

        //! Installed models in simulator eco system
        virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const = 0;

        //! Model for model string
        virtual BlackMisc::Simulation::CAircraftModelList getInstalledModelsStartingWith(const QString modelString) const = 0;

        //! ICAO data for model string
        virtual BlackMisc::Aviation::CAircraftIcao getIcaoForModelString(const QString &modelString) const = 0;

        //! Set time synchronization between simulator and user's computer time
        //! \remarks not all drivers implement this, e.g. if it is an intrinsic simulator feature
        virtual bool setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) = 0;

        //! Is time synchronization on?
        virtual bool isTimeSynchronized() const = 0;

        //! Max. number of remote aircraft rendered
        virtual int getMaxRenderedAircraft() const = 0;

        //! Max. number of remote aircraft rendered and provide optional selection which aircraft this are
        virtual void setMaxRenderedAircraft(int number, const BlackMisc::Aviation::CCallsignList &renderedAircraft) = 0;

        //! Time synchronization offset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const = 0;

        //! Load specific simulator plugin
        virtual bool loadSimulatorPlugin(const BlackSim::CSimulatorInfo &simulatorInfo) = 0;

        //! Load specific simulator plugin as set in settings
        virtual bool loadSimulatorPluginFromSettings() = 0;

        //! Unload simulator plugin
        virtual void unloadSimulatorPlugin() = 0;

        //! Simulator avialable (driver available)?
        bool isSimulatorAvailable() const { return BlackMisc::CProject::isCompiledWithFlightSimulatorSupport() && !getSimulatorInfo().isUnspecified(); }

        //! Simulator paused?
        virtual bool isPaused() const = 0;

        //! Settings have been changed
        virtual void settingsChanged(uint type) = 0;

        //! Icon representing the model
        virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const = 0;

        //! Enable debugging
        virtual void enableDebugMessages(bool driver, bool interpolator) = 0;

    protected:
        //! Constructor
        IContextSimulator(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContext(mode, runtime) {}
    };

} // namespace

#endif // guard
