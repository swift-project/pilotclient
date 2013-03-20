#include "testmain.h"

namespace BlackCoreTest {

//! Starting main, equivalent to QTEST_APPLESS_MAIN for multiple test classes.
/*! \param argc
    \param argv
    \sa http://stackoverflow.com/questions/12194256/qt-how-to-organize-unit-test-with-more-than-one-class
*/
int TestMain::unitMain(int argc, char *argv[])
{
    int status = 0;
    {
        TestPhysicalQuantitiesBase pqBaseTests ;
        status |= QTest::qExec(&pqBaseTests, argc, argv);
    }
    return status;
}
}
