/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FSXCOMMON_SIMCONNECTSYMBOLS_H
#define BLACKSIMPLUGIN_FSXCOMMON_SIMCONNECTSYMBOLS_H

#include <QtGlobal>

#ifdef Q_OS_WIN64

enum P3DSimConnectVersion
{
    P3DSimConnectv40,
    P3DSimConnectv41,
    P3DSimConnectv42,
    P3DSimConnectv43
};

//! Load and resolve versioned P3D SimConnect.
//! If a another version was already loaded previously, it won't unload it.
//! You have to call /sa unloadSimConnect() before.
bool loadAndResolveP3DSimConnect(P3DSimConnectVersion version);

#else

//! Load and resolve FSX SimConnect.
//! \param manifestProbing  Set to true if you want to try loading from the assembly cache with manifests.
//!                         Otherwise the library in the bin folder will be loaded.
bool loadAndResolveFsxSimConnect(bool manifestProbing);

#endif

#endif // guard
