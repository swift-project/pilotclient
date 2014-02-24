/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_SIMULATOR_H
#define BLACKCORE_SIMULATOR_H

#include <blackmisc/avaircraft.h>
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
            Not_Connected,
            Connected
        };

        //! \brief Constructor
        ISimulator(QObject *parent = nullptr) : QObject(parent) {}

        //! \brief Destructor
        virtual ~ISimulator() {}

        //! \brief Are we connected to the simulator?
        virtual bool isConnected() const = 0;

        /*!
         * \brief Return user aircraft object
         * \return
         */
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const = 0;

    signals:
        //! \brief Emitted when the connection status has changed
        void connectionChanged(bool value);

        /*!
         * \brief Emitted when new a new data object of the user aircraft is received
         * \param aircraft
         */
        void ownAircraftReceived(BlackMisc::Aviation::CAircraft aircraft);
    };

} // namespace BlackCore

#endif // BLACKCORE_SIMULATOR_H
