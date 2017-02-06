/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS

/*!
 * \file
 * \ingroup testblackcore
 */

#include "testcontext.h"
#include "blackcore/context/contextallproxies.h"

using namespace BlackCore::Context;

namespace BlackCoreTest
{
    void CTestContext::contextInitTest()
    {
        // will cause asserts when signal connects fail

        CContextAudioProxy::unitTestRelaySignals();
        CContextNetworkProxy::unitTestRelaySignals();
        CContextOwnAircraftProxy::unitTestRelaySignals();
        CContextSimulatorProxy::unitTestRelaySignals();
    }
}

//! \endcond
