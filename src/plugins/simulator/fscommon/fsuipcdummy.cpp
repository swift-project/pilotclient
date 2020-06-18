/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#if !defined(SWIFT_USING_FSUIPC32) && !defined(SWIFT_USING_FSUIPC64)

#include "fsuipc.h"

using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Weather;

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

        bool CFsuipc::open(bool force)
        {
            Q_UNUSED(force);
            return false;
        }

        void CFsuipc::close()
        { }

        bool CFsuipc::isOpened() const
        {
            return false;
        }

        bool CFsuipc::isOpen() const
        {
            return false;
        }

        bool CFsuipc::write(const CSimulatedAircraft &aircraft)
        {
            Q_UNUSED(aircraft);
            return false;
        }

        bool CFsuipc::write(const CTransponder &xpdr)
        {
            Q_UNUSED(xpdr);
            return false;
        }

        bool CFsuipc::write(const CWeatherGrid &weatherGrid)
        {
            Q_UNUSED(weatherGrid);
            return false;
        }

        bool CFsuipc::setSimulatorTime(int hour, int minute)
        {
            Q_UNUSED(hour);
            Q_UNUSED(minute);
            return false;
        }

        QString CFsuipc::getVersion() const
        {
            return QStringLiteral("N/A");
        }

        // cppcheck-suppress constParameter
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

#endif // !defined(SWIFT_USING_FSUIPC32) && !defined(SWIFT_USING_FSUIPC64)
