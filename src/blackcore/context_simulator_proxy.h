/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTSIMULATOR_PROXY_H
#define BLACKCORE_CONTEXTSIMULATOR_PROXY_H

#include "blackcore/context_simulator.h"
#include "blackmisc/genericdbusinterface.h"

namespace BlackCore
{
    //! \brief DBus proxy for Simulator Context
    class CContextSimulatorProxy : public IContextSimulator
    {
        Q_OBJECT
    public:
        /*!
         * \brief DBus version constructor
         * \param serviceName
         * \param connection
         * \param parent
         */
        CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection, QObject *parent = 0);

        //! Destructor
        ~CContextSimulatorProxy() {}

        /*!
         * \brief Using local objects?
         * \return
         */
        virtual bool usingLocalObjects() const override { return false; }

        virtual BlackMisc::Aviation::CAircraft ownAircraft() const override;

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface;

        /*!
         * Relay connection signals to local signals
         * No idea why this has to be wired and is not done automatically
         * \param connection
         */
        void relaySignals(const QString &serviceName, QDBusConnection &connection);

    protected:
        /*!
         * \brief CContextNetworkProxy
         * \param parent
         */
        CContextSimulatorProxy(QObject *parent = nullptr) : IContextSimulator(parent), m_dBusInterface(0) {}

    signals:

    public slots:

        //! \copydoc IContextSimulator::isConnected()
        virtual bool isConnected() const override;

        //virtual void ownAircraftUpdateReceived(BlackMisc::Aviation::CAircraft aircraft) override;

    };

} // namespace BlackCore

#endif // BLACKCORE_CONTEXTSIMULATOR_PROXY_H
