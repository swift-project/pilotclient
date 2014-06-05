/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_SIMULATOR_H
#define BLACKCORE_SIMULATOR_H

#include "blacksim/simulatorinfo.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/avaircraft.h"
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

    public:

        //! \brief Simulator connection
        enum Status
        {
            Disconnected,
            Connected,
            ConnectionFailed
        };

        //! \brief Constructor
        ISimulator(QObject *parent = nullptr) : QObject(parent) {}

        //! \brief Destructor
        virtual ~ISimulator() {}

        //! \brief Are we connected to the simulator?
        virtual bool isConnected() const = 0;

        //! \brief Can we connect?
        virtual bool canConnect() = 0;

    public slots:

        //! \brief Connect to simulator
        virtual bool connectTo() = 0;

        //! Connect asynchron to simulator
        virtual void asyncConnectTo() = 0;

        //! \brief Disconnect from simulator
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

    signals:
        //! Emitted when the connection status has changed
        void statusChanged(ISimulator::Status status);

        //! Simulator started
        void simulatorStarted();

        //! Simulator stopped;
        void simulatorStopped();
    };

    //! Factory pattern class to create instances of ISimulator
    class ISimulatorFactory
    {
    public:

        //! \brief Virtual destructor
        virtual ~ISimulatorFactory() {}

        /*!
         * \brief Create a new instance
         * \param parent
         * \return
         */
        virtual ISimulator *create(QObject *parent = nullptr) = 0;

        //! Simulator info
        virtual BlackSim::CSimulatorInfo getSimulatorInfo() const = 0;
    };

} // namespace BlackCore

// TODO: Use CProject to store this string
Q_DECLARE_INTERFACE(BlackCore::ISimulatorFactory, "net.vatsim.PilotClient.BlackCore.SimulatorInterface")

#endif // guard
