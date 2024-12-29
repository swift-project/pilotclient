// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXTSIMULATOR_H
#define SWIFT_CORE_CONTEXTSIMULATOR_H

#include <QObject>
#include <QString>

#include "core/context/context.h"
#include "core/corefacade.h"
#include "core/corefacadeconfig.h"
#include "core/simulator.h"
#include "core/swiftcoreexport.h"
#include "misc/aviation/airportlist.h"
#include "misc/geo/elevationplane.h"
#include "misc/pixmap.h"
#include "misc/pq/length.h"
#include "misc/pq/time.h"
#include "misc/simulation/aircraftmatchersetup.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/interpolation/interpolationrenderingsetup.h"
#include "misc/simulation/matchinglog.h"
#include "misc/simulation/matchingstatistics.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/simulatorinternals.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/simulation/simulatorplugininfolist.h"

// clazy:excludeall=const-signal-or-slot

//! \ingroup dbus
//! DBus interface for context
#define SWIFT_CORE_CONTEXTSIMULATOR_INTERFACENAME "org.swift_project.swift_core.contextsimulator"

//! \ingroup dbus
//! DBus object path for context
#define SWIFT_CORE_CONTEXTSIMULATOR_OBJECTPATH "/simulator"

class QDBusConnection;

namespace swift::misc
{
    class CDBusServer;
    namespace simulation
    {
        class CSimulatedAircraft;
    }
} // namespace swift::misc
namespace swift::core::context
{
    //! Network context
    class SWIFT_CORE_EXPORT IContextSimulator : public IContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", SWIFT_CORE_CONTEXTSIMULATOR_INTERFACENAME)

    public:
        //! Service name
        static const QString &InterfaceName();

        //! Service path
        static const QString &ObjectPath();

