/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSIMULATOR_H
#define BLACKCORE_CONTEXTSIMULATOR_H

#define BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextSimulator"
#define BLACKCORE_CONTEXTSIMULATOR_OBJECTPATH "/Simulator"

#include "context.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_runtime.h"
#include "blacksim/simulatorinfo.h"
#include "blacksim/simulatorinfolist.h"
#include "blackmisc/nwaircraftmodel.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/project.h"
#include <QObject>

namespace BlackCore
{
    //! \brief Network context
    class IContextSimulator : public CContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)

    public:
        //! \brief Service name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME);
            return s;
        }

        //! \brief Service path
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
        void connectionChanged(bool value);

        //! Emitted when own aircraft model changes (TODO move to own aircraft context?)
        void ownAircraftModelChanged(BlackMisc::Network::CAircraftModel model);

    public slots:

        //! Return list of available simulator plugins
        virtual BlackSim::CSimulatorInfoList getAvailableSimulatorPlugins() const = 0;

        //! Returns true when simulator is connected and available
        virtual bool isConnected() const = 0;

        //! \brief Can we connect?
        virtual bool canConnect() = 0;

        //! \brief Connect to simulator
        virtual bool connectTo() = 0;

        //! \brief Connect to simulator (asynchronous version)
        virtual void asyncConnectTo() = 0;

        //! \brief Disconnect from simulator
        virtual bool disconnectFrom() = 0;

        //! Simulator info
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const = 0;

        //! Aircraft model
        virtual BlackMisc::Network::CAircraftModel getOwnAircraftModel() const = 0;

        //! Load specific simulator plugin
        virtual bool loadSimulatorPlugin(const BlackSim::CSimulatorInfo &simulatorInfo) = 0;

        //! Unload simulator plugin
        virtual void unloadSimulatorPlugin() = 0;

        //! Simulator avialable?
        bool isSimulatorAvailable() const { return BlackMisc::CProject::isCompiledWithFlightSimulatorSupport() && !getSimulatorInfo().isUnspecified(); }

    protected:
        //! \brief Constructor
        IContextSimulator(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContext(mode, runtime) {}
    };

} // namespace BlackCore

#endif // guard
