#ifndef TESTMAIN_H
#define TESTMAIN_H

#include <QtTest/QtTest>
#include "testphysicalquantitiesbase.h"

namespace BlackMiscTest{

/*!
 * Class firing of all unit tests in this namespace.
 * Avoids clashes with other main(..) functions and allows to fire the test cases
 * simply from any other main.
 */
class TestMain
{
public:
    static int unitMain(int argc, char *argv[]);
};
}

#endif // TESTMAIN_H
