/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblacksimplugin
 */

#include "testsimconnectsymbols.h"
#include "plugins/simulator/fsxcommon/simconnectsymbols.h"
#include "plugins/simulator/fsxcommon/simconnectwindows.h"
#include <QTest>

namespace BlackSimPluginFsxP3D
{
    void CTestSimconnectSymbols::resolveSymbols()
    {
        QVERIFY2(loadAndResolveSimConnect(false), "Could not load and resolve SimConnect library!");
        HANDLE hSimConnect;
        SimConnect_Open(&hSimConnect, "Test", nullptr, 0, 0, 0);
        SimConnect_Close(hSimConnect);
    }

} // ns

//! \endcond
