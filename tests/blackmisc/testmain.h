/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISCTEST_TESTMAIN_H
#define BLACKMISCTEST_TESTMAIN_H

#include "testphysicalquantitiesbase.h"
#include "testaviationbase.h"
#include "testvectormatrix.h"
#include <QtTest/QtTest>

namespace BlackMiscTest
{

/*!
 * Class firing of all unit tests in this namespace.
 * Avoids clashes with other main(..) functions and allows to fire the test cases
 * simply from any other main.
 */
class CTestMain
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

#endif // BLACKMISCTEST_TESTMAIN_H
