/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_SIMULATOR_XPLANE_H
#define BLACKSIMPLUGIN_SIMULATOR_XPLANE_H

//! \file

#include "blackcore/simulator.h"

namespace BlackSimPlugin
{
    namespace XPlane
    {

        /*!
         * X-Plane ISimulator implementation
         */
        class CSimulatorXPlane : public BlackCore::ISimulator
        {
            Q_OBJECT

        public:
            //! Constructor
            CSimulatorXPlane(QObject *parent = nullptr);

            //! \copydoc BlackCore::ISimulator::isConnected
            virtual bool isConnected() const override;

            //! \copydoc BlackCore::ISimulator::canConnect
            virtual bool canConnect() override;

        public slots:
            //! \copydoc BlackCore::ISimulator::connectTo
            virtual bool connectTo() override;

            //! \copydoc BlackCore::ISimulator::asyncConnectTo
            virtual void asyncConnectTo() override;

            //! \copydoc BlackCore::ISimulator::disconnectFrom
            virtual bool disconnectFrom() override;

            //! \copydoc BlackCore::ISimulator::getOwnAircraft
            virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const override;

            //! \copydoc BlackCore::ISimulator::addRemoteAircraft
            virtual void addRemoteAircraft(const BlackMisc::Aviation::CCallsign &/*callsign*/, const QString &/*type*/,
                const BlackMisc::Aviation::CAircraftSituation &/*initialSituation*/) override {}

            //! \copydoc BlackCore::ISimulator::addAircraftSituation
            virtual void addAircraftSituation(const BlackMisc::Aviation::CCallsign &/*callsign*/,
                const BlackMisc::Aviation::CAircraftSituation &/*initialSituation*/) override {}

            //! \copydoc BlackCore::ISimulator::removeRemoteAircraft
            virtual void removeRemoteAircraft(const BlackMisc::Aviation::CCallsign &/*callsign*/) override {}

            //! \copydoc BlackCore::ISimulator::updateOwnSimulatorCockpit
            virtual bool updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft) override;

            //! \copydoc BlackCore::ISimulator::getSimulatorInfo
            virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override { return BlackSim::CSimulatorInfo::XP(); }

            //! \copydoc BlackCore::ISimulator::displayStatusMessage
            virtual void displayStatusMessage(const BlackMisc::CStatusMessage &message) const override;

            //! \copydoc BlackCore::ISimulator::getAircraftModel
            virtual BlackMisc::Network::CAircraftModel getAircraftModel() const override;
        };
    
        //! Factory for creating CSimulatorXPlane instance
        class CSimulatorXPlaneFactory : public QObject, public BlackCore::ISimulatorFactory
        {
            Q_OBJECT
            Q_PLUGIN_METADATA(IID "net.vatsim.PilotClient.BlackCore.SimulatorInterface")
            Q_INTERFACES(BlackCore::ISimulatorFactory)

        public:
            //! \copydoc BlackCore::ISimulatorFactory::create
            virtual BlackCore::ISimulator *create(QObject *parent = nullptr) override { return new CSimulatorXPlane(parent); }

            //! \copydoc BlackCore::ISimulatorFactory::getSimulatorInfo
            virtual BlackSim::CSimulatorInfo getSimulatorInfo() const override { return BlackSim::CSimulatorInfo::XP(); }
        };

    }
}

#endif // guard
