/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISCTEST_TESTMAIN_H
#define BLACKMISCTEST_TESTMAIN_H

//! \cond PRIVATE_TESTS
//! \file
//! \ingroup testblackmisc

namespace BlackMiscTest
{
    /*!
     * Class firing all unit tests in this namespace.
     * Avoids clashes with other main(..) functions and allows to fire the test cases
     * simply from any other main.
     */
    class CBlackMiscTestMain
    {
    public:
        //! Starting all
        static int unitMain(int argc, char *argv[]);
    };
}

//! \endcond

#endif // guard
