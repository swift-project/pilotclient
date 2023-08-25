// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

// in P3Dv4 the simconnect.h does not include windows.h
// here we include windows.h first

#ifndef BLACKSIMPLUGIN_FSX_SIMCONNECTWINDOWS_H
#define BLACKSIMPLUGIN_FSX_SIMCONNECTWINDOWS_H

#ifndef NOMINMAX
#    define NOMINMAX
#endif

// clash with struct interface in objbase.h used to happen
#pragma push_macro("interface")
#undef interface

#include <Windows.h>
#include <SimConnect.h>

#pragma pop_macro("interface")

#ifndef Q_OS_WIN64
//! adding struct SIMCONNECT_DATA_PBH not existing in SimConnect FSX
struct SIMCONNECT_DATA_PBH
{
    double Pitch; //!< pitch
    double Bank; //!< bank
    double Heading; //!< heading
};
#endif

#endif // guard
