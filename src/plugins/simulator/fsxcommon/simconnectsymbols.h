// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_FSXCOMMON_SIMCONNECTSYMBOLS_H
#define SWIFT_SIMPLUGIN_FSXCOMMON_SIMCONNECTSYMBOLS_H

#include <QtGlobal>

#include "plugins/simulator/fsxcommon/fsxcommonexport.h"

#ifdef Q_OS_WIN64

//! P3D versions
enum P3DSimConnectVersion
{
    P3DSimConnectv40,
    P3DSimConnectv41,
    P3DSimConnectv42,
    P3DSimConnectv43,
    P3DSimConnectv44,
    P3DSimConnectv45
};

//! String to the enum
FSXCOMMON_EXPORT P3DSimConnectVersion stringToP3DVersion(const QString &p3d);

//! Load and resolve versioned P3D SimConnect.
//! If a another version was already loaded previously, it won't unload it.
//! You have to call /sa unloadSimConnect() before.
FSXCOMMON_EXPORT bool loadAndResolveP3DSimConnect(P3DSimConnectVersion version);

//! Same but string version
inline bool loadAndResolveP3DSimConnectByString(const QString &version)
{
    return loadAndResolveP3DSimConnect(stringToP3DVersion(version));
}

FSXCOMMON_EXPORT bool loadAndResolveMSFSimConnect();

// FSXCOMMON_EXPORT bool loadAndResolveMSFS2024SimConnect();

#else

//! Load and resolve FSX SimConnect.
//! \param manifestProbing  Set to true if you want to try loading from the assembly cache with manifests.
//!                         Otherwise the library in the bin folder will be loaded.
FSXCOMMON_EXPORT bool loadAndResolveFsxSimConnect(bool manifestProbing);

#endif

#endif // SWIFT_SIMPLUGIN_FSXCOMMON_SIMCONNECTSYMBOLS_H
