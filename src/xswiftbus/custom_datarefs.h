// SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_CUSTOM_DATAREFS_H
#define SWIFT_SIM_XSWIFTBUS_CUSTOM_DATAREFS_H

namespace XSwiftBus
{
    //! Is swift connected to a network?
    struct TSwiftNetworkConnected
    {
        //! Dataref name
        static constexpr const char *name() { return "org/swift-project/xswiftbus/connected"; }
        //! Can be written to?
        static constexpr bool writable = false;
        //! Dataref type
        using type = int;
        //! Not an array dataref
        static constexpr bool is_array = false;
    };

} // namespace XSwiftBus

#endif // SWIFT_SIM_XSWIFTBUS_CUSTOM_DATAREFS_H
