/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef SWIFT_USING_FSUIPC

#include "fsuipc.h"

using namespace BlackMisc::Simulation;

namespace BlackSimPlugin
{
    namespace FsCommon
    {
        //! Dummy FsuipcWeatherMessage
        struct CFsuipc::FsuipcWeatherMessage
        { };

        CFsuipc::CFsuipc(QObject *parent)
            : QObject(parent)
        { }

        CFsuipc::~CFsuipc()
        { }

        bool CFsuipc::connect()
        {
            return false;
        }

        void CFsuipc::disconnect()
        { }

        bool CFsuipc::isConnected() const
        {
            return false;
        }

        bool CFsuipc::write(const CSimulatedAircraft &aircraft)
        {
            Q_UNUSED(aircraft);
            return false;
        }

        bool CFsuipc::write(const BlackMisc::Weather::CWeatherGrid &weatherGrid)
        {
            Q_UNUSED(weatherGrid);
            return false;
        }

        QString CFsuipc::getVersion() const
        {
            return "N/A";
        }

        bool CFsuipc::read(CSimulatedAircraft &aircraft, bool cockpit, bool situation, bool aircraftParts)
        {
            Q_UNUSED(aircraft);
            Q_UNUSED(cockpit);
            Q_UNUSED(situation);
            Q_UNUSED(aircraftParts);
            return false;
        }

        void CFsuipc::timerEvent(QTimerEvent *event)
        {
            Q_UNUSED(event);
        }

    } // namespace
} // namespace

#endif //SWIFT_USING_FSUIPC
