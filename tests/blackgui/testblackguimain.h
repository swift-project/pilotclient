/* Copyright (C) 2013
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
 * \ingroup testblackgui
 */

#ifndef BLACKGUITEST_TESTMAIN_H
#define BLACKGUITEST_TESTMAIN_H

namespace BlackGuiTest
{

    /*!
     * Class firing all unit tests in this namespace.
     * Avoids clashes with other main(..) functions and allows to fire the test cases
     * simply from any other main.
     */
    class CBlackGuiTestMain
    {
    public:
        //! Unit tests
        static int unitMain(int argc, char *argv[]);
    };
}

//! \endcond

#endif // guard
