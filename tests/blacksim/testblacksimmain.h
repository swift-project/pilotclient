/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
