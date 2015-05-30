#include "testblackmiscmain.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QCoreApplication>
#include <QDebug>

using namespace BlackMiscTest;

/*!
 * Starter for test cases
 * \brief main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Q_UNUSED(a);

    BlackMisc::registerMetadata();
    return CBlackMiscTestMain::unitMain(argc, argv);;
}
