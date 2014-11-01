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

#include "blacksim/simulatorinfo.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/nwaircraftmodellist.h"
#include "blackmisc/nwtextmessage.h"
#include <QObject>

namespace BlackCore
{
    /*!
     * Interface to a simulator.
     */
    class ISimulator : public QObject
    {
        Q_OBJECT
        Q_ENUMS(Status)

    public:
        //! ISimulator connection
        enum Status
        {
            Disconnected,
            Connected,
            ConnectionFailed
        };

        //! Constructor
        ISimulator(QObject *parent = nullptr);

        //! Destructor
        virtual ~ISimulator() {}

        //! Are we connected to the simulator?
        virtual bool isConnected() const = 0;

        //! Can we connect?
        virtual bool canConnect() = 0;

    public slots:

        //! Connect to simulator
        virtual bool connectTo() = 0;

        //! Connect asynchronously to simulator
        virtual void asyncConnectTo() = 0;

        //! Disconnect from simulator
        virtual bool disconnectFrom() = 0;

        //! Return user aircraft object
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const = 0;

        //! Add new remote aircraft to the simulator
        //! \todo Add parameter: aircraft model
        virtual void addRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &initialSituation) = 0;

        //! Add new aircraft situation
        virtual void addAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation) = 0;

        //! Remove remote aircraft from simulator
        virtual void removeRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Update own aircraft cockpit (usually from context)
        virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft) = 0;

        //! Simulator info
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const = 0;

        //! Display a status message in the simulator
        virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const = 0;

        //! Display a text message
        virtual void displayTextMessage(const BlackMisc::Network::CTextMessage &message) const = 0;

        //! Own aircraft Model
        virtual BlackMisc::Network::CAircraftModel getAircraftModel() const = 0;

        //! Aircraft models for available remote aircraft
        virtual BlackMisc::Network::CAircraftModelList getInstalledModels() const = 0;

        //! Airports in range
        virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const = 0;

        //! Set time synchronization between simulator and user's computer time
        //! \remarks not all drivers implement this, e.g. if it is an intrinsic simulator feature
        virtual void setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset) = 0;

        //! Is time synchronization on?
        virtual bool isTimeSynchronized() const = 0;

        //! Time synchronization offset
        virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const = 0;

        //! Simulator paused?
        virtual bool isSimPaused() const = 0;

    signals:
        //! Emitted when the connection status has changed
        void statusChanged(ISimulator::Status status);

        //! Emitted when own aircraft model has changed
        void aircraftModelChanged(BlackMisc::Network::CAircraftModel model);

        //! Simulator started
        void simulatorStarted();

        //! Simulator stopped;
        void simulatorStopped();
    };

    //! Factory pattern class to create instances of ISimulator
    class ISimulatorFactory
    {
    public:

        //! ISimulatorVirtual destructor
        virtual ~ISimulatorFactory() {}

        //! Create a new instance
        virtual ISimulator *create(QObject *parent = nullptr) = 0;

        //! Simulator info
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const = 0;
    };

} // namespace BlackCore

// TODO: Use CProject to store this string
Q_DECLARE_INTERFACE(BlackCore::ISimulatorFactory, "net.vatsim.PilotClient.BlackCore.SimulatorInterface")
Q_DECLARE_METATYPE(BlackCore::ISimulator::Status)

#endif // guard
