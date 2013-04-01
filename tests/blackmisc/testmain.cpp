#include "testmain.h"

namespace BlackMiscTest {

//! Starting main, equivalent to QTEST_APPLESS_MAIN for multiple test classes.
/*! \param argc
    \param argv
    \sa http://stackoverflow.com/questions/12194256/qt-how-to-organize-unit-test-with-more-than-one-class
*/
int CTestMain::unitMain(int argc, char *argv[])
{
    int status = 0;
    {
        CTestPhysicalQuantitiesBase pqBaseTests ;
        CTestAviationBase avBaseTests;
        status |= QTest::qExec(&pqBaseTests, argc, argv);
        status |= QTest::qExec(&avBaseTests, argc, argv);
    }
    return status;
}
}
