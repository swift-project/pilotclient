/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SIMULATOR_H
#define BLACKCORE_SIMULATOR_H

#include "blackcoreexport.h"
#include "blackcore/interpolator.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorsetup.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/network/client.h"
#include "blackmisc/pixmap.h"
#include <QObject>

namespace BlackCore
{

    //! Interface to a simulator.
    class BLACKCORE_EXPORT ISimulator : public QObject
    {
        Q_OBJECT

    public:
        //! ISimulator status
        enum SimulatorStatus
        {
            Disconnected = 0,
            Connected   = 1 << 0, //!< Is the plugin connected to the simulator?
            Running     = 1 << 1, //!< Is the simulator actually simulating?
            Paused      = 1 << 2, //!< Is the simulator paused?
        };

        //! Render all aircraft
        const int MaxAircraftInfinite = 100;

        //! Destructor
        virtual ~ISimulator() {}

        //! Are we connected to the simulator?
        virtual bool isConnected() const = 0;

        //! Can we connect?
        virtual bool canConnect() const = 0;

        //! Is time synchronization on?
        virtual bool isTimeSynchronized() const = 0;

        //! Simulator paused?
        virtual bool isPaused() const = 0;

        //! Simulator running?
        virtual bool isSimulating() const = 0;

        //! Get the simulator info (metadata of plugin)
        virtual const BlackMisc::Simulation::CSimulatorPluginInfo &getSimulatorPluginInfo() const = 0;

        //! Get the setup (simulator environemnt)
        virtual const BlackMisc::Simulation::CSimulatorSetup &getSimulatorSetup() const = 0;

        //! Connect to simulator
        virtual bool connectTo() = 0;

        //! Connect asynchronously to simulator
        virtual void asyncConnectTo() = 0;

        //! Disconnect from simulator
        virtual bool disconnectFrom() = 0;

        //! Logically add a new aircraft. Depending on max. aircraft, enabled status etc.
        //! it will physically added to the simulator.
        //! \sa physicallyAddRemoteAircraft
        virtual bool logicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) = 0;

