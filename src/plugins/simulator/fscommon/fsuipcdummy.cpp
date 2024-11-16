// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#if !defined(SWIFT_USING_FSUIPC32) && !defined(SWIFT_USING_FSUIPC64)

#    include "fsuipc.h"

using namespace swift::misc::simulation;
using namespace swift::misc::aviation;

namespace swift::simplugin
{
    namespace fscommon
    {
        CFsuipc::CFsuipc(QObject *parent)
            : QObject(parent)
        {}

        CFsuipc::~CFsuipc()
        {}

        bool CFsuipc::open(bool force)
        {
            Q_UNUSED(force);
            return false;
        }

        void CFsuipc::close()
        {}

        bool CFsuipc::isOpened() const
        {
            return false;
        }

        bool CFsuipc::isOpen() const
        {
            return false;
        }

        bool CFsuipc::write(const CTransponder &xpdr)
        {
            Q_UNUSED(xpdr);
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
    } // namespace fscommon
} // namespace swift::simplugin

#endif // !defined(SWIFT_USING_FSUIPC32) && !defined(SWIFT_USING_FSUIPC64)
