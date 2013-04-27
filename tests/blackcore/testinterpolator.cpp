/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "testinterpolator.h"
using namespace BlackCore;

namespace BlackCoreTest
{

/*
 * Interpolator tests
 */
void CTestInterpolator::interpolatorBasics()
{
    CInterpolator c;
    QVERIFY2(!c.isValid(), "Default interpolator should not be valid");
}

} // namespace