        //! Logically remove remote aircraft from simulator. Depending on max. aircraft, enabled status etc.
        //! it will physically added to the simulator.
        virtual bool logicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Change remote aircraft per property
        virtual bool changeRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator) = 0;

        //! Aircraft got enabled / disabled
        virtual bool changeRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const QString &originator) = 0;

        //! Update own aircraft cockpit (usually from context)
        virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator) = 0;

        //! ICAO data for model string
        virtual BlackMisc::Aviation::CAircraftIcaoData getIcaoForModelString(const QString &modelString) const = 0;

        //! Display a status message in the simulator
        virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const = 0;

        //! Display a text message
        virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const = 0;

        //! Aircraft models for available remote aircrafts
        virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const = 0;

        //! Count of aircraft models for available remote aircrafts
        virtual int getInstalledModelsCount() const = 0;

        //! Reload the installed models from disk
        virtual void reloadInstalledModels() = 0;

        //! Airports in range from simulator
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const = 0;

        //! Set time synchronization between simulator and user's computer time
        //! \remarks not all drivers implement this, e.g. if it is an intrinsic simulator feature
        virtual bool setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) = 0;

        //! Time synchronization offset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const = 0;

        //! Representing icon for model string
        virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const = 0;

        //! Max. rendered aircraft
        virtual int getMaxRenderedAircraft() const = 0;

        //! Max. rendered aircraft
        virtual void setMaxRenderedAircraft(int maxRenderedAircraft) = 0;

        //! Max. distance for rendered aircraft
        virtual void setMaxRenderedDistance(BlackMisc::PhysicalQuantities::CLength &distance) = 0;

        //! Max. distance for rendered aircraft
        virtual BlackMisc::PhysicalQuantities::CLength getMaxRenderedDistance() const = 0;

        //! Technical range until aircraft are visible
        virtual BlackMisc::PhysicalQuantities::CLength getRenderedDistanceBoundary() const = 0;

        //! Restricted number of aircraft
        virtual bool isMaxAircraftRestricted() const = 0;

        //! Restriced distance
        virtual bool isMaxDistanceRestricted() const = 0;

        //! Is there a restriction? No rendering -> limited number of aircraft -> unlimited number of aircraft
        virtual bool isRenderingRestricted() const = 0;

        //! Is rendering enabled?
        virtual bool isRenderingEnabled() const = 0;

        //! Delete all restrictions (if any) -> unlimited number of aircraft
        virtual void deleteAllRenderingRestrictions() = 0;

        //! Enable debugging messages
        virtual void enableDebugMessages(bool driver, bool interpolator) = 0;

        //! Is the aircraft rendered (displayed in simulator)?
        //! This shall only return true if the aircraft is really visible in the simulator
        virtual bool isPhysicallyRenderedAircraft(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Physically rendered (displayed in simulator)?
        //! This shall only return aircraft really visible in the simulator
        virtual BlackMisc::Aviation::CCallsignSet physicallyRenderedAircraft() const = 0;

        //! Highlight the aircraft for given time (or disable highlight)
        virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) = 0;

        //! Originator
        const QString &simulatorOriginator();

    signals:
        //! Simulator combined status
        void simulatorStatusChanged(int status);

        //! Emitted when own aircraft model has changed
        void ownAircraftModelChanged(BlackMisc::Simulation::CSimulatedAircraft aircraft);

        //! Render restrictions have been changed
        void renderRestrictionsChanged(bool restricted, int maxAircraft, const BlackMisc::PhysicalQuantities::CLength &maxRenderedDistance, const BlackMisc::PhysicalQuantities::CLength &maxRenderedBoundary);

        //! A single model has been matched
        void modelMatchingCompleted(BlackMisc::Simulation::CSimulatedAircraft aircraft);

        //! Installed aircraft models ready or changed
        void installedAircraftModelsChanged();

        //! An airspace snapshot was handled
        void airspaceSnapshotHandled();

    protected:
        //! Default constructor
        ISimulator(QObject *parent = nullptr) : QObject(parent) {}

        //! Add new remote aircraft physically to the simulator
        //! \sa changeRemoteAircraftEnabled to hide a remote aircraft
        virtual bool physicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) = 0;

        //! Remove remote aircraft from simulator
        virtual bool physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Remove remote aircraft from simulator
        virtual bool physicallyRemoveMultipleRemoteAircraft(const BlackMisc::Aviation::CCallsignSet &callsigns) = 0;

        //! Remove all remote aircraft
        virtual void physicallyRemoveAllRemoteAircraft() = 0;

        //! Emit the combined status
        //! \sa simulatorStatusChanged;
        void emitSimulatorCombinedStatus();
    };

    //! Interface to a simulator listener.
    //! The simulator listener is responsible for letting the core know when
    //! the corresponding simulator is started.
    class BLACKCORE_EXPORT ISimulatorListener : public QObject
    {
        Q_OBJECT

    public:

        //! Constructor
        //! \sa ISimulatorFactory::createListener().
        //! \note msvc2015: use inherited constructor
        ISimulatorListener(QObject *parent);

        //! Destructor
        virtual ~ISimulatorListener() = default;

    public slots:
        //! Start listening for the simulator to start.
        virtual void start() = 0;

        //! Stops listening.
        virtual void stop() = 0;

    signals:

        //! Emitted when the listener discovers the simulator running.
        void simulatorStarted();

    };

    //! Factory pattern class to create instances of ISimulator
    class BLACKCORE_EXPORT ISimulatorFactory
    {
    public:

        //! ISimulatorVirtual destructor
        virtual ~ISimulatorFactory() {}

        //! Create a new instance of a driver
        //! \param info                      metadata about simulator
        //! \param ownAircraftProvider       in memory access to own aircraft data
        //! \param renderedAircraftProvider  in memory access to rendered aircraft data such as situation history and aircraft itself
        //! \param parent QObject
        //! \return driver instance
        virtual ISimulator *create(
            const BlackMisc::Simulation::CSimulatorPluginInfo &info,
            BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
            BlackMisc::Simulation::IRemoteAircraftProvider *renderedAircraftProvider,
            QObject *parent = nullptr) = 0;

        //! Simulator listener instance
        virtual ISimulatorListener *createListener(QObject *parent = nullptr) = 0;

    };
} // namespace

Q_DECLARE_INTERFACE(BlackCore::ISimulatorFactory, "org.swift-project.blackcore.simulatorinterface")

#endif // guard
