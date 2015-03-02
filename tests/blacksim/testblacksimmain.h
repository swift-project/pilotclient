/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMTEST_TESTMAIN_H
#define BLACKSIMTEST_TESTMAIN_H

#include <QtTest/QtTest>

namespace BlackSimTest
{

    /*!
     * Class firing all unit tests in this namespace.
     * Avoids clashes with other main(..) functions and allows to fire the test cases
     * simply from any other main.
     */
    class CBlackSimTestMain
    {
    public:
        /*!
         * \brief Starting all
         * \param argc
         * \param argv
         * \return
         */
        static int unitMain(int argc, char *argv[]);
    };
}

#endif // guard
