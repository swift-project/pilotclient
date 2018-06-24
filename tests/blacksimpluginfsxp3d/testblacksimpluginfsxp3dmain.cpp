/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testplugins

#include "testblacksimpluginfsxp3dmain.h"
#include "testfsxp3dcommon.h"
#include "blackmisc/test/test.h"
#include <QStringList>
#include <QTest>

namespace BlackSimPluginFsxP3D
{
    int CBlackSimPluginFsxP3DTest::unitMain(int argc, char *argv[])
    {
        BlackMisc::Test::CTest test(argc, argv);

        int status = 0;
        {
            CTestFsxP3DCommon commonTest;
            status |= test.exec(&commonTest, "blacksimpluginfsxp3d_fsxp3dcommon");
        }
        return status;
    }
} // namespace

//! \endcond
