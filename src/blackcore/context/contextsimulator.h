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
#define BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME "org.swift_project.blackcore.contextsimulator"

//! DBus object path for context
#define BLACKCORE_CONTEXTSIMULATOR_OBJECTPATH "/simulator"

//! @}

#include "blackconfig/buildconfig.h"
#include "blackcore/blackcoreexport.h"
#include "blackcore/context/context.h"
#include "blackcore/corefacade.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/simulator.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/identifier.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/matchingstatistics.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/simulation/simulatorinternals.h"
#include "blackmisc/simulation/matchingstatistics.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/weather/weathergrid.h"

#include <QObject>
#include <QString>

class QDBusConnection;

namespace BlackMisc
{
    class CDBusServer;
    namespace Simulation { class CSimulatedAircraft; }
}
namespace BlackCore
{
    namespace Context
    {
        //! Network context
        class BLACKCORE_EXPORT IContextSimulator : public CContext
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)

        public:
            //! Service name
            static const QString &InterfaceName();

            //! Service path
            static const QString &ObjectPath();

            //! Highlight time
            static const BlackMisc::PhysicalQuantities::CTime &HighlightTime();

            //! \copydoc CContext::getPathAndContextId()
            virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

            //! Factory method
            static IContextSimulator *create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection);

            //! Destructor
            virtual ~IContextSimulator() {}

        signals:
            //! Simulator combined status
            //! \sa ISimulator::SimulatorStatus
            //! \remark still int for DBus
            void simulatorStatusChanged(int status);

            //! Simulator plugin loaded / unloaded (default info)
            void simulatorPluginChanged(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

            //! Render restrictions have been changed
            void renderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft, const BlackMisc::PhysicalQuantities::CLength &maxRenderedDistance);

            //! Model set ready or changed
            void modelSetChanged();

            //! A single model has been matched for given aircraft
            void modelMatchingCompleted(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Adding a remote aircraft failed
            void addingRemoteModelFailed(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CStatusMessage &message);

            //! Aircraft rendering changed
            void aircraftRenderingChanged(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Emitted when own aircraft model changes
            void ownAircraftModelChanged(const BlackMisc::Simulation::CAircraftModel &model);

            //! An airspace snapshot was handled
            void airspaceSnapshotHandled();

            //! A weather grid, requested with requestWeatherGrid(), is received
            void weatherGridReceived(const BlackMisc::Weather::CWeatherGrid &weatherGrid, const BlackMisc::CIdentifier &identifier);

            //! Relevant simulator messages to be explicitly displayed
            void driverMessages(const BlackMisc::CStatusMessageList &messages);

        public slots:
            //! Simulator info, currently loaded plugin
            virtual BlackMisc::Simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const = 0;

            //! Return list of available simulator plugins
            virtual BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const = 0;

            //! Load and start specific simulator plugin
            virtual bool startSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) = 0;

            //! Stop listener or unload the given plugin (if currently loaded)
            virtual void stopSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) = 0;

            //! Simulator combined status
            virtual int getSimulatorStatus() const = 0;

            //! Get simulator status as enum
            //! \fixme To be removed with Qt 5.5 when getSimualtorStatus directly provides the enum
            BlackCore::ISimulator::SimulatorStatus getSimulatorStatusEnum() const;

            //! Simulator setup
            virtual BlackMisc::Simulation::CSimulatorInternals getSimulatorInternals() const = 0;

            //! Airports in range
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const = 0;

            //! Installed models in simulator eco system
            //! \note might be slow since list can be big
            virtual BlackMisc::Simulation::CAircraftModelList getModelSet() const = 0;

            //! Get the model set loader simulator directly
            virtual BlackMisc::Simulation::CSimulatorInfo getModelSetLoaderSimulator() const = 0;

            //! Set the model set loader simulator directly
            //! \note for testing purposes
            virtual void setModelSetLoaderSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

            //! Simulators which have an initialized model set
            virtual BlackMisc::Simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const = 0;

            //! Number of installed models in simulator eco system
            virtual int getModelSetCount() const = 0;

            //! Models for model string
            virtual BlackMisc::Simulation::CAircraftModelList getModelSetModelsStartingWith(const QString &modelString) const = 0;

            //! Model strings
            virtual QStringList getModelSetStrings() const = 0;

            //! Model set completer string
            virtual QStringList getModelSetCompleterStrings(bool sorted) const = 0;

            //! Set time synchronization between simulator and user's computer time
            //! \remarks not all drivers implement this, e.g. if it is an intrinsic simulator feature
            virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) = 0;

            //! Is time synchronization on?
            virtual bool isTimeSynchronized() const = 0;

            //! Set interpolation and rendering
            virtual BlackMisc::Simulation::CInterpolationAndRenderingSetup getInterpolationAndRenderingSetup() const = 0;

            //! Set interpolation and rendering
            virtual void setInterpolationAndRenderingSetup(const BlackMisc::Simulation::CInterpolationAndRenderingSetup &setup) = 0;

            //! Time synchronization offset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const = 0;

            //! Simulator avialable (driver available)?
            bool isSimulatorAvailable() const { return BlackConfig::CBuildConfig::isCompiledWithFlightSimulatorSupport() && !getSimulatorPluginInfo().isUnspecified(); }

            //! Is available simulator simulating? Returns false if no simulator is available
            bool isSimulatorSimulating() const;

            //! Icon representing the model
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const = 0;

            //! Get mapping messages
            virtual BlackMisc::CStatusMessageList getMatchingMessages(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Enabled mapping logging?
            virtual bool isMatchingMessagesEnabled() const = 0;

            //! Enable mapping logging
            virtual void enableMatchingMessages(bool enabled) = 0;

            //! Highlight aircraft in simulator
            virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) = 0;

            //! Reset model by matching it again
            virtual bool resetToModelMatchingAircraft(const BlackMisc::Aviation::CCallsign &callsign) = 0;

            //! Activates or deactivates simulator weather
            virtual void setWeatherActivated(bool activated) = 0;

            //! Request weather grid. Argument identifier is past in the signal to identify the requestor
            virtual void requestWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid, const BlackMisc::CIdentifier &identifier) = 0;

            //! Current matching statistics
            virtual BlackMisc::Simulation::CMatchingStatistics getCurrentMatchingStatistics(bool missingOnly) const = 0;

        protected:
            //! Constructor
            IContextSimulator(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : CContext(mode, runtime) {}
        };
    } // namespace
} // namespace

#endif // guard