        //! \copydoc IContext::getPathAndContextId()
        QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextSimulator *create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode,
                                         swift::misc::CDBusServer *server, QDBusConnection &connection);

        //! Destructor
        ~IContextSimulator() override {}

    signals:
        //! Simulator combined status
        //! \sa ISimulator::SimulatorStatus
        //! \remark still int for DBus
        void simulatorStatusChanged(int status);

        //! Simulator plugin loaded / unloaded (default info)
        void simulatorPluginChanged(const swift::misc::simulation::CSimulatorPluginInfo &info);

        //! Same as simulatorPluginChanged, only with simulator signature
        void simulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Simulator settings have been changed
        void simulatorSettingsChanged();

        //! A formerly vital driver is no longer vital/responding
        void vitalityLost();

        //! Frame rate has fallen too far below the threshold to maintain consistent sim rate
        void insufficientFrameRateDetected(bool fatal);

        //! Render restrictions have been changed
        void renderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft,
                                       const swift::misc::physical_quantities::CLength &maxRenderedDistance);

        //! Setup changed
        void interpolationAndRenderingSetupChanged();

        //! Matching setup changed
        void matchingSetupChanged();

        //! Model set ready or changed
        void modelSetChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! A single model has been matched for given aircraft
        void modelMatchingCompleted(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Adding a remote aircraft failed
        void addingRemoteModelFailed(const swift::misc::simulation::CSimulatedAircraft &aircraft, bool disabled,
                                     bool failover, const swift::misc::CStatusMessage &message);

        //! Aircraft rendering changed
        void aircraftRenderingChanged(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Emitted when own aircraft model changes
        void ownAircraftModelChanged(const swift::misc::simulation::CAircraftModel &model);

        //! An airspace snapshot was handled
        void airspaceSnapshotHandled();

        //! Relevant simulator messages to be explicitly displayed
        void driverMessages(const swift::misc::CStatusMessageList &messages);

        //! Validated model set
        void validatedModelSet(const swift::misc::simulation::CSimulatorInfo &simulator,
                               const swift::misc::simulation::CAircraftModelList &valid,
                               const swift::misc::simulation::CAircraftModelList &invalid, bool stopped,
                               const swift::misc::CStatusMessageList &msgs);

        //! Auto publish data written for simulator
        void autoPublishDataWritten(const swift::misc::simulation::CSimulatorInfo &simulator);

    public slots:
        //! Simulator info, currently loaded plugin
        virtual swift::misc::simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const = 0;

        //! Return list of available simulator plugins
        virtual swift::misc::simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const = 0;

        //! Get the current simulator settings
        virtual swift::misc::simulation::settings::CSimulatorSettings getSimulatorSettings() const = 0;

        //! Set settings for give simulator
        virtual bool setSimulatorSettings(const swift::misc::simulation::settings::CSimulatorSettings &settings,
                                          const swift::misc::simulation::CSimulatorInfo &simulator) = 0;

        //! Check all listeners enabled if simulator is connected
        virtual int checkListeners() = 0;

        //! Load and start specific simulator plugin
        virtual bool startSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) = 0;

        //! Stop listener or unload the given plugin (if currently loaded)
        virtual void stopSimulatorPlugin(const swift::misc::simulation::CSimulatorPluginInfo &simulatorInfo) = 0;

        //! Simulator combined status
        virtual ISimulator::SimulatorStatus getSimulatorStatus() const = 0;

        //! Simulator setup
        virtual swift::misc::simulation::CSimulatorInternals getSimulatorInternals() const = 0;

        //! Airports in range
        virtual swift::misc::aviation::CAirportList getAirportsInRange(bool recalculateDistance) const = 0;

        //! Installed models in simulator eco system
        //! \note might be slow since list can be big
        virtual swift::misc::simulation::CAircraftModelList getModelSet() const = 0;

        //! Get the model set loader simulator directly
        virtual swift::misc::simulation::CSimulatorInfo getModelSetLoaderSimulator() const = 0;

        //! Set the model set loader simulator directly
        //! \note for testing purposes and can be used if no simulator is connected
        virtual void setModelSetLoaderSimulator(const swift::misc::simulation::CSimulatorInfo &simulator) = 0;

        //! Simulators which have an initialized model set
        virtual swift::misc::simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const = 0;

        //! Verify prerequisites for simulation like an existing model set
        //! \pre system needs to be initialized to work correctly
        virtual swift::misc::CStatusMessageList verifyPrerequisites() const = 0;

        //! Number of installed models in simulator eco system
        virtual int getModelSetCount() const = 0;

        //! Models for model string
        virtual swift::misc::simulation::CAircraftModelList
        getModelSetModelsStartingWith(const QString &modelString) const = 0;

        //! Model strings
        virtual QStringList getModelSetStrings() const = 0;

        //! Model set completer string
        virtual QStringList getModelSetCompleterStrings(bool sorted) const = 0;

        //! Remove models from set
        virtual int removeModelsFromSet(const swift::misc::simulation::CAircraftModelList &removeModels) = 0;

        //! \copydoc CAircraftMatcher::disableModelsForMatching
        virtual void disableModelsForMatching(const swift::misc::simulation::CAircraftModelList &removedModels,
                                              bool incremental) = 0;

        //! \copydoc CAircraftMatcher::getDisabledModelsForMatching
        virtual swift::misc::simulation::CAircraftModelList getDisabledModelsForMatching() const = 0;

        //! \copydoc swift::misc::simulation::CBackgroundValidation::isValidating
        virtual bool isValidationInProgress() const = 0;

        //! Trigger model set validation
        virtual bool triggerModelSetValidation(const swift::misc::simulation::CSimulatorInfo &simulator) = 0;

        //! \copydoc CAircraftMatcher::restoreDisabledModels
        virtual void restoreDisabledModels() = 0;

        //! Known model?
        virtual bool isKnownModelInSet(const QString &modelstring) const = 0;

        //! Set time synchronization between simulator and user's computer time
        //! \remarks not all drivers implement this, e.g. if it is an intrinsic simulator feature
        virtual bool setTimeSynchronization(bool enable, const swift::misc::physical_quantities::CTime &offset) = 0;

        //! Is time synchronization on?
        virtual bool isTimeSynchronized() const = 0;

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::getInterpolationSetupGlobal
        virtual swift::misc::simulation::CInterpolationAndRenderingSetupGlobal
        getInterpolationAndRenderingSetupGlobal() const = 0;

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::getInterpolationSetupsPerCallsign
        virtual swift::misc::simulation::CInterpolationSetupList
        getInterpolationAndRenderingSetupsPerCallsign() const = 0;

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::getInterpolationSetupPerCallsignOrDefault
        virtual swift::misc::simulation::CInterpolationAndRenderingSetupPerCallsign
        getInterpolationAndRenderingSetupPerCallsignOrDefault(
            const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::setInterpolationSetupGlobal
        virtual void setInterpolationAndRenderingSetupGlobal(
            const swift::misc::simulation::CInterpolationAndRenderingSetupGlobal &setup) = 0;

        //! \copydoc swift::misc::simulation::IInterpolationSetupProvider::setInterpolationSetupsPerCallsign
        virtual bool
        setInterpolationAndRenderingSetupsPerCallsign(const swift::misc::simulation::CInterpolationSetupList &setups,
                                                      bool ignoreSameAsGlobal) = 0;

        //! Interpolation messages
        virtual swift::misc::CStatusMessageList
        getInterpolationMessages(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Time synchronization offset
        virtual swift::misc::physical_quantities::CTime getTimeSynchronizationOffset() const = 0;

        //! Simulator avialable (driver available)?
        bool isSimulatorAvailable() const;

        //! Is available simulator simulating? Returns false if no simulator is available
        bool isSimulatorSimulating() const;

        //! Get mapping messages
        virtual swift::misc::CStatusMessageList
        getMatchingMessages(const swift::misc::aviation::CCallsign &callsign) const = 0;

        //! Enabled mapping logging?
        virtual swift::misc::simulation::MatchingLog isMatchingMessagesEnabled() const = 0;

        //! Enable mapping logging
        virtual void enableMatchingMessages(swift::misc::simulation::MatchingLog enabled) = 0;

        //! Follow aircraft im simulator view
        virtual bool followAircraft(const swift::misc::aviation::CCallsign &callsign) = 0;

        //! Recalculate all aircraft
        virtual void recalculateAllAircraft() = 0;

        //! Reset model by matching it again
        virtual bool resetToModelMatchingAircraft(const swift::misc::aviation::CCallsign &callsign) = 0;

        //! \copydoc swift::misc::simulation::ISimulationEnvironmentProvider::requestElevationBySituation
        virtual bool requestElevationBySituation(const swift::misc::aviation::CAircraftSituation &situation) = 0;

        //! \copydoc swift::misc::simulation::ISimulationEnvironmentProvider::findClosestElevationWithinRange
        virtual swift::misc::geo::CElevationPlane
        findClosestElevationWithinRange(const swift::misc::geo::CCoordinateGeodetic &reference,
                                        const swift::misc::physical_quantities::CLength &range) const = 0;

        //! Repeat all matchings
        virtual int doMatchingsAgain() = 0;

        //! Repeat the matching callsign
        virtual bool doMatchingAgain(const swift::misc::aviation::CCallsign &callsign) = 0;

        //! Current matching statistics
        virtual swift::misc::simulation::CMatchingStatistics getCurrentMatchingStatistics(bool missingOnly) const = 0;

        //! Set matching setup
        virtual void setMatchingSetup(const swift::misc::simulation::CAircraftMatcherSetup &setup) = 0;

        //! Test a remote aircraft
        virtual bool testRemoteAircraft(const swift::misc::simulation::CSimulatedAircraft &aircraft, bool add) = 0;

        //! Test update remote aircraft
        virtual bool testUpdateRemoteAircraft(const swift::misc::aviation::CCallsign &cs,
                                              const swift::misc::aviation::CAircraftSituation &situation,
                                              const swift::misc::aviation::CAircraftParts &parts) = 0;

        //! Get matching setup
        virtual swift::misc::simulation::CAircraftMatcherSetup getMatchingSetup() const = 0;

    protected:
        //! Constructor
        IContextSimulator(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContext(mode, runtime) {}
    };
} // namespace swift::core::context

#endif // SWIFT_CORE_CONTEXTSIMULATOR_H
