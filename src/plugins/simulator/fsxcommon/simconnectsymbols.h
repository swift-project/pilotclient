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

//! Load and resolve SimConnect.
//! \param manifestProbing  Set to true if you want to try loading from the assembly cache with manifests.
//!                         Otherwise the library in the bin folder will be loaded.
bool loadAndResolveSimConnect(bool manifestProbing);

#endif // guard
