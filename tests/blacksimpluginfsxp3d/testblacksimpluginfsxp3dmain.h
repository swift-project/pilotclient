/* Copyright (C) 2018
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIMPLUGINFSXP3D_TESTMAIN_H
#define BLACKSIMPLUGINFSXP3D_TESTMAIN_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testplugins

namespace BlackSimPluginFsxP3D
{
    /*!
     * Class firing all unit tests in this namespace.
     * Avoids clashes with other main(..) functions and allows to fire the test cases
     * simply from any other main.
     */
    class CBlackSimPluginFsxP3DTest
    {
    public:
        //! Starting all
        static int unitMain(int argc, char *argv[]);
    };
} // ns

//! \endcond

#endif // guard
