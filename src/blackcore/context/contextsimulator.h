// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXTSIMULATOR_H
#define BLACKCORE_CONTEXTSIMULATOR_H

#include "blackcore/context/context.h"
#include "blackcore/corefacade.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/simulator.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmatchersetup.h"
#include "blackmisc/simulation/matchingstatistics.h"
#include "blackmisc/simulation/matchinglog.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/simulation/simulatorinternals.h"
#include "blackmisc/simulation/matchingstatistics.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/identifier.h"
#include "blackmisc/pixmap.h"
#include "blackconfig/buildconfig.h"

#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot

//! \ingroup dbus
//! DBus interface for context
#define BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME "org.swift_project.blackcore.contextsimulator"

//! \ingroup dbus
//! DBus object path for context
#define BLACKCORE_CONTEXTSIMULATOR_OBJECTPATH "/simulator"

class QDBusConnection;

namespace BlackMisc
{
    class CDBusServer;
    namespace Simulation
    {
        class CSimulatedAircraft;
    }
}
namespace BlackCore::Context
{
    //! Network context
    class BLACKCORE_EXPORT IContextSimulator : public IContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)

    public:
        //! Service name
        static const QString &InterfaceName();

        //! Service path
        static const QString &ObjectPath();

        //! \copydoc IContext::getPathAndContextId()
        virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextSimulator *create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection);

        //! Destructor
        virtual ~IContextSimulator() override {}

        //! Get simulator status as enum
        //! \fixme To be removed with Qt 5.5 when getSimualtorStatus directly provides the enum
        BlackCore::ISimulator::SimulatorStatus getSimulatorStatusEnum() const;

        // ---- some convenienc functions implemented on interface level

        //! Current simulator
        BlackMisc::Simulation::CSimulatorInfo getSimulatorInfo() const;

    signals:
        //! Simulator combined status
        //! \sa ISimulator::SimulatorStatus
        //! \remark still int for DBus
        void simulatorStatusChanged(int status);

        //! Simulator plugin loaded / unloaded (default info)
        void simulatorPluginChanged(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

        //! Same as simulatorPluginChanged, only with simulator signature
        void simulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! Simulator settings have been changed
        void simulatorSettingsChanged();

        //! A formerly vital driver is no longer vital/responding
        void vitalityLost();

        //! Frame rate has fallen too far below the threshold to maintain consistent sim rate
        void insufficientFrameRateDetected(bool fatal);

        //! Render restrictions have been changed
        void renderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft, const BlackMisc::PhysicalQuantities::CLength &maxRenderedDistance);

        //! Setup changed
        void interpolationAndRenderingSetupChanged();

        //! Matching setup changed
        void matchingSetupChanged();

        //! Model set ready or changed
        void modelSetChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! A single model has been matched for given aircraft
        void modelMatchingCompleted(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Adding a remote aircraft failed
        void addingRemoteModelFailed(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, bool disabled, bool failover, const BlackMisc::CStatusMessage &message);

        //! Aircraft rendering changed
        void aircraftRenderingChanged(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Emitted when own aircraft model changes
        void ownAircraftModelChanged(const BlackMisc::Simulation::CAircraftModel &model);

        //! An airspace snapshot was handled
        void airspaceSnapshotHandled();

        //! Relevant simulator messages to be explicitly displayed
        void driverMessages(const BlackMisc::CStatusMessageList &messages);

        //! Validated model set
        void validatedModelSet(const BlackMisc::Simulation::CSimulatorInfo &simulator,
                               const BlackMisc::Simulation::CAircraftModelList &valid, const BlackMisc::Simulation::CAircraftModelList &invalid,
                               bool stopped, const BlackMisc::CStatusMessageList &msgs);

        //! Auto publish data written for simulator
        void autoPublishDataWritten(const BlackMisc::Simulation::CSimulatorInfo &simulator);

    public slots:
        //! Simulator info, currently loaded plugin
        virtual BlackMisc::Simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const = 0;

        //! Return list of available simulator plugins
        virtual BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const = 0;

        //! Get the current simulator settings
        virtual BlackMisc::Simulation::Settings::CSimulatorSettings getSimulatorSettings() const = 0;

        //! Set settings for give simulator
        virtual bool setSimulatorSettings(const BlackMisc::Simulation::Settings::CSimulatorSettings &settings, const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

        //! Check all listeners enabled if simulator is connected
        virtual int checkListeners() = 0;

        //! Load and start specific simulator plugin
        virtual bool startSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) = 0;

        //! Stop listener or unload the given plugin (if currently loaded)
        virtual void stopSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) = 0;

        //! Simulator combined status
        virtual int getSimulatorStatus() const = 0;

        //! Simulator setup
        virtual BlackMisc::Simulation::CSimulatorInternals getSimulatorInternals() const = 0;

        //! Airports in range
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange(bool recalculateDistance) const = 0;

        //! Installed models in simulator eco system
        //! \note might be slow since list can be big
        virtual BlackMisc::Simulation::CAircraftModelList getModelSet() const = 0;

        //! Get the model set loader simulator directly
        virtual BlackMisc::Simulation::CSimulatorInfo getModelSetLoaderSimulator() const = 0;

        //! Set the model set loader simulator directly
        //! \note for testing purposes and can be used if no simulator is connected
        virtual void setModelSetLoaderSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

        //! Simulators which have an initialized model set
        virtual BlackMisc::Simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const = 0;

        //! Verify prerequisites for simulation like an existing model set
        //! \pre system needs to be initialized to work correctly
        virtual BlackMisc::CStatusMessageList verifyPrerequisites() const = 0;

        //! Number of installed models in simulator eco system
        virtual int getModelSetCount() const = 0;

        //! Models for model string
        virtual BlackMisc::Simulation::CAircraftModelList getModelSetModelsStartingWith(const QString &modelString) const = 0;

        //! Model strings
        virtual QStringList getModelSetStrings() const = 0;

        //! Model set completer string
        virtual QStringList getModelSetCompleterStrings(bool sorted) const = 0;

        //! Remove models from set
        virtual int removeModelsFromSet(const BlackMisc::Simulation::CAircraftModelList &removeModels) = 0;

        //! \copydoc CAircraftMatcher::disableModelsForMatching
        virtual void disableModelsForMatching(const BlackMisc::Simulation::CAircraftModelList &removedModels, bool incremental) = 0;

        //! \copydoc CAircraftMatcher::getDisabledModelsForMatching
        virtual BlackMisc::Simulation::CAircraftModelList getDisabledModelsForMatching() const = 0;

        //! \copydoc BlackMisc::Simulation::CBackgroundValidation::isValidating
        virtual bool isValidationInProgress() const = 0;

        //! Trigger model set validation
        virtual bool triggerModelSetValidation(const BlackMisc::Simulation::CSimulatorInfo &simulator) = 0;

        //! \copydoc CAircraftMatcher::restoreDisabledModels
        virtual void restoreDisabledModels() = 0;

        //! Known model?
        virtual bool isKnownModelInSet(const QString &modelstring) const = 0;

        //! Set time synchronization between simulator and user's computer time
        //! \remarks not all drivers implement this, e.g. if it is an intrinsic simulator feature
        virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) = 0;

        //! Is time synchronization on?
        virtual bool isTimeSynchronized() const = 0;

        //! \copydoc BlackMisc::Simulation::IInterpolationSetupProvider::getInterpolationSetupGlobal
        virtual BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal getInterpolationAndRenderingSetupGlobal() const = 0;

        //! \copydoc BlackMisc::Simulation::IInterpolationSetupProvider::getInterpolationSetupsPerCallsign
        virtual BlackMisc::Simulation::CInterpolationSetupList getInterpolationAndRenderingSetupsPerCallsign() const = 0;

        //! \copydoc BlackMisc::Simulation::IInterpolationSetupProvider::getInterpolationSetupPerCallsignOrDefault
        virtual BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign getInterpolationAndRenderingSetupPerCallsignOrDefault(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! \copydoc BlackMisc::Simulation::IInterpolationSetupProvider::setInterpolationSetupGlobal
        virtual void setInterpolationAndRenderingSetupGlobal(const BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal &setup) = 0;

        //! \copydoc BlackMisc::Simulation::IInterpolationSetupProvider::setInterpolationSetupsPerCallsign
        virtual bool setInterpolationAndRenderingSetupsPerCallsign(const BlackMisc::Simulation::CInterpolationSetupList &setups, bool ignoreSameAsGlobal) = 0;

        //! Interpolation messages
        virtual BlackMisc::CStatusMessageList getInterpolationMessages(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Time synchronization offset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const = 0;

        //! Simulator avialable (driver available)?
        bool isSimulatorAvailable() const;

        //! Is available simulator simulating? Returns false if no simulator is available
        bool isSimulatorSimulating() const;

        //! Icon representing the model
        virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const = 0;

        //! Get mapping messages
        virtual BlackMisc::CStatusMessageList getMatchingMessages(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Enabled mapping logging?
        virtual BlackMisc::Simulation::MatchingLog isMatchingMessagesEnabled() const = 0;

        //! Enable mapping logging
        virtual void enableMatchingMessages(BlackMisc::Simulation::MatchingLog enabled) = 0;

        //! Follow aircraft im simulator view
        virtual bool followAircraft(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Recalculate all aircraft
        virtual void recalculateAllAircraft() = 0;

        //! Reset model by matching it again
        virtual bool resetToModelMatchingAircraft(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! \copydoc BlackMisc::Simulation::ISimulationEnvironmentProvider::requestElevationBySituation
        virtual bool requestElevationBySituation(const BlackMisc::Aviation::CAircraftSituation &situation) = 0;

        //! \copydoc BlackMisc::Simulation::ISimulationEnvironmentProvider::findClosestElevationWithinRange
        virtual BlackMisc::Geo::CElevationPlane findClosestElevationWithinRange(const BlackMisc::Geo::CCoordinateGeodetic &reference, const BlackMisc::PhysicalQuantities::CLength &range) const = 0;

        //! Repeat all matchings
        virtual int doMatchingsAgain() = 0;

        //! Repeat the matching callsign
        virtual bool doMatchingAgain(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Current matching statistics
        virtual BlackMisc::Simulation::CMatchingStatistics getCurrentMatchingStatistics(bool missingOnly) const = 0;

        //! Set matching setup
        virtual void setMatchingSetup(const BlackMisc::Simulation::CAircraftMatcherSetup &setup) = 0;

        //! Test a remote aircraft
        virtual bool testRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, bool add) = 0;

        //! Test update remote aircraft
        virtual bool testUpdateRemoteAircraft(const BlackMisc::Aviation::CCallsign &cs, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CAircraftParts &parts) = 0;

        //! Get matching setup
        virtual BlackMisc::Simulation::CAircraftMatcherSetup getMatchingSetup() const = 0;

    protected:
        //! Constructor
        IContextSimulator(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContext(mode, runtime) {}
    };
} // namespace

#endif // guard
