/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "simulator_xplane.h"

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CSimulatorXPlane::CSimulatorXPlane(QObject *parent) : BlackCore::ISimulator(parent)
        {
        }

        bool CSimulatorXPlane::isConnected() const
        {
            return false;
        }

        bool CSimulatorXPlane::canConnect()
        {
            return false;
        }

        bool CSimulatorXPlane::connectTo()
        {
            return false;
        }

        void CSimulatorXPlane::asyncConnectTo()
        {
        }

        bool CSimulatorXPlane::disconnectFrom()
        {
            return false;
        }

        BlackMisc::Aviation::CAircraft CSimulatorXPlane::getOwnAircraft() const
        {
            return {};
        }

        void CSimulatorXPlane::displayStatusMessage(const BlackMisc::CStatusMessage &message) const
        {
            Q_UNUSED(message);
        }

        BlackMisc::Network::CAircraftModel CSimulatorXPlane::getAircraftModel() const
        {
            return {};
        }

        bool CSimulatorXPlane::updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft)
        {
            Q_UNUSED(aircraft);
        }

    }
}
