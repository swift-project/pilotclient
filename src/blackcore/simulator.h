/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_SIMULATOR_H
#define BLACKCORE_SIMULATOR_H

#include "blacksim/simulatorinfo.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/nwaircraftmodel.h"
#include <QObject>

namespace BlackCore
{
    /*!
     * Interface to a connection to a ATC voice server for use in flight simulation.
     *
     * \warning If an INetwork signal is connected to a slot, and that slot emits a signal
     *          which is connected to an INetwork slot, then at least one of those connections
     *          must be a Qt::QueuedConnection.
     *          Reason: IVoiceClient implementations are not re-entrant.
     */
    class ISimulator : public QObject
    {
        Q_OBJECT
        Q_ENUMS(Status)

    public:

        //! ISimulatorSimulator connection
        enum Status
        {
            Disconnected,
            Connected,
            ConnectionFailed
        };

        //! ISimulatorConstructor
        ISimulator(QObject *parent = nullptr);

        //! ISimulatorDestructor
        virtual ~ISimulator() {}

        //! ISimulatorAre we connected to the simulator?
        virtual bool isConnected() const = 0;

        //! ISimulatorCan we connect?
        virtual bool canConnect() = 0;

    public slots:

        //! ISimulatorConnect to simulator
        virtual bool connectTo() = 0;

        //! Connect asynchron to simulator
        virtual void asyncConnectTo() = 0;

        //! ISimulatorDisconnect from simulator
        virtual bool disconnectFrom() = 0;

        //! Return user aircraft object
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const = 0;

        //! Add new remote aircraft to the simulator
        virtual void addRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign, const QString &type, const BlackMisc::Aviation::CAircraftSituation &initialSituation) = 0;

        //! Add new aircraft situation
        virtual void addAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &initialSituation) = 0;

        //! Remove remote aircraft from simulator
        virtual void removeRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Update own aircraft cockpit (usually from context)
        virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft) = 0;

        //! Simulator info
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const = 0;

        //! Display a status message in the simulator
        virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const = 0;

        //! Aircraft Model
        virtual BlackMisc::Network::CAircraftModel getAircraftModel() const = 0;

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

        //! Send a message to be displayed (from simulator to contexts)
        void sendStatusMessage(const BlackMisc::CStatusMessage &message);

        //! Send messages to be displayed (from simulator to contexts)
        void sendStatusMessages(const BlackMisc::CStatusMessageList &messages);
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
