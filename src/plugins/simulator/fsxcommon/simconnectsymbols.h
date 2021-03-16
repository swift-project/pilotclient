/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_SIMCONNECTSYMBOLS_H
#define BLACKSIMPLUGIN_FSXCOMMON_SIMCONNECTSYMBOLS_H

#include "plugins/simulator/fsxcommon/fsxcommonexport.h"

#include <QtGlobal>

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
inline bool loadAndResolveP3DSimConnectByString(const QString &version) { return loadAndResolveP3DSimConnect(stringToP3DVersion(version)); }

FSXCOMMON_EXPORT bool loadAndResolveMSFSimConnect();

#else

//! Load and resolve FSX SimConnect.
//! \param manifestProbing  Set to true if you want to try loading from the assembly cache with manifests.
//!                         Otherwise the library in the bin folder will be loaded.
FSXCOMMON_EXPORT bool loadAndResolveFsxSimConnect(bool manifestProbing);

#endif

#endif // guard
